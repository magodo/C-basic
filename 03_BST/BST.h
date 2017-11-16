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
#include <exception>
#include <functional>
#include <string>

using namespace std;

/**
 * Declaration of Node
 *
 * note: the type T must implement operator<=
 */

template<typename T>
class Node
{
    public:

        Node(T value):
            value{value},
            left{nullptr},
            right{nullptr},
            left_count{},
            right_count{}
        {}

        static bool le(Node n, Node m) { return (n.value <= m.value);}

        T value;
        Node *left;
        Node *right;
        int left_count;
        int right_count;
};

/**
 * Declaration of BST
 */

enum class BSTTraverseOrder
{
    pre,
    mid,
    post
};

template <class T>
class BST
{
    public:

        /**
         * C'tor
         */
        BST(vector<T> container);

        /* D'tor */
        ~BST() noexcept;

        /**
         * Traverse BST
         */
        void traverse(BSTTraverseOrder order, function<void(T&)> f);

        /**
         * Search an element by key(return the first match)
         */
        T *search(int key);

        /* Insert */
//        void insert()
        void draw(string filename);


    protected:

        /**
         * Recersively construct balanced BST
         */
        virtual T *do_initBalanceBST(vector<T> container);

        /* Recersively destroy node from the node specified. */

        void do_destroyBalanceBST(T *node) noexcept;

        /**
         * Recersively traverse BST
         */
        void do_traverse(T *node, BSTTraverseOrder order);

        /**
         * Recursively search
         */
        T *do_search(int key, T*);

    private:

        T *root;

        /* Traverse functor. */
        function<void(T&)> func;
};

#include "BST.tpp" // check [this](http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file)

#endif // BST_H
