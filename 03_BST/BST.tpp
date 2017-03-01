/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 01 Mar 2017 08:05:58 PM CST
 Description: 
 ************************************************************************/

/**
 * Init balance BST from an unsorted vector
 */

template<class T>
void BST<T>::initBalanceBST(vector<T> container)
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

    T *new_node = new T(mid->key, mid->value);

    /* Continue only when there is space in heap, otherwise the calling node be leaf */
    if (new_node != NULL)
    {
        new_node->left = (beg < mid)? do_initBalanceBST(vector<T>(beg, mid)) : NULL;
        new_node->right = (mid+1 < end)? do_initBalanceBST(vector<T>(mid+1, end)) : NULL;
    }

    return new_node;
}

/**
 * Traverse BST
 *
 * Note: if this is a BST, the result is always sorted
 */

template<class T>
void BST<T>::traverse()
{
    do_traverse(root);
}

template<class T>
void BST<T>::do_traverse(T *node)
{
    if (node == NULL)
        return;
    do_traverse(node->left);
    node->inspect();
    do_traverse(node->right);
}

/**
 * Search element by key
 */

template<class T>
T *BST<T>::search(int key)
{
    return do_search(key, root);
}

template<class T>
T *BST<T>::do_search(int key, T *node)
{
    if ((node == NULL) || (node->key == key))
        return node;
    
    return (key > node->key)? do_search(key, node->right) : do_search(key, node->left);
}

