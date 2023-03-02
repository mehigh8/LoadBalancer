// Copyright 2021 Rosu Mihai Cosmin 313CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "Hashtable.h"

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	// Aloc memoria pentru un hashtable si ii setez size-ul si hmax-ul.
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	DIE(!ht, "Malloc failed\n");
	ht->size = 0;
	ht->hmax = hmax;
	// Aloc vectorul de liste
	// iar pentru fiecare pozitie din vector creez lista.
	ht->buckets = malloc(ht->hmax * sizeof(linked_list_t *));
	for (unsigned int i = 0; i < hmax; i++)
		ht->buckets[i] = ll_create(sizeof(struct info));
	// Setez functiile de comparare.
	ht->hash_function = hash_function;
	ht->compare_function = compare_function;
	return ht;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[index]->head;
	int ok = 0;
	// Parcurg nod cu nod lista de pe pozitia index
	// si daca gasesc cheia specificata, doar modific valoarea.
	for (unsigned int i = 0; i < ht->buckets[index]->size; i++) {
		if (!ht->compare_function(key, ((struct info *)(node->data))->key)) {
			memcpy(((struct info *)(node->data))->value, value, value_size);
			ok = 1;
		}
		node = node->next;
	}
	// Daca nu gasesc cheia
	// adaug un nod nou care retine perechea cheie-valoare primita.
	if (!ok) {
		struct info aux;
		aux.key = malloc(key_size);
		DIE(!aux.key, "Malloc failed\n");
		aux.value = malloc(value_size);
		DIE(!aux.value, "Malloc failed\n");
		memcpy(aux.key, key, key_size);
		memcpy(aux.value, value, value_size);
		ll_add_nth_node(ht->buckets[index], ht->buckets[index]->size, &aux);
		ht->size++;
	}
}

void *
ht_get(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[index]->head;
	// Parcurg lista de pe pozitia index nod cu nod,
	// iar daca gasesc cheia returnez valoarea de la perechea acesteia.
	for (unsigned int i = 0; i < ht->buckets[index]->size; i++) {
		if (!ht->compare_function(key, ((struct info *)(node->data))->key))
			return ((struct info *)(node->data))->value;
		node = node->next;
	}
	return NULL;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[index]->head;
	// Parcurg lista de pe pozitia index nod cu nod
	// si daca gasesc cheia returnez 1, altfel 0.
	for (unsigned int i = 0; i < ht->buckets[index]->size; i++) {
		if (!ht->compare_function(key, ((struct info *)(node->data))->key))
			return 1;
		node = node->next;
	}
	return 0;
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[index]->head;
	unsigned int pos = 0;
	// Parcurg lista de pe pozitia index nod cu nod
	// si cand gasesc cheia ies din for si, in cazul in care nodul nu este
	// NULL, il elimin din lista si eliberez memoria din struct info,
	// din data si din nod.
	for (unsigned int i = 0; i < ht->buckets[index]->size; i++) {
		if (!ht->compare_function(key, ((struct info *)(node->data))->key)) {
			pos = i;
			break;
		}
		node = node->next;
	}
	if (node != NULL) {
		node = ll_remove_nth_node(ht->buckets[index], pos);
		free(((struct info *)(node->data))->value);
		free(((struct info *)(node->data))->key);
		free(node->data);
		free(node);
		ht->size--;
	}
}

void
ht_free(hashtable_t *ht)
{
	// Pentru fiecare lista din vectorul de liste,
	// eliberez memoria din fiecare struct info retinut in data, iar apoi
	// eliberez memoria listei prin functia ll_free.
	for (unsigned int i = 0; i < ht->hmax; i++) {
		ll_node_t *node = ht->buckets[i]->head;
		while (node) {
			free(((struct info *)(node->data))->value);
			free(((struct info *)(node->data))->key);
			node = node->next;
		}
		ll_free(&ht->buckets[i]);
	}
	// Eliberez memoria vectorului de liste si a hashtable-ului.
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
