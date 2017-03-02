/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:06:02 PM CST
 Description: 
 
 ************************************************************************/

#ifndef BST_H
#define BST_H

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;


/**
 * Declaration of Node
 */

template<typename T>
struct Node
{
    Node(int key):
        Node(key, T())
    {}

    Node(int key, T value):
        key(key),
        value(value),
        left(NULL),
        right(NULL)
    {}

    T value;
    int key;
    Node *left;
    Node *right;
    
    inline static bool le(Node n, Node m) { return (n.key <= m.key);}
    inline void inspect(){ cout << "(" << key << ", " << value << ") ";}
};

/**
 * Declaration of BST
 */

template <class T>
class BST
{

#ifdef DEBUG
    public:
        T *root;
#else
    private:
        T *root;
#endif

    public:

        /**
         * Construct a balanced BST from vector
         */
        void initBalanceBST(vector<T> container);

        /**
         * Traverse BST
         */
        void traverse();

        /**
         * Search an element by key(return the first match)
         */
        T *search(int key);


    private:

        /**
         * Recersively construct balanced BST
         */
        T *do_initBalanceBST(vector<T> container);

        /**
         * Recersively traverse BST
         */
        void do_traverse(T *node);

        /**
         * Recursively search
         */
        T *do_search(int key, T*);
};

#include "BST.tpp" // check [this](http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file)

#endif // BST_H
