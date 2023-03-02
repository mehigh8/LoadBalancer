// Copyright 2021 Rosu Mihai Cosmin 313CA
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "utils.h"
// Structura folosita pentru a retine in hash_ring perechi de id
// si hash pentru fiecare server/replica de server adaugat.
struct pair_id_hash {
	int id;
	unsigned int hash;
};
// In load balancer am ales sa retin hash_ring-ul sub forma unei liste simplu
// inlantuite, un vector de servere in care servers[i] = server_memory-ul
// celui de-al i-ulea server, iar in final un server_count
struct load_balancer {
	linked_list_t *hash_ring;
	server_memory *servers[MAX_SERVERS];
	int server_count;
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}


load_balancer* init_load_balancer() {
	// Aloc memorie pentru un nou load_balancer.
	// Deoarece folosesc calloc vectorul de server_memory-uri va avea
	// toate elementele nule initial.
	load_balancer *lb = calloc(1, sizeof(load_balancer));
	DIE(!lb, "Calloc load balancer\n");
	// Creez lista hash_ring-ului.
	lb->hash_ring = ll_create(sizeof(pair_id_hash_t));
	// Setez server_count-ul la 0;
	lb->server_count = 0;
	return lb;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id)
{
	// Retin hash-ul cheii primite.
	unsigned int hash = hash_function_key(key);
	// Parcurg hash_ring-ul pana gasesc un element cu un hash mai mare.
	ll_node_t *aux = main->hash_ring->head;
	unsigned int i;
	for (i = 0; i < main->hash_ring->size; i++) {
		if (((pair_id_hash_t *)(aux->data))->hash > hash)
			break;
		aux = aux->next;
	}
	// Daca nu am gasit inseamna ca cheia primita are hash-ul mai mare decat
	// orice server, deci obiectul va fi adaugat pe primul server din hash_ring.
	if (i == main->hash_ring->size)
		*server_id = ((pair_id_hash_t *)(main->hash_ring->head->data))->id;
	// Daca am gasit un element cu hash mai mare, obiectul va fi adaugat pe
	// server-ul cu id-ul retinut de elementul gasit.
	else
		*server_id = ((pair_id_hash_t *)(aux->data))->id;
	// Obiectul este adaugat prin intermediul functiei server_store.
	server_store(main->servers[*server_id], key, value);
}

// In aceasta functie se parcurg aceeasi pasi ca in loader_store, doar ca
// la final se returneaza rezultatul functiei server_retrieve apelata pentru
// cheia primita si serverul determinat.
char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	unsigned int hash = hash_function_key(key);
	ll_node_t *aux = main->hash_ring->head;
	unsigned int i;
	for (i = 0; i < main->hash_ring->size; i++) {
		if (((pair_id_hash_t *)(aux->data))->hash > hash)
			break;
		aux = aux->next;
	}
	if (i == main->hash_ring->size)
		*server_id = ((pair_id_hash_t *)(main->hash_ring->head->data))->id;
	else
		*server_id = ((pair_id_hash_t *)(aux->data))->id;
	return server_retrieve(main->servers[*server_id], key);
}

// Functia aceasta adauga un element pe hash_ring si returneaza pozitia
// pe care a fost adaugat.
unsigned int
hash_ring_add_server(linked_list_t *ll, pair_id_hash_t pair)
{
	// Daca lista nu este goala trebuie cautat locul unde va fi adaugat
	// noul element.
	if (ll->size) {
		ll_node_t *curr = ll->head;
		unsigned int i;
		for (i = 0; i < ll->size; i++) {
			if (((pair_id_hash_t *)(curr->data))->hash >= pair.hash)
				break;
			curr = curr->next;
		}
		// Daca nu a fost gasit niciun element cu hash mai mare inseamna ca
		// noul element va fi adaugat pe ultima pozitie.
		if (i == ll->size) {
			ll_add_nth_node(ll, i, &pair);
			return i;
		// Daca a fost gasit un element si are acelasi hash cu noul element
		// atunci noul element va fi adaugat in functie de id.
		} else if (((pair_id_hash_t *)(curr->data))->hash == pair.hash) {
			if (((pair_id_hash_t *)(curr->data))->id > pair.id) {
				ll_add_nth_node(ll, i, &pair);
				return i;
			} else {
				ll_add_nth_node(ll, i + 1, &pair);
				return i + 1;
			}
		// Daca nu are acelasi hash, noul element este adaugat direct pe
		// pozitia gasita.
		} else {
			ll_add_nth_node(ll, i, &pair);
			return i;
		}
	}
	// Daca lista este goala, elementul este adaugat pe pozitia 0.
	ll_add_nth_node(ll, 0, &pair);
	return 0;
}

void loader_add_server(load_balancer* main, int server_id) {
	// Se initializeaza server_memory-ul noului server.
	main->servers[server_id] = init_server_memory();
	// Se adauga in hash_ring cele 3 replici ale server-ului.
	for (int i = 0; i < 3; i++) {
		pair_id_hash_t pair;
		pair.id = server_id;
		int tmp = i * 100000 + server_id;
		pair.hash = hash_function_servers(&tmp);
		unsigned int index = hash_ring_add_server(main->hash_ring, pair);
		// Daca nu este primul server adaugat, el va trebui sa preia din
		// load-ul celorlalte servere (vecinului din dreapta).
		if (main->server_count) {
			// Daca replica server-ului ajunge pe prima pozitie in hash_ring
			// ea va prelua elementele cu hash-ul mai mare decat orice server
			// sau mai mici decat orice server.
			if (!index) {
				// prev reprezinta replica serverului precedent din punct de vedere al
				// hash-ului, iar next este serverul de pe care se va prelua load-ul.
				ll_node_t *prev = ll_get_nth_node(main->hash_ring, main->hash_ring->size);
				ll_node_t *next = ll_get_nth_node(main->hash_ring, index + 1);
				int next_id = ((pair_id_hash_t *)(next->data))->id;
				// Daca server-ul din dreapta are acelasi id inseamna ca nu are rost
				// preluarea load-ului intrucatserver_memory-ul este acelasi.
				if (server_id == next_id)
					continue;
				// Parcurg hashtable-ul din server_memory si pentru fiecare element
				// recalculez hash-ul si verific daca se afla in intervalul necesar.
				for (unsigned int j = 0; j < main->servers[next_id]->ht->hmax; j++) {
					ll_node_t *aux = main->servers[next_id]->ht->buckets[j]->head;
					while (aux) {
						char *key = (char *)(((struct info *)(aux->data))->key);
						char *value = (char *)(((struct info *)(aux->data))->value);
						unsigned int tmp_hash = hash_function_key(key);
						aux = aux->next;
						if (tmp_hash > ((pair_id_hash_t *)(prev->data))->hash
							|| tmp_hash < pair.hash) {
							server_store(main->servers[server_id], key, value);
							server_remove(main->servers[next_id], key);
						}
					}
				}
			// Daca replica nu este prima in hash_ring, se intampla tot acelasi lucru,
			// cu exceptia conditiei de preluare a load-ului.
			} else {
				ll_node_t *prev = ll_get_nth_node(main->hash_ring, index - 1);
				ll_node_t *next;
				if (index == main->hash_ring->size - 1)
					next = ll_get_nth_node(main->hash_ring, 0);
				else
					next = ll_get_nth_node(main->hash_ring, index + 1);
				int next_id = ((pair_id_hash_t *)(next->data))->id;
				if (server_id == next_id)
					continue;
				for (unsigned int j = 0; j < main->servers[next_id]->ht->hmax; j++) {
					ll_node_t *aux = main->servers[next_id]->ht->buckets[j]->head;
					while (aux) {
						char *key = (char *)(((struct info *)(aux->data))->key);
						char *value = (char *)(((struct info *)(aux->data))->value);
						unsigned int tmp_hash = hash_function_key(key);
						aux = aux->next;
						// In acest caz preluarea load-ului are loc daca hash-ul unui
						// element este intre hash-ul serverului precedent si hash-ul
						// serverului curent.
						if (tmp_hash > ((pair_id_hash_t *)(prev->data))->hash
							&& tmp_hash < pair.hash) {
							server_store(main->servers[server_id], key, value);
							server_remove(main->servers[next_id], key);
						}
					}
				}
			}
		}
	}
	// In final creste server_countul.
	main->server_count++;
}

// Functia elimina toate replicile unui server din hash_ring.
void hash_ring_remove_server(linked_list_t *ll, int server_id)
{
	ll_node_t *aux = ll->head;
	for (unsigned int i = 0; i < ll->size; i++) {
		if (((pair_id_hash_t *)(aux->data))->id == server_id) {
			aux = aux->next;
			ll_node_t *removed = ll_remove_nth_node(ll, i);
			i--;
			free(removed->data);
			free(removed);
		} else {
			aux = aux->next;
		}
	}
}

void loader_remove_server(load_balancer* main, int server_id) {
	// Sunt eliminate replicile server-ului ce trebuie scos.
	hash_ring_remove_server(main->hash_ring, server_id);
	// Parcurg hashtable-ul server-ului ce trebuie scos, si adaug elementele
	// la noi servere in functie de hash.
	for (unsigned int j = 0; j < main->servers[server_id]->ht->hmax; j++) {
		ll_node_t *aux = main->servers[server_id]->ht->buckets[j]->head;
		while (aux) {
			char *key = (char *)(((struct info *)(aux->data))->key);
			char *value = (char *)(((struct info *)(aux->data))->value);
			int tmp;
			aux = aux->next;
			loader_store(main, key, value, &tmp);
			server_remove(main->servers[server_id], key);
		}
	}
	// Eliberez memoria server-ului si decrementez server_count-ul.
	free_server_memory(main->servers[server_id]);
	main->servers[server_id] = NULL;
	main->server_count--;
}

void free_load_balancer(load_balancer* main) {
	// Eliberez memoria tuturor serverelor.
    for (int i = 0; i < MAX_SERVERS; i++)
    	if (main->servers[i] != NULL) {
    		free_server_memory(main->servers[i]);
    	}
    // Eliberez memoria hash_ring-ului si a load_balancer-ului.
    ll_free(&(main->hash_ring));
    free(main);
}
