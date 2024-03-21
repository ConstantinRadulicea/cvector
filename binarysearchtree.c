#include "binarysearchtree.h"
#include <stddef.h>


int binarysearchtree_insert(binarysearchtree* _binarytree, binarysearchtree_node* new_node) {
    if (_binarytree->head == NULL) {
        _binarytree->head = new_node;
        return BINARYSEARCHTREE_SUCCESS;
    }
    return binarysearchtree_node_insert(_binarytree->head, new_node, _binarytree->cmp_data_function, _binarytree->cmp_data_ctx);
}

binarysearchtree_node* binarysearchtree_find(binarysearchtree* _binarytree, void* _data) {
    return binarysearchtree_node_find(_binarytree->head, _data, _binarytree->cmp_data_function, _binarytree->cmp_data_ctx);
}

binarysearchtree_node* binarysearchtree_delete(binarysearchtree* _binarytree, void* _data) {
    binarysearchtree_node* data_node;
    _binarytree->head = binarysearchtree_node_delete(_binarytree->head, _data, &data_node, _binarytree->cmp_data_function, _binarytree->cmp_data_ctx);
    return data_node;
}

void binarysearchtree_node_init(binarysearchtree_node* root, void* _data) {
    root->data = _data;
    root->left = NULL;
    root->right = NULL;
}

void binarysearchtree_init(binarysearchtree* _binarytree, binarytree_cmp_data_function cmp_data_function, void* cmp_data_ctx) {
    _binarytree->head = NULL;
    _binarytree->cmp_data_function = cmp_data_function;
    _binarytree->cmp_data_ctx = cmp_data_ctx;
}

// https://www.geeksforgeeks.org/insertion-in-binary-search-tree/
int binarysearchtree_node_insert(binarysearchtree_node* root, binarysearchtree_node* new_node, binarytree_cmp_data_function cmp_function, void* ctx) {
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
        
		if (cmp_function(temp->data, new_node->data, ctx) > 0) {
			prev = temp;
			temp = temp->left;
		}
		else if (cmp_function(temp->data, new_node->data, ctx) < 0) {
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

binarysearchtree_node* binarysearchtree_node_find(binarysearchtree_node* root, void* _data, binarytree_cmp_data_function cmp_function, void* ctx) {
	binarysearchtree_node* temp;

	if (root == NULL) {
		return NULL;
	}
	temp = root;
    
	while (temp) {
		if (cmp_function(temp->data, _data, ctx) > 0) {
			temp = temp->left;
		}
		else if (cmp_function(temp->data, _data, ctx) < 0) {
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
binarysearchtree_node* binarysearchtree_node_delete(binarysearchtree_node* root, void* _data, binarysearchtree_node** data_node, binarytree_cmp_data_function cmp_function, void* ctx)
{
    binarysearchtree_node* curr = root;
    binarysearchtree_node* prev = NULL;

    // Check if the key is actually
    // present in the BST.
    // the variable prev points to
    // the parent of the key to be deleted.

    while (curr != NULL && cmp_function(curr->data, _data, ctx) != 0) {
        prev = curr;
        if (cmp_function(_data, curr->data, ctx) < 0)
            curr = curr->left;
        else
            curr = curr->right;
    }

    if (curr == NULL) {
        *data_node = NULL;
        return root;
    }

    // Check if the node to be
    // deleted has atmost one child.
    if (curr->left == NULL || curr->right == NULL) {

        // newCurr will replace
        // the node to be deleted.
        binarysearchtree_node* newCurr;

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
        //free(curr);
        *data_node = curr;
    }

    // node to be deleted has
    // two children.
    else {
        binarysearchtree_node* p = NULL;
        binarysearchtree_node* temp;

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
        if (p != NULL)
            p->left = temp->right;

        // if the inorder successor was the
        // curr (i.e. curr = the node which has the
        // same data as the given data by the
        // user to be deleted), then make the
        // right child of the node to be
        // deleted equal to the right child of
        // the inorder successor.
        else
            curr->right = temp->right;

        curr->data = temp->data;
        //free(temp);
        *data_node = temp;
    }
    return root;
}

binarysearchtree_node* binarysearchtree_node_delete2(binarysearchtree_node* root, void* _data, binarysearchtree_node** data_node, binarytree_cmp_data_function cmp_function, void* ctx) {
    binarysearchtree_node* nodeToDelete, *parentNodeToDelete;
    binarysearchtree_node* replacementNode, *parentReplacementNode;
    binarysearchtree_node* temp, *parent;

    if (root == NULL) {
        return NULL;
    }

    replacementNode = NULL;
    nodeToDelete = NULL;
    parent = NULL;
    temp = root;

    while (temp) {
        if (cmp_function(temp->data, _data, ctx) > 0) {
            parent = temp;
            temp = temp->left;
        }
        else if (cmp_function(temp->data, _data, ctx) < 0) {
            parent = temp;
            temp = temp->right;
        }
        else {
            nodeToDelete = temp;
            break;
        }
    }

    // node not found
    if (nodeToDelete == NULL) {
        return NULL;
    }
    parentNodeToDelete = parent;

    // node without any children
    if (nodeToDelete->left == NULL && nodeToDelete->right == NULL) {
        if (parentNodeToDelete != NULL)
        {
            if (parentNodeToDelete->left == nodeToDelete) {
                parentNodeToDelete->left = NULL;
            }
            else {
                parentNodeToDelete->right = NULL;
            }
        }
    }

    // node with only 1 children
    else if (nodeToDelete->left == NULL && nodeToDelete->right != NULL) {
        if (parentNodeToDelete != NULL) {
            if (parentNodeToDelete->left == nodeToDelete) {
                parentNodeToDelete->left = nodeToDelete->right;
            }
            else {
                parentNodeToDelete->right = nodeToDelete->right;
            }
        }
    }
    else if (nodeToDelete->left != NULL && nodeToDelete->right == NULL) {
        if (parentNodeToDelete != NULL) {
            if (parentNodeToDelete->left == nodeToDelete) {
                parentNodeToDelete->left = nodeToDelete->left;
            }
            else {
                parentNodeToDelete->right = nodeToDelete->left;
            }
        }
    }
    else {
        // node with 2 children
        parentReplacementNode = nodeToDelete;
        replacementNode = nodeToDelete->right;
        if (replacementNode != NULL)
        {
            while (replacementNode->left) {
                parentReplacementNode = replacementNode;
                replacementNode = replacementNode->left;
            }
        }


        if (nodeToDelete == parentReplacementNode) {
            replacementNode->left = nodeToDelete->left;
        }
        else {
            if (replacementNode->right != NULL) {
                parentReplacementNode->left = replacementNode->right;
            }
            else {
                parentReplacementNode->left = NULL;
            }
            replacementNode->left = nodeToDelete->left;
            replacementNode->right = nodeToDelete->right;
        }

        if (parentNodeToDelete != NULL) {
            if (parentNodeToDelete->left == nodeToDelete) {
                parentNodeToDelete->left = replacementNode;
            }
            else {
                parentNodeToDelete->right = replacementNode;
            }
        }
    }
    *data_node = nodeToDelete;
    if (nodeToDelete == root)
    {
        return replacementNode;
    }
    else {
        return root;
    }
}

binarysearchtree_node* binarysearchtree_node_max(binarysearchtree_node* root) {
    binarysearchtree_node* cur;

    if (root == NULL) {
        return NULL;
    }

    cur = root;
    while (cur->right)
    {
        cur = cur->right;
    }
    return cur;
}

binarysearchtree_node* binarysearchtree_max(binarysearchtree* _binarytree) {
    return binarysearchtree_node_max(_binarytree->head);
}

binarysearchtree_node* binarysearchtree_node_min(binarysearchtree_node* root) {
    binarysearchtree_node* cur;

    if (root == NULL) {
        return NULL;
    }

    cur = root;
    while (cur->left)
    {
        cur = cur->left;
    }
    return cur;
}

binarysearchtree_node* binarysearchtree_min(binarysearchtree* _binarytree) {
    return binarysearchtree_node_min(_binarytree->head);
}

int binarysearchtree_iterator(binarysearchtree* _binarytree, int (*routine)(binarysearchtree_node* node, void* _ctx), void* ctx) {
    binarysearchtree_node_iterator(_binarytree->head, routine, ctx);
}

// iterate the three while the function routine returns 0
int binarysearchtree_node_iterator(binarysearchtree_node* root, int (*routine)(binarysearchtree_node* node, void* _ctx), void* ctx) {
    if (root == NULL) {
        return 0;
    }

    // First recur on left child
    if (binarysearchtree_node_iterator(root->left, routine, ctx) != 0) {
        return 1;
    }

    // Then print the data of node
    if (routine(root, ctx) != 0) {
        return 1;
    }
    //printf("%d ", root->data);

    // Now recur on right child
    return binarysearchtree_node_iterator(root->right, routine, ctx);
}
