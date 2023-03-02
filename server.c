// Copyright 2021 Rosu Mihai Cosmin 313CA
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

// Functia de comparare pentru hashtable.
int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}
// Functia de hash pentru hashtable.
unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

server_memory* init_server_memory() {
	// Am alocat memorie pentru server_memory.
	server_memory *sm = malloc(sizeof(server_memory));
	DIE(!sm, "Malloc server memory\n");
	// Am creat un nou hashtable. Am ales sa folosesc 10 bucket-uri.
	sm->ht = ht_create(MAX_BUCKETS, hash_function_string,
								compare_function_strings);
	return sm;
}

void server_store(server_memory* server, char* key, char* value) {
	// Pentru a stoca un nou element folosesc functia ht_put a hashtable-ului.
	ht_put(server->ht, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	// Pentru a elimina un element folosesc ht_remove_entry.
	ht_remove_entry(server->ht, key);
}

char* server_retrieve(server_memory* server, char* key) {
	// Verifica daca exista elementul cu cheia primita, caz in care il returnez.
	if (ht_has_key(server->ht, key))
		return (char *)ht_get(server->ht, key);
	// Altfel returnez nul.
	else
		return NULL;
}

void free_server_memory(server_memory* server) {
	// Eliberez memoria hashtable-ului si apoi a server_memory-ului.
	ht_free(server->ht);
	free(server);
}
