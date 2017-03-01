/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 12:25:04 PM CST
 File Name: CBST.h
 Description: 
 ************************************************************************/

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct Node
{
    Node() = default;
    Node(int key, int value):
        key(key),
        value(value),
        left(NULL),
        right(NULL),
        parent(NULL)
    {}

    int key;
    int value;
    Node *left;
    Node *right;
    Node *parent;

    static bool comp(Node i, Node j) {return (i.key < j.key);}
    static void echo(Node i) { cout << i.value << endl;}
};

/**
 * This BST allow duplicate numbers, the equaled value will be inserted as left child.
 *
 */
class CBST
{
    public:
        
        CBST() = default;
        ~CBST() = default;

        /** 
         *  Sort array and save as balanced BST started from "root"
         *  (Define in header explicitly, refer to: http://stackoverflow.com/questions/648900/c-templates-undefined-reference)
         */
        template<class T>
        void arrayToBalancedBST(vector<T> container, bool(*comp)(T, T))
        {
            typename vector<T>::iterator begin, end;
            // sort input array
            sort(container.begin(), container.end(), comp);
            root = sortedArrayToBalancedBST(container, NULL);
        }

        // search
        Node *search(int key);

        // insertion
        void insert(int key, int value);

        // traverse
        void traverse();

        // verification
        bool isBST();
#if 0
        // deletion
        void del(int key);

#endif

    private:

        /** 
         *  Sorted array to balanced BST
         *  (Define in header explicitly, refer to: http://stackoverflow.com/questions/648900/c-templates-undefined-reference)
         */
        template<class T>
        Node *sortedArrayToBalancedBST(vector<T> container, Node *parent)
        {
            typename vector<T>::iterator begin, end;

            begin = container.begin();
            end = container.end();
            auto mid = begin + (end-begin)/2;
            Node *node = new Node(mid->key, mid->value);

            if (node != NULL)
            {
                node->parent = parent;
                node->left = (mid>begin)? sortedArrayToBalancedBST(vector<T>(begin, mid), node):NULL;
                node->right = (end>mid+1)? sortedArrayToBalancedBST(vector<T>(mid+1, end), node):NULL;
            }

            return node;
        }

        // search
        Node* search(Node *node, int key);
        
        // insertion
        Node *insert(Node *node, int key, int value, Node *parent);
        
        // traverse
        template<class T>
        void traverse(T *node, void (*callback)(T));
        
        // verification
        bool isBST(int min, int max, Node *node);


        Node *root;

};
