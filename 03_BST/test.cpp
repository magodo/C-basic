/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:49:25 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <vector>
using namespace std;

#include "BST.h"

int main()
{
    /**
     * Construct a balanced BST
     */

    BST<Node<int> > binary_tree{{1,2,3,4,5,6,7}};

    /* Traverse */
    binary_tree.traverse([](Node<int>& node) {cout << node.key << " ";}, BSTTraverseOrder::mid);
    cout << endl;

    /**
     * Search the element by key
     */
    auto node = binary_tree.search(5);
    if (node)
        cout << node->key << endl;
}


