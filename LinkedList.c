// Copyright 2021 Rosu Mihai Cosmin 313CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

linked_list_t*
ll_create(unsigned int data_size)
{
    // Aloc memorie pentru lista.
    linked_list_t *ll = malloc(sizeof(linked_list_t));
    DIE(!ll, "Malloc failed");

    // Initializez campurile listei.
    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;
    return ll;
}

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    // Creez un nod nou ce urmeaza sa fie adaugat in lista.
	ll_node_t *new = malloc(sizeof(ll_node_t));
	DIE(!new, "Malloc failed");

    new->next = NULL;
    new->data = malloc(list->data_size);
    DIE(!(new->data), "Malloc failed");

    memcpy(new->data, new_data, list->data_size);

    if (n > list->size)
    	n = list->size;
    // Adaug elementul pe pozitia specificata.
    if (n == 0) {
    	ll_node_t *aux = list->head;
    	new->next = aux;
    	list->head = new;
    } else if (n == list->size) {
    	ll_node_t *aux = list->head;

    	while (aux->next != NULL)
    		aux = aux->next;
    	aux->next = new;
    } else {
    	unsigned int i = 1;
    	ll_node_t *prev, *curr;
    	prev = list->head;
    	curr = prev->next;

    	while (i < n) {
    		i++;
    		prev = prev->next;
    		curr = curr->next;
    	}
    	new->next = curr;
    	prev->next = new;
    }

    list->size++;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    if (list->head == NULL) {
    	fprintf(stderr, "There are no nodes to remove.");
    	return NULL;
    }

    if (n >= list->size)
    	n = list->size - 1;

    if (n == 0){
    	ll_node_t *aux = list->head;
    	list->head = aux->next;
    	list->size--;
    	return aux;
    } else {
    	unsigned int i = 1;
    	ll_node_t *prev, *curr;
    	prev = list->head;
    	curr = prev->next;

    	while (i < n){
    		i++;
    		prev = prev->next;
    		curr = curr->next;
    	}
    	prev->next = curr->next;
    	list->size--;
    	return curr;
    }
}

ll_node_t*
ll_get_nth_node(linked_list_t* list, unsigned int n)
{
    if (list->head == NULL) {
        fprintf(stderr, "There are no nodes to get.");
        return NULL;
    }

    if (n >= list->size)
        n = list->size - 1;

    ll_node_t* aux = list->head;
    for (unsigned int i = 0; i < n; i++)
        aux = aux->next;
    return aux;
}

unsigned int
ll_get_size(linked_list_t* list)
{
    return list->size;
}

void
ll_free(linked_list_t** pp_list)
{
    if (pp_list == NULL || *pp_list == NULL) {
        return;
    }

    while ((*pp_list)->size > 0) {
        // Elimin nodurile din lista si le eliberez memoria.
        ll_node_t* curr = ll_remove_nth_node(*pp_list, 0);
        free(curr->data);
        free(curr);
    }
    // Eliberez memoria listei si ii setez pointerul la NULL.
    free(*pp_list);
    *pp_list = NULL;
}
