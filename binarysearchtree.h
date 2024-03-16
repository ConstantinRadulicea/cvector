#ifndef __BINARYSEARCHTREE_H__
#define __BINARYSEARCHTREE_H__

#include <stdint.h>

#define BINARYSEARCHTREE_SUCCESS 0
#define BINARYSEARCHTREE_DUPLICATE 1
#define BINARYSEARCHTREE_EMPTY_TREE 2


struct binarysearchtree_node;

typedef int (*binarytree_cmp_data_function)(void* data1, void* data2, void* _ctx);

typedef struct binarytree_node {
	void* data;
	struct binarysearchtree_node* left;
	struct binarysearchtree_node* right;
}binarysearchtree_node;

typedef struct binarysearchtree {
	struct binarysearchtree_node* head;
	binarytree_cmp_data_function cmp_data_function;
	void* cmp_data_ctx;
}binarysearchtree;

void binarysearchtree_node_init(binarysearchtree_node* _binarytree);
void binarysearchtree_init(binarysearchtree* _binarytree, binarytree_cmp_data_function cmp_data_function, void* cmp_data_ctx);

int binarysearchtree_node_insert(binarysearchtree_node* head, binarysearchtree_node* new_node, binarytree_cmp_data_function cmp_function, void* ctx);
int binarysearchtree_insert(binarysearchtree* head, binarysearchtree_node* new_node);

binarysearchtree_node* binarysearchtree_node_search(binarysearchtree_node* root, void* _data, binarytree_cmp_data_function cmp_function, void* ctx);
binarysearchtree_node* binarysearchtree_search(binarysearchtree* root, void* _data);

// returns the new head, data_node if not NULL will contain the deleted node
binarysearchtree_node* binarysearchtree_node_delete(binarysearchtree_node* root, void* _data, binarysearchtree_node** data_node, binarytree_cmp_data_function cmp_function, void* ctx);

// returns the deleted node
binarysearchtree_node* binarysearchtree_delete(binarysearchtree* root, void* _data);

// iterate the three while the function routine returns 0
int binarysearchtree_iterator(binarysearchtree_node* root, int (*routine)(binarysearchtree_node* node, void* _ctx), void* ctx);


#endif // !__BINARYSEARCHTREE_H__
