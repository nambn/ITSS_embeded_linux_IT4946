/*
* @Author: Nam
* @Date:   2018-04-28 18:39:15
* @Last Modified by:   Nam
* @Last Modified time: 2018-05-15 04:58:01
*/

#include <stdio.h>
#include <stdlib.h>

struct node {
	int           value;
	struct node*  left;
	struct node*  right;
};

void AddTree(struct node**, int);
void WalkTree(struct node*);
void FreeTree(struct node*);

/* Tree sort */
int main(void)
{
	struct node* rootp = 0;
	int i;
	int array[15] = {50, 12, 18, 70, 41, 19, 91, 1, 7, 6, 81, 65, 55, 20, 0};

	for (i = 0; i < 15; i++) {
		AddTree(&rootp, array[i]);
	}

	WalkTree(rootp);
	printf("\n");
	FreeTree(rootp);
	return 0;
}

void AddTree(struct node** pp, int val)
{
	/* create new node if *p is null */
	if (*pp == NULL ) {
		struct node *temp = (struct node *)malloc(sizeof(struct node));
		temp->value = val;
		temp->left = NULL;
		temp->right = NULL;
		*pp = temp;
	}
	else if ((*pp)->value > val) {
		AddTree(&(*pp)->left, val);
	}
	else if ((*pp)->value < val) {
		AddTree(&(*pp)->right, val);
	}
	/* (*pp)->value == val */
	else {
		/* do nothing */
	}
}

void WalkTree(struct node* p)
{
	if (p == NULL){
		return;
	}
	WalkTree(p->left);
	printf("%d ", p->value);
	WalkTree(p->right);
}

void FreeTree(struct node* p)
{
	if (p == NULL){
		return;
	}
	FreeTree(p->left);
	FreeTree(p->right);
	free(p);
	return;
}
