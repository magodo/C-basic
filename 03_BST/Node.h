/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:46:07 PM CST
 Description: 
 ************************************************************************/

#include <iostream>

using namespace std;

struct Node
{
    Node(int key, int value):
        key(key),
        value(value),
        left(NULL),
        right(NULL)
    {}

    int value;
    int key;
    Node *left;
    Node *right;
    
    inline static bool le(Node n, Node m) { return (n.key <= m.key);}
    inline void inspect(){ cout << "(" << key << ", " << value << ") ";}
};
