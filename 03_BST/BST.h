/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:06:02 PM CST
 Description: 
 
    This BST class is a container, users need to implement the element, which
    at least contain following member variables/functions:
    
    struct T
    {
        T(key,value):
          key(key),
          value(value),
          left(NULL),
          right(NULL)
        {}

        int key;  // used for comparing 
        CopyableType value; // used for storing record
        T *left;  // pointer to left sub-tree
        T *right; // pointer to right sub-tree
        
        static bool le(T e1, T e2); // return true if e1 <= e2 under some rule
        void inspect(T);            // show the content of each element, for traverse
    };

 ************************************************************************/

#ifndef BST_H
#define BST_H

#include <vector>
#include <algorithm>

using namespace std;

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
