// Copyright 2021 Rosu Mihai Cosmin 313CA
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "LinkedList.h"

struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	// Array de liste simplu-inlantuite.
	linked_list_t **buckets;
	// Nr. total de noduri existente curent in toate bucket-urile.
	unsigned int size;
	// Nr. de bucket-uri.
	unsigned int hmax;
	// (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor.
	unsigned int (*hash_function)(void*);
	// (Pointer la) Functie pentru a compara doua chei.
	int (*compare_function)(void*, void*);
};

// Functia creeaza un hashtable cu hmax bucket-uri, si cu functii de
// comparare si de hash precizate.
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));

// Functia adauga un element in hashtable.
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);

// Functia preia un element din hashtable in functie de cheie.
void *
ht_get(hashtable_t *ht, void *key);

// Functia verifica daca exista cheia precizata in hashtable.
int
ht_has_key(hashtable_t *ht, void *key);

// Functia elimina un element din hashtable.
void
ht_remove_entry(hashtable_t *ht, void *key);

// Functia returneaza size-ul hashtable-ului.
unsigned int
ht_get_size(hashtable_t *ht);

// Functia returneaza hmax-ul hashtable-ului.
unsigned int
ht_get_hmax(hashtable_t *ht);

// Functia elibereaza memoria hashtable-ului.
void
ht_free(hashtable_t *ht);

#endif  // HASHTABLE_H_
