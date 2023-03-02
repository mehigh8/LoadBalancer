// Copyright 2021 Rosu Mihai Cosmin 313CA
#ifndef SERVER_H_
#define SERVER_H_

#include "Hashtable.h"
#define MAX_BUCKETS 10

typedef struct server_memory server_memory;

// Am reprezentat server_memory-ul printr-un hashtable.
struct server_memory {
	hashtable_t *ht;
};

int compare_function_strings(void *a, void *b);

unsigned int hash_function_string(void *a);

server_memory* init_server_memory();

void free_server_memory(server_memory* server);

/**
 * server_store() - Stores a key-value pair to the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 */
void server_store(server_memory* server, char* key, char* value);

/**
 * server_remove() - Removes a key-pair value from the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 */
void server_remove(server_memory* server, char* key);

/**
 * server_remove() - Gets the value associated with the key.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 *
 * Return: String value associated with the key
 *         or NULL (in case the key does not exist).
 */
char* server_retrieve(server_memory* server, char* key);

#endif  // SERVER_H_
