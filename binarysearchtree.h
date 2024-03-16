#ifndef __BINARYSEARCHTREE_H__
#define __BINARYSEARCHTREE_H__

#include <stdint.h>

#define BINARYSEARCHTREE_SUCCESS 0
#define BINARYSEARCHTREE_DUPLICATE 1
#define BINARYSEARCHTREE_EMPTY_TREE 2


struct binarysearchtree_node;

typedef struct binarytree_node {
	void* data;
	struct binarysearchtree_node* left;
	struct binarysearchtree_node* right;
}binarysearchtree_node;


void binarysearchtree_init(binarysearchtree_node* _binarytree);

int binarysearchtree_insert(binarysearchtree_node* head, binarysearchtree_node* new_node, int (*cmp_function)(void* data1, void* data2, void* _ctx), void* ctx);

binarysearchtree_node* binarysearchtree_search(binarysearchtree_node* root, void* _data, int (*cmp_function)(void* data1, void* data2, void* _ctx), void* ctx);

binarysearchtree_node* binarysearchtree_delete(binarysearchtree_node* root, void* _data, binarysearchtree_node* data_node, int (*cmp_function)(void* data1, void* data2, void* _ctx), void* ctx);

// iterate the three while the function routine returns 0
int binarysearchtree_iterator(binarysearchtree_node* root, int (*routine)(binarysearchtree_node* node, void* _ctx), void* ctx);


#endif // !__BINARYSEARCHTREE_H__
