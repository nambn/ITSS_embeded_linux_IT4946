#include <stdlib.h>
#include "single-linked-list-lib.h"

sll* sll_create(void)
{
	sll *list = (sll*) malloc( sizeof(sll) );
	if (list != NULL) {
		list->head = NULL;
		list->size = 0;
	}
	return list;
}

void sll_destroy(sll* list)
{
	sllnode *nxt = list->head;
	sllnode *current = list->head;
	while (current != NULL) {
		nxt = nxt ->next;
		free(current);
		current = nxt;
	}
	free(list);
}

sllnode *sll_new_node (void *data)
{
	sllnode *newNode = (sllnode*) malloc (sizeof (sllnode));
	if (newNode != NULL) {
		newNode->data = data;
		newNode->next = NULL;
	}
	return newNode;
}

sllnode *sll_get_node (sll *list, int i)
{
	if (i > list->size || i < 1) {
		return NULL;
	}
	sllnode *current = list->head;
	for (int j = 1; j < i; j++){
		current = current->next;
	}
	return current;
}

sllnode *sll_get_last_node (sll *list)
{
	sllnode *current = list->head;
	while (current->next != NULL){
		current = current->next;
	}
	return current;
}

sllnode *sll_get_prev_node (sll *list, sllnode *node)
{
	sllnode *current = list->head;
	while (current != NULL) {
		if (current->next == node){
			break;
		}
		current = current->next;
	}
	return current;
}

int sll_traverse(sll* list, void (*show_data)(sllnode* node))
{
	sllnode *current = list->head;
	while (current != NULL) {
		show_data(current);
		current = current->next;
	}
	return 0;
}

sllnode *sll_search(sll *list, int (*search_data)(sllnode *node))
{
	sllnode *current = list->head;
	while (current != NULL) {
		if (search_data(current)) {
			break;
		}
		current = current->next;
	}
	return current;
}

int sll_prepend (sll *list, sllnode *node)
{
	if (node == NULL || list == NULL) {
		return -1;
	}
	if (list->size != 0) {
		node->next = list->head;
	}
	list->head = node;
	list->size++;
	return 0;
}

int sll_append (sll *list, sllnode *node)
{
	if (list == NULL || node == NULL) {
		return -1;
	}
	if (list->size == 0) {
		list->head = node;
	} else {
		sll_get_last_node(list)->next = node;
	}
	list->size++;
	return 0;
}

int sll_insert_after (sll *list , sllnode *prev, sllnode *node)
{
	if (list == NULL || prev == NULL || node == NULL) {
		return -1;
	}
	if (prev == sll_get_last_node(list)) {
		prev->next = node;
	} else {
		node->next = prev->next;
		prev->next = node;
	}
	list->size++;
	return 0;
}

int sll_insert_before (sll* list, sllnode *nxt, sllnode *node)
{
	if (list == NULL || nxt == NULL || node == NULL) {
		return -1;
	}
	if (nxt == list->head) {
		node->next = nxt;
		list->head = node;
	} else {
		sll_get_prev_node(list, nxt)->next = node;
		node->next = nxt;
	}
	list->size++;
	return 0;
}

sllnode *sll_pop_head (sll* list)
{
	if (list->size == 0) {
		return NULL;
	}
	sllnode *ripnode;
	ripnode = list->head;
	list->head = list->head->next;
	ripnode->next = NULL;
	list->size--;
	return ripnode;
}

sllnode *sll_pop_tail (sll* list)
{
	if (list->size == 0) {
		return NULL;
	}
	sllnode *ripnode;
	if (list->size == 1){
		ripnode = list->head;
		list->head = NULL;
		ripnode->next = NULL;
	} else {
		ripnode = sll_get_last_node(list);
		sll_get_prev_node(list, ripnode)->next = NULL;
	}
	list->size--;
	return ripnode;
}

sllnode *sll_pop (sll* list, sllnode *node)
{
	if (list->size == 0) {
		return NULL;
	}
	if (list->head == node){
		return sll_pop_head(list);
	}
	sll_get_prev_node(list, node)->next = node->next;
	node->next = NULL;
	list->size--;
	return node;
}

sll *sll_reverse (sll* list)
{
	sllnode *current = NULL;
	sllnode *prev = NULL;
	while (list->head != NULL) {
		current = list->head;
		list->head = list->head->next;
		current->next = prev;
		prev = current;
	}
	list->head = current;
	return list;
}
