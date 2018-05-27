#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/single-linked-list-lib.h"

typedef struct address
{
	char name[30];
	char tel[15];
	char email[50];
} address_t ;

address_t new_addr (char *name, char* tel, char* email)
{
	address_t temp;
	strcpy(temp.name, name);
	strcpy(temp.tel, tel);
	strcpy(temp.email, email);
	return temp;
}

void address_print(sllnode *node)
{
	address_t *t = (address_t*) node->data;
	printf("%s - %s - %s\n", t->name, t->tel, t->email);
}

int search(sllnode *node)
{
	address_t *t = (address_t*) node->data;
	char *s;
	printf("Enter search term: ");
	scanf(" %[^\n]", s);
	getchar();
	if (strcmp(t->name, s) == 0) {
		return 1;
	}
	return 0;
}

int main() {
	sll *addrList = sll_create();

	address_t temp = new_addr("nam", "123456", "frozen_cat@nyan.com");
	sll_append(addrList, sll_new_node(&temp));

	address_t temp2 = new_addr("fox", "0987654", "melting_snow@nyan.com");
	sll_append(addrList, sll_new_node(&temp2));
	sll_traverse(addrList, &address_print);
	printf("list size: %d\n", addrList->size);
	puts("> append, traverse work\n");

	address_t temp3 = new_addr("nyan", "21321423", "admin@nyan.com");
	sll_prepend(addrList, sll_new_node(&temp3));
	sll_traverse(addrList, &address_print);
	printf("list size: %d\n", addrList->size);
	puts("> prepend work\n");

	address_t temp4 = new_addr("glad", "i2435234", "guide@nyan.com");
	sll_insert_after(addrList, sll_get_node(addrList, 2), sll_new_node(&temp4));
	sll_traverse(addrList, &address_print);
	puts("> insert before work\n");

	address_t temp5 = new_addr("error", "123435435", "reer@nyan.com");
	sll_insert_before(addrList, sll_get_node(addrList, 2), sll_new_node(&temp5));
	sll_traverse(addrList, &address_print);
	printf("list size: %d\n", addrList->size);
	puts("> insert after work\n");

	address_print(sll_get_last_node(addrList));
	printf("list size: %d\n", addrList->size);
	puts("> get last node work\n");

	address_print(sll_get_node(addrList, 3));
	printf("list size: %d\n", addrList->size);
	puts("> get node at index work\n");

	address_print(sll_get_prev_node(addrList, sll_get_node(addrList, 3)));
	printf("list size: %d\n", addrList->size);
	puts("> get previous node work\n");

	sll_traverse(sll_reverse(addrList), &address_print);
	puts("> reverse list work\n");

	address_print(sll_pop_head (addrList));
	address_print(sll_pop_tail (addrList));
	printf("list size: %d\n", addrList->size);
	puts("> pop head and tail work\n");

	address_print(sll_pop(addrList,sll_get_node(addrList,2)));
	printf("list size: %d\n", addrList->size);
	puts("> pop at index work\n");

	sllnode *found = sll_search(addrList, &search);
	if (found == NULL){
		puts("Node not found!");
	} else {
		puts("Found!");
		address_print(found);
	}

	sll_destroy(addrList);
	return 0;
}
