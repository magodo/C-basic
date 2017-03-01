/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 02:58:22 PM CST
 File Name: main.cpp
 Description: 
 ************************************************************************/

#include <iostream>
#include <vector>
#include "CBST.h"

using namespace std;

int main()
{
    vector<Node> v;
    CBST binary_tree, non_bst;
    Node *node;

    for (unsigned i = 0; i < 10; ++i)
    {
        v.push_back(Node(i, i));
    }

    binary_tree.arrayToBalancedBST(v, Node::comp);

    //node = binary_tree.search(3);
    //cout << node->key << " " << node->value << endl;

    binary_tree.traverse();

}
