#include "binarysearchtree.h"


void binarysearchtree_init(binarysearchtree_node* _binarytree) {
	_binarytree->data = NULL;
	_binarytree->left = NULL;
	_binarytree->right = NULL;
}


// https://www.geeksforgeeks.org/insertion-in-binary-search-tree/
int binarysearchtree_insert(binarysearchtree_node* root, binarysearchtree_node* new_node, int (*cmp_function)(void* data1, void* data2, void* _ctx), void* ctx) {
	binarysearchtree_node* prev;
	binarysearchtree_node* temp;

	if (root == NULL) {
		return BINARYSEARCHTREE_EMPTY_TREE;
	}
	if (new_node == NULL) {
		return BINARYSEARCHTREE_SUCCESS;
	}
	prev = NULL;
	temp = root;

	while (temp) {
        
		if (cmp_function(root->data, new_node->data, ctx) > 0) {
			prev = temp;
			temp = temp->left;
		}
		else if (cmp_function(root->data, new_node->data, ctx) < 0) {
			prev = temp;
			temp = temp->right;
		}
		else {
			return BINARYSEARCHTREE_DUPLICATE;
		}
	}
    
	if (cmp_function(prev->data, new_node->data, ctx) > 0) {
		prev->left = new_node;
	}
	else {
		prev->right = new_node;
	}

	return BINARYSEARCHTREE_SUCCESS;
}


binarysearchtree_node* binarysearchtree_search(binarysearchtree_node* root, void* _data, int (*cmp_function)(void* data1, void* data2, void* _ctx), void* ctx) {
	binarysearchtree_node* temp;

	if (root == NULL) {
		return NULL;
	}
	temp = root;
    
	while (temp) {
		if (cmp_function(root->data, _data, ctx) > 0) {
			temp = temp->left;
		}
		else if (cmp_function(root->data, _data, ctx) < 0) {
			temp = temp->right;
		}
		else {
			return temp;
		}
	}
	return NULL;
}

// Iterative Function to delete
// 'key' from the BST.
// https://www.geeksforgeeks.org/binary-search-tree-set-3-iterative-delete/
binarysearchtree_node* binarysearchtree_delete(binarysearchtree_node* root, void* _data, binarysearchtree_node* data_node, int (*cmp_function)(void* data1, void* data2, void* _ctx), void* ctx)
{
    binarysearchtree_node* curr = root;
    binarysearchtree_node* prev = NULL;
    binarysearchtree_node* newCurr;
    binarysearchtree_node* p = NULL;
    binarysearchtree_node* temp;

    // Check if the key is actually
    // present in the BST.
    // the variable prev points to
    // the parent of the key to be deleted.
    
    while (curr != NULL && (cmp_function(curr->data, _data, ctx) != 0)) {
        prev = curr;
        if (cmp_function(_data, curr->data, ctx) < 0) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
    }

    // node not found
    if (curr == NULL) {
        data_node = NULL;
        return root;
    }

    // Check if the node to be
    // deleted has atmost one child.
    if (curr->left == NULL || curr->right == NULL) {

        // newCurr will replace
        // the node to be deleted.
        

        // if the left child does not exist.
        if (curr->left == NULL)
            newCurr = curr->right;
        else
            newCurr = curr->left;

        // check if the node to
        // be deleted is the root.
        if (prev == NULL)
            return newCurr;

        // check if the node to be deleted
        // is prev's left or right child
        // and then replace this with newCurr
        if (curr == prev->left)
            prev->left = newCurr;
        else
            prev->right = newCurr;

        // free memory of the
        // node to be deleted.
        data_node = curr;
        // free(curr);
    }

    // node to be deleted has
    // two children.
    else {
        // Compute the inorder successor
        temp = curr->right;
        while (temp->left != NULL) {
            p = temp;
            temp = temp->left;
        }

        // check if the parent of the inorder
        // successor is the curr or not(i.e. curr=
        // the node which has the same data as
        // the given data by the user to be
        // deleted). if it isn't, then make the
        // the left child of its parent equal to
        // the inorder successor'd right child.
        if (prev->left == curr) {
            prev->left = temp;
        }
        else {
            prev->right = temp;
        }
        temp->left = curr->left;
        if (p != NULL) {
            p->left = temp->right;
            temp->right = curr->right;
        }
        
        // if the inorder successor was the
        // curr (i.e. curr = the node which has the
        // same data as the given data by the
        // user to be deleted), then make the
        // right child of the node to be
        // deleted equal to the right child of
        // the inorder successor.
        else {
            // curr->right = temp->right;
        }

        //curr->data = temp->data;
        data_node = curr;
        //free(temp);
    }
    return root;
}


// iterate the three while the function routine returns 0
int binarysearchtree_iterator(binarysearchtree_node* root, int (*routine)(binarysearchtree_node* node, void* _ctx), void* ctx)
{
    if (root == NULL)
        return 0;

    // First recur on left child
    if (binarysearchtree_iterator(root->left, routine, ctx) != 0) {
        return 1;
    }

    // Then print the data of node
    if (routine(root, ctx) != 0) {
        return 1;
    }
    //printf("%d ", root->data);

    // Now recur on right child
    return binarysearchtree_iterator(root->right, routine, ctx);
}