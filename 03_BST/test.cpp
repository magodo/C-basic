/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:49:25 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <vector>
using namespace std;

#include "BST.h"
#include "Node.h"

int main()
{
    BST<Node> binary_tree;

    /**
     * Construct a non-BST tree and traverse it 
     */
    binary_tree.root = new Node(2,2);
    binary_tree.root->left = new Node(1,1);
    binary_tree.root->left->right = new Node(3,3);
    binary_tree.traverse();
    cout << endl;

    /**
     * Construct a balanced BST
     */

    vector<int> v{1,5,2,23,5,7,324,4,6,324,35,7,32};
    vector<Node> vn;
    for (auto i: v)
        vn.push_back(Node(i,i));

    binary_tree.initBalanceBST(vn);
    binary_tree.traverse();
    cout << endl;

    /**
     * Search the element by key
     */
    cout << binary_tree.search(23)->value << endl;
}


