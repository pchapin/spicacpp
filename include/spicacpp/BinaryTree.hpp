/*! \file    BinaryTree.hpp
 *  \brief   Binary tree template.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * TODO: Consider the following items...
 *
 * + Implement a test program! This is a priority, especially for a template!
 * + Implement operator--() for iterators.
 * + Implement reverse iterators.
 * + Implement the copy constructor and operator=()
 */

#ifndef SPICA_BINARYTREE_H
#define SPICA_BINARYTREE_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace spica {

    //! Binary tree class template.
    /*!
     * This template provides a binary search tree with no extra code for balancing. Thus if the
     * tree becomes unbalanced it will not perform very well. On the other hand it has lower
     * overhead than, for example, a red-black tree.
     *
     * This template provides all the usual operations (or at least, that is the intent).
     */
    template <typename T, typename StrictWeakOrdering = std::less<T>> class BinaryTree {

      public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

      private:
        struct TreeNode {
            T data;
            TreeNode* parent;
            TreeNode* left;
            TreeNode* right;

            TreeNode(const T& d, TreeNode* p, TreeNode* l, TreeNode* r) :
                data(d), parent(p), left(l), right(r)
            {
            }
        }; // End of nested TreeNode structure.

        // Private members of a tree.
        TreeNode* root;          // Points at root of tree or NULL if tree empty.
        size_type count;         // Number of nodes in the tree.
        StrictWeakOrdering comp; // Comparison object.

        // Private methods.
        void kill_subtree(TreeNode*);
        const TreeNode* minimum_node(TreeNode*) const; // Given node non-null.
        const TreeNode* maximum_node(TreeNode*) const; // Given node non-null.

      public:
        //! Tree iterators class.
        /*!
         * Tree iterators are bidirectional iterators and support all the usual bidirectional
         * operations (at least that is the intent).
         */
        class iterator {

            friend class BinaryTree;

          public:
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef T value_type;
            typedef T* pointer;
            typedef T& reference;
            typedef std::ptrdiff_t difference_type;

            //! Returns a reference to the current data item.
            const_reference operator*() const
            {
                return my_node->data;
            }

            //! Returns a pointer to the current data item.
            const_pointer operator->() const
            {
                return &my_node->data;
            }

            //! Prefix increment moves the iterator to the next item.
            iterator& operator++();

            //! Prefix decrement moves the iterator to the previous item.
            iterator& operator--();

            //! Two iterators are equal if they point to the same object.
            bool operator==(const iterator& other) const
            {
                if (my_tree == other.my_tree && my_node == other.my_node)
                    return true;
                return false;
            }

            //! Two iterators are unequal if they point to different objects.
            bool operator!=(const iterator& other) const
            {
                return !(*this == other);
            }

            //! Default constructor.
            iterator() : my_tree(nullptr), my_node(nullptr)
            {
            }

          private:
            iterator(const BinaryTree* t, const typename BinaryTree::TreeNode* n) :
                my_tree(t), my_node(n)
            {
            }

            const BinaryTree* my_tree;
            const typename BinaryTree::TreeNode* my_node;
        }; // End of nested iterator class.

        friend class iterator;

        //! Default constructors.
        BinaryTree(StrictWeakOrdering c = StrictWeakOrdering()) : root(nullptr), count(0), comp(c)
        {
        }

        // For now don't bother with copying. Save that for version 2.0!
        BinaryTree(const BinaryTree&) = delete;
        BinaryTree& operator=(const BinaryTree&) = delete;

        //! Destructor.
        ~BinaryTree();

        //! Returns an iterator that points at the first item in the tree.
        iterator begin() const;

        //! Returns an iterator that points just past the last item in the tree.
        iterator end() const;

        //! Returns the number of items in the tree.
        size_type size() const
        {
            return count;
        }

        //! Inserts a new item in the tree.
        std::pair<iterator, bool> insert(const T&);

        //! Locates an item in the tree.
        iterator find(const T&) const;

        //! Erases an item at the specified location.
        void erase(iterator);

        //! Erases the entire tree.
        void clear();

    }; // End of class template tree.

    // =====
    // Methods of BinaryTree<T, StrictWeakOrdering>::iterator
    // =====

    template <typename T, typename StrictWeakOrdering>
    typename BinaryTree<T, StrictWeakOrdering>::iterator&
    BinaryTree<T, StrictWeakOrdering>::iterator::operator++()
    {
        // Is incrementing an off-the-end iterator undefined?
        if (my_node == nullptr)
            return *this;

        if (my_node->right != nullptr) {
            my_node = my_tree->minimum_node(my_node->right);
        }
        else {
            typename BinaryTree::TreeNode* candidate = my_node->parent;
            while (candidate != nullptr && my_node == candidate->right) {
                my_node = candidate;
                candidate = my_node->parent;
            }
            my_node = candidate;
        }
        return *this;
    }

    template <typename T, typename StrictWeakOrdering>
    typename BinaryTree<T, StrictWeakOrdering>::iterator&
    BinaryTree<T, StrictWeakOrdering>::iterator::operator--()
    {
        // UNFINISHED!
        return *this;
    }

    // =====
    // Methods of BinaryTree
    // =====

    template <typename T, typename StrictWeakOrdering>
    void BinaryTree<T, StrictWeakOrdering>::kill_subtree(TreeNode* r)
    {
        if (r == nullptr)
            return;
        kill_subtree(r->left);
        kill_subtree(r->right);
        delete r;
    }

    template <typename T, typename StrictWeakOrdering>
    const typename BinaryTree<T, StrictWeakOrdering>::TreeNode*
    BinaryTree<T, StrictWeakOrdering>::minimum_node(TreeNode* r) const
    {
        while (r->left != nullptr) {
            r = r->left;
        }
        return r;
    }

    template <typename T, typename StrictWeakOrdering>
    const typename BinaryTree<T, StrictWeakOrdering>::TreeNode*
    BinaryTree<T, StrictWeakOrdering>::maximum_node(TreeNode* r) const
    {
        while (r->right != nullptr) {
            r = r->right;
        }
        return r;
    }

    template <typename T, typename StrictWeakOrdering>
    BinaryTree<T, StrictWeakOrdering>::~BinaryTree()
    {
        kill_subtree(root);
    }

    template <typename T, typename StrictWeakOrdering>
    typename BinaryTree<T, StrictWeakOrdering>::iterator
    BinaryTree<T, StrictWeakOrdering>::begin() const
    {
        if (root == nullptr)
            return iterator(this, nullptr);
        return iterator(this, minimum_node(root));
    }

    template <typename T, typename StrictWeakOrdering>
    typename BinaryTree<T, StrictWeakOrdering>::iterator
    BinaryTree<T, StrictWeakOrdering>::end() const
    {
        return iterator(this, nullptr);
    }

    template <typename T, typename StrictWeakOrdering>
    std::pair<typename BinaryTree<T, StrictWeakOrdering>::iterator, bool>
    BinaryTree<T, StrictWeakOrdering>::insert(const T& item)
    {
        TreeNode* bookmark = nullptr;
        TreeNode* current = root;
        TreeNode* new_node = new TreeNode(item, nullptr, nullptr, nullptr);

        while (current != nullptr) {
            bookmark = current;
            if (comp(new_node->data, current->data)) {
                current = current->left;
            }
            else if (comp(current->data, new_node->data)) {
                current = current->right;
            }
            else {
                // Current is a copy of the new item.
                delete new_node;
                return std::pair<iterator, bool>(iterator(this, current), false);
            }
        }

        new_node->parent = bookmark;
        if (bookmark == nullptr) {
            root = new_node;
        }
        else if (comp(new_node->data, bookmark->data)) {
            bookmark->left = new_node;
        }
        else {
            bookmark->right = new_node;
        }

        ++count;
        return std::pair<iterator, bool>(iterator(this, new_node), true);
    }

    template <typename T, typename StrictWeakOrdering>
    typename BinaryTree<T, StrictWeakOrdering>::iterator
    BinaryTree<T, StrictWeakOrdering>::find(const T& item) const
    {
        TreeNode* p = root;
        while (p != 0 && (comp(item, p->data) || comp(p->data, item))) {
            if (comp(item, p->data)) {
                p = p->left;
            }
            else {
                p = p->right;
            }
        }
        return iterator(this, p);
    }

    template <typename T, typename StrictWeakOrdering>
    void BinaryTree<T, StrictWeakOrdering>::erase(iterator it)
    {
        // This version of erase assumes 'it' points at a valid node.
        const TreeNode* splice;
        TreeNode* rest;

        if (it.my_node->left == nullptr || it.my_node->right == nullptr) {
            splice = it.my_node;
        }
        else {
            splice = minimum_node(it.my_node->right);
        }

        if (splice->left != nullptr) {
            rest = splice->left;
        }
        else {
            rest = splice->right;
        }

        if (rest != 0) {
            rest->parent = splice->parent;
        }
        if (splice->parent == nullptr) {
            root = rest;
        }
        else if (splice == splice->parent->left) {
            splice->parent->left = rest;
        }
        else {
            splice->parent->right = rest;
        }

        if (splice != it.my_node) {
            const_cast<TreeNode*>(it.my_node)->data = splice->data;
        }

        delete splice;
        --count;

        // The iterator `it` is now invalid. Is that okay?
    }

    template <typename T, typename StrictWeakOrdering>
    void BinaryTree<T, StrictWeakOrdering>::clear()
    {
        kill_subtree(root);
        root = nullptr;
        count = 0;
    }

} // End of namespace spica.

#endif
