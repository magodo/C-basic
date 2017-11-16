/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:05:58 PM CST
 Description: 
 ************************************************************************/

/* C'tor */

#include <fstream>

template<class T>
BST<T>::BST(vector<T> container):
    func{nullptr}
{
    sort(container.begin(), container.end(), T::le);

    root = do_initBalanceBST(container);
}

template<class T>
T *BST<T>::do_initBalanceBST(vector<T> container)
{
    auto beg = container.begin();
    auto end = container.end();
    auto mid = beg + (end-beg)/2;

    T *node = new T(mid->value);
    node->left_count = (mid-beg);
    node->right_count = (end-mid-1);

    /* Continue only when there is space in heap, otherwise the calling node be leaf */
    node->left = (beg < mid)? do_initBalanceBST(vector<T>(beg, mid)) : nullptr;
    node->right = (mid+1 < end)? do_initBalanceBST(vector<T>(mid+1, end)) : nullptr;

    return node;
}

/* D'tor */

template<class T>
BST<T>::~BST() noexcept
{
    do_destroyBalanceBST(root);
}

template<class T>
void BST<T>::do_destroyBalanceBST(T *node) noexcept
{
    if (node == nullptr)
        return;

    do_destroyBalanceBST(node->left);
    do_destroyBalanceBST(node->right);
    delete node;
}

/* Traverse BST */

template<class T>
void BST<T>::traverse(BSTTraverseOrder order, function<void(T&)> f)
{
    func = f;
    do_traverse(root, order);
}

template<class T>
void BST<T>::do_traverse(T *node, BSTTraverseOrder order)
{
    if (node == NULL)
        return;
    switch (order)
    {
        case BSTTraverseOrder::pre:
            func(*node);
            do_traverse(node->left, BSTTraverseOrder::pre);
            do_traverse(node->right, BSTTraverseOrder::pre);
            break;

        case BSTTraverseOrder::mid:
            do_traverse(node->left, BSTTraverseOrder::mid);
            func(*node);
            do_traverse(node->right, BSTTraverseOrder::mid);
            break;

        case BSTTraverseOrder::post:
            do_traverse(node->left, BSTTraverseOrder::post);
            do_traverse(node->right, BSTTraverseOrder::post);
            func(*node);
            break;
    }
}

/* Search element by value */

template<class T>
T *BST<T>::search(int value)
{
    return do_search(value, root);
}

template<class T>
T *BST<T>::do_search(int value, T *node)
{
    if ((node == nullptr) || (node->value == value))
        return node;
    
    return (value > node->value)? do_search(value, node->right) : do_search(value, node->left);
}

/* draw into a dot file */
template<class T>
void BST<T>::draw(string filename)
{
    ofstream os;
    os.open(filename, ios::out);

    if (os.is_open())
    {
        os << "digraph G\n{\n";
        traverse(BSTTraverseOrder::mid, [&](T& node){ \
                if (node.left)
                    os << "\t\"" << node.value << "\" -> \"" << node.left->value << "\"\n"; \
                if (node.right)
                    os << "\t\"" << node.value << "\" -> \"" << node.right->value << "\"\n"; \
                });
        os << "}";
    }
}

