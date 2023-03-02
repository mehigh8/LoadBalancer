// Copyright 2021 Rosu Mihai Cosmin 313CA
#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

linked_list_t*
ll_create(unsigned int data_size);

// Functia adauga un nod pe pozitia n in lista.
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* data);

// Functia elimina nodul de pe pozitia n din lista.
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n);

// Functia preia nodul de pe pozitia n din lista.
ll_node_t*
ll_get_nth_node(linked_list_t* list, unsigned int n);

// Functia returneaza dimensiunea listei.
unsigned int
ll_get_size(linked_list_t* list);

// Functia elibereaza memoria listei.
void
ll_free(linked_list_t** pp_list);

#endif  // LINKEDLIST_H_
