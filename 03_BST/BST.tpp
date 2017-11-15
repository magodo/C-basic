/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:05:58 PM CST
 Description: 
 ************************************************************************/

/* C'tor */

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
    node->left_cout = (mid-beg);
    node->right_cout = (end-mid-1);

    /* Continue only when there is space in heap, otherwise the calling node be leaf */
    new_node->left = (beg < mid)? do_initBalanceBST(vector<T>(beg, mid)) : nullptr;
    new_node->right = (mid+1 < end)? do_initBalanceBST(vector<T>(mid+1, end)) : nullptr;

    return new_node;
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
void BST<T>::traverse(function<void(T&)> f, BSTTraverseOrder order)
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

/* Search element by key */

template<class T>
T *BST<T>::search(int key)
{
    return do_search(key, root);
}

template<class T>
T *BST<T>::do_search(int key, T *node)
{
    if ((node == nullptr) || (node->key == key))
        return node;
    
    return (key > node->key)? do_search(key, node->right) : do_search(key, node->left);
}

