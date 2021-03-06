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

    BST<Node<int> > binary_tree{{1,2,3,4,5,6,7,45,125,2346,12,123,235,67,142}};

    /* Traverse */

    //binary_tree.traverse(BSTTraverseOrder::mid, [](Node<int>& node) {cout << node.value << "(left: "<< node.left_count << ", right: " << node.right_count << ") ";});
    //cout << endl;

    /**
     * Search the element by value
     */
    auto node = binary_tree.search(5);
    if (node)
        cout << node->value << endl;

    /* draw */
    binary_tree.draw("foo.dot");
}


