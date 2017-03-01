/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 12:33:58 PM CST
 File Name: CBST.cpp
 Description: 
 ************************************************************************/

#include <algorithm>
#include <climits>
#include "CBST.h"

using namespace std;


/**
 * Return a node if found, otherwise return NULL
 */
Node *CBST::search(int key)
{
    return search(root, key);
}

Node *CBST::search(Node *node, int key)
{
    if (node == NULL)
        return NULL;
    if (node->key == key)
        return node;
    else if (key > node->key)
        return search(node->right, key);
    else
        return search(node->left, key);
}

/**
 * If a node with same key already exists, the new node will be inserted as left child
 */
void CBST::insert(int key, int value)
{
    insert(root, key, value, NULL);
}

Node *CBST::insert(Node *node, int key, int value, Node *parent)
{
    if (node == NULL)
    {
        Node *new_node = new Node(key, value);
        if (new_node != NULL)
        {
            new_node->parent = parent;
        }
        return new_node;
    }
    if (key <= node->key)
        node->left = insert(node->left, key, value, node);
    else
        node->right = insert(node->right, key, value, node);
}

/**
 * Traverse BST in-order (the result is always a sorted list)
 */

void CBST::traverse()
{
    traverse(root, Node::echo);
}

template<class T>
void CBST::traverse(T *node, void (*callback)(T))
{
    if (node == NULL)
    {
        return;
    }
    traverse(node->left, callback);
    callback(*node);
    traverse(node->right, callback);
}

/**
 * Verify if this is an BST.
 *
 * The basic principle is that:
 *
 * for left child, it should be less than its parent, but large than its nearest ancestor who is a right child of some node if any
 * for right child, it should be bigger than its parent, but less than its nearest ancestor who is a left child of some node if any
 */
bool CBST::isBST()
{
    return isBST(INT_MIN, INT_MAX, root);
}

bool CBST::isBST(int min, int max, Node *node)
{
    if (node == NULL) return true;
    if ((node->key > max) || (node->key < min)) return false;

    return (isBST(node->key, max, node->right) && isBST(min, node->key, node->left));
}



