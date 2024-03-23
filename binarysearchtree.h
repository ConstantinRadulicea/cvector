#ifndef __BINARYSEARCHTREE_H__
#define __BINARYSEARCHTREE_H__

#include <stdint.h>

#define BINARYSEARCHTREE_SUCCESS 0
#define BINARYSEARCHTREE_DUPLICATE 1
#define BINARYSEARCHTREE_EMPTY_TREE 2


struct binarysearchtree_node_s;

typedef int (*binarytree_cmp_data_function)(void* data1, void* data2, void* _ctx);
typedef void (*binarytree_delete_tree_function)(struct binarysearchtree_node_s* node, void* _ctx);
typedef int (*binarytree_iterator_routine)(struct binarysearchtree_node_s* node, void* _ctx);


typedef struct binarysearchtree_node_s {
	void* data;
	struct binarysearchtree_node_s* left;
	struct binarysearchtree_node_s* right;
}binarysearchtree_node;


typedef struct binarysearchtree_s {
	struct binarysearchtree_node_s* head;
	binarytree_cmp_data_function cmp_data_function;
	void* cmp_data_ctx;
}binarysearchtree;


void binarysearchtree_node_init(binarysearchtree_node* root, void* _data);
void binarysearchtree_init(binarysearchtree* _binarytree, binarytree_cmp_data_function cmp_data_function, void* cmp_data_ctx);

int binarysearchtree_node_insert(binarysearchtree_node* root, binarysearchtree_node* new_node, binarytree_cmp_data_function cmp_function, void* ctx);
int binarysearchtree_insert(binarysearchtree* _binarytree, binarysearchtree_node* new_node);

binarysearchtree_node* binarysearchtree_node_find(binarysearchtree_node* root, void* _data, binarytree_cmp_data_function cmp_function, void* ctx);
binarysearchtree_node* binarysearchtree_find(binarysearchtree* _binarytree, void* _data);

// returns the new head, deleted_node if not NULL will contain the deleted node
binarysearchtree_node* binarysearchtree_node_delete(binarysearchtree_node* root, void* _data, binarysearchtree_node** deleted_node, binarytree_cmp_data_function cmp_function, void* ctx);

// returns the deleted node
binarysearchtree_node* binarysearchtree_delete(binarysearchtree* _binarytree, void* _data);

void binarysearchtree_delete_tree(binarysearchtree* _binarytree, binarytree_delete_tree_function delete_routine, void* _ctx);
void binarysearchtree_delete_sub_tree(binarysearchtree_node* sub_tree, binarytree_delete_tree_function delete_routine, void* _ctx);

// iterate the three while the function routine returns 0
int binarysearchtree_node_inorder_iterator(binarysearchtree_node* root, binarytree_iterator_routine routine, void* ctx);
int binarysearchtree_inorder_iterator(binarysearchtree* _binarytree, binarytree_iterator_routine routine, void* ctx);

// iterate the three while the function routine returns 0
int binarysearchtree_node_preorder_iterator(binarysearchtree_node* root, binarytree_iterator_routine routine, void* ctx);
int binarysearchtree_preorder_iterator(binarysearchtree* _binarytree, binarytree_iterator_routine routine, void* ctx);


// iterate the three while the function routine returns 0
int binarysearchtree_node_postorder_iterator(binarysearchtree_node* root, binarytree_iterator_routine routine, void* ctx);
int binarysearchtree_postorder_iterator(binarysearchtree* _binarytree, binarytree_iterator_routine routine, void* ctx);


binarysearchtree_node* binarysearchtree_max(binarysearchtree* _binarytree);
binarysearchtree_node* binarysearchtree_node_max(binarysearchtree_node* root);

binarysearchtree_node* binarysearchtree_min(binarysearchtree* _binarytree);
binarysearchtree_node* binarysearchtree_node_min(binarysearchtree_node* root);

#endif // !__BINARYSEARCHTREE_H__
