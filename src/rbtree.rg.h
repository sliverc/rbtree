// TODO: User docu
//
// TODO: Review
//
// TODO: Comments in with ., lists don't
//
// TODO: Assert message don't have periods
//
// ==============
// Red-Black Tree
// ==============
//
// * Textbook implementation
// * Extensive tests
// * Has parent pointers and therefore faster delete_node and constant time
//   replace_node
// * Composable
// * Readable
// * Generic
// * Easy to use, a bit complex to extend because it is generic [1]_
// * Code size could be optimized [2]_
//
// .. [1] My rgc preprocessor and its MACRO_DEBUG mode are very helpful.
//
// .. [2] It quite easy to bind intermediate functions. But it only uses
//        about 1800 bytes (-Os), per type.
//
// Jean-Louis Fuchs <ganwell@fangorn.ch> based on Introduction To Algorithms
//
// WORK IN PROGRESS
// ================
//
// Installation
// ============
//
// Copy rbtree.h into your source.
//
// Development
// ===========
//
// Developed on github_
//
// .. _github: https://github.com/ganwell/rbtree
//
// Requirements
// ------------
//
// * gcc or clang
// * hypothesis
// * cffi
// * rst2html
//
// Build and test
// --------------
//
// .. code-block:: bash
//
//    make
//
// Debug macros
// ------------
//
// .. code-block:: bash
//
//    make clean all MACRO_DEBUG=True
//
// This will expand the macros into .c files, so gdb can step into them.
//
// Usage
// =====
//
// Standard
// --------
//
//
// Extended
// --------
//
// Every function x comes in two flavors
//
// cx_x
//    These functions are bound to a type. Traits and the comparator are mapped
//    to the context. You have to define the type and the traits for the
//    context and then you bind the function.
//
//    .. code-block:: cpp
//
//       #define my_color_m(x) (x)->color
//       #define my_parent_m(x) (x)->parent
//       #define my_left_m(x) (x)->left
//       #define my_right_m(x) (x)->right
//       #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
//       rb_bind_cx_m(my, node_t)
//
//    .. code-block:: cpp
//
//       my_tree_init(&tree);
//       my_node_init(node);
//
//    There is also a short if you know your are going to use all standard
//    fields in your struct (color, parent, left right)
//
//    .. code-block:: cpp
//
//       #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
//       rb_bind_m(my, node_t)
//
//    .. code-block:: cpp
//
//       my_tree_init(&tree);
//       my_node_init(node);
//
//    Of course usually you want to split declaration and implementation of the
//    function so it is. example.h:
//
//    .. code-block:: cpp
//
//       #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
//       rb_bind_decl_m(my, node_t)
//
//    And example.c:
//
//    .. code-block:: cpp
//
//       #include "example.h"
//       rb_bind_impl_m(my, node_t)
//
//       int main(void) { my_node_init(node); return 0; }
//
// rb_x_m
//    These functions are macros and take a type and traits as standard
//    arguments and are the most verbose. Used to extend rbtree.
//
//    To use the rb_x_m functions you also need to initialize the nil pointer.
//
//    .. code-block:: cpp
//
//       tree = my_nil_ptr;
//       rb_node_init_m(
//           my_nil_ptr,
//           rb_color_m,
//           rb_parent_m,
//           rb_left_m,
//           rb_right_m,
//           my_nil_ptr
//       ); // Instead of my_tree_init in the bound functions
//
// Questions
// =========
//
// Why don't you just generate typed functions from the beginning?
//    I want to be able to reuse and compose my code. Especially for
//    composability I need access to the generic functions.
//
// Why is the iterator so complicated?
//    rbtree is part of a larger set of data-structures, some need more
//    complicated iterator setups, to make the data-structures interchangeable,
//    all have to follow the iterator protocol. use rb_for_cx_m.
//
// Lessons learned
// ===============
//
// I thought I don't have to understand the red-black trees and can just adjust
// an existing implementation. I chose poorly and the thing was inherently
// broken. I wasted a lot of time on it. They replaced the nil pointer with
// NULL and it resulted in a tree that works, but is not balanced. So my
// check_tree function failed and I tried to fix that implementation. It turns
// out buttom-up-fixups are very difficult to implement with NULL pointers. So
// after many wasted hours I just read Introductions to Algorithms and fixed my
// implementation.
//
// Performance
// ===========
//
// I reference sglib, because it is the best and greatest I know. Kodos to
// Marian Vittek.
//
// .. image:: https://github.com/ganwell/rbtree/raw/master/perf_insert.png
//    :width: 90%
//    :align: center
//    :alt: insert
//
// .. image:: https://github.com/ganwell/rbtree/raw/master/perf_delete.png
//    :width: 90%
//    :align: center
//    :alt: delete
//
// sglib has no delete_node. For many application a delete_node and a
// replace_node function is handy, since the application already has the right
// node to delete or replace.
//
// .. image:: https://github.com/ganwell/rbtree/raw/master/perf_replace.png
//    :width: 90%
//    :align: center
//    :alt: replace
//
// Because we have parent pointer we can implement replace_node in constant
// time O(1). With sglib we have to add/remove for a replacement.
//
// Implementation
// ==============
//
// Based on Introduction to Algorithms: official_, wiki_, web_, pdf_ and
// archive_.
//
// .. _official: https://mitpress.mit.edu/books/introduction-algorithms
// .. _wiki: https://en.wikipedia.org/wiki/Introduction_to_Algorithms
// .. _web: http://staff.ustc.edu.cn/~csli/graduate/algorithms/book6/chap14.htm
// .. _pdf: http://www.realtechsupport.org/UB/SR/algorithms/Cormen_Algorithms_3rd.pdf
// .. _archive: https://archive.org/details/IntroductionToAlgorithms3edCorman_201508
//
// Properties
// ----------
//
// A binary search tree is a red-black tree if it satisfies the following
// red-black properties:
//
// 1. Every node is either red or black.
//
// 2. Every leaf (NIL) is black.
//
// 3. If a node is red, then both its children are black.
//
// 4. Every simple path from a node to a descendant leaf contains the same
//    number of black nodes.
//
// In order to understand the deletion, the concept of double (extra) blackness
// is introduced. If a black node was deleted its blackness is pushed down and a
// child can become extra black. This is the way property 1 can be violated.
//
// Assertion
// =========
//
// .. code-block:: cpp
//
#include <assert.h>

//
// Basic traits
// ============
//
// Traits used by default (rb_x_m macros)
//
// .. code-block:: cpp
//
#define rb_color_m(x) (x)->color
#define rb_parent_m(x) (x)->parent
#define rb_left_m(x) (x)->left
#define rb_right_m(x) (x)->right
#define rb_value_m(x) (x)->value

#begindef rb_new_context_m(cx, type)
    typedef type cx##_type_t;
    typedef type cx##_iter_t;
    extern cx##_type_t* const cx##_nil_ptr;
#enddef

// Comparators
// ===========
//
// Some basic comparators usually you would define your own.
//
// rb_pointer_cmp_m
// ----------------
//
// Compares pointers.
//
// x, y
//    Nodes to compare
//
// .. code-block:: cpp
//
#begindef rb_pointer_cmp_m(x, y)
    ((int) (x - y))
#enddef

// rb_value_cmp_m
// ----------------
//
// Compares nodes that have the rb_value_m trait.
//
// x, y
//    Nodes to compare
//
// .. code-block:: cpp
//
#begindef rb_value_cmp_m(x, y)
    (rb_value_m(x) - rb_value_m(y))
#enddef

// Colors
// ======
//
// The obvious color plus white which is used for nodes that are currently not
// in the rbtree. This way we can assert if a node is added twice.
//
// .. code-block:: cpp
//
#define RB_BLACK 0
#define RB_RED   1

#define rb_is_black_m(x)   (x == RB_BLACK)
#define rb_is_red_m(x)     (x == RB_RED)

#define rb_make_black_m(x) x = RB_BLACK
#define rb_make_red_m(x)   x = RB_RED

// API
// ===
//
// Functions that are part of the API. The standard arguments are documented
// once:
//
// type
//    The type of the nodes in the red-black tree.
//
// nil
//    A pointer to the nil object.
//
// color
//    The color trait of the nodes in the rbtree.
//
// parent
//    The parent trait of the nodes in the rbtree is a pointer back to the
//    parent node.
//
// left
//    The left trait of the nodes in the rbtree is a pointer to the left branch
//    of the node.
//
// right
//    The right trait of the nodes in the rbtree is a pointer to the right
//    branch of the node.
//
// rb_node_init_m
// --------------
//
// Bound: cx##_node_init
//
// Initializes a node by setting the color to 0 and all pointers to nil.
//
// node
//    The node to initialize.
//
// .. code-block:: cpp
//
#begindef rb_node_init_m(
        nil,
        color,
        parent,
        left,
        right,
        node
)
{
    color(node) = 0;
    parent(node) = nil;
    left(node) = nil;
    right(node) = nil;
}
#enddef

// rb_for_cx_m
// ------------
//
// Generates a for loop header using the iterator.
//
// iter
//    The new iterator variable.
//
// elem
//    The pointer to the current element.
//
// .. code-block:: cpp
//
#begindef rb_for_cx_m(cx, tree, iter, elem)
    for(
            cx##_iter_init(tree, iter, &elem);
            elem != NULL;
            cx##_iter_next(iter, &elem)
    )
#enddef

// rb_iter_decl_m
// ---------------
//
// Also: rb_iter_decl_cx_m
//
// Declare iterator variables.
//
// iter
//    The new iterator variable.
//
// elem
//    The pointer to the current element.
//
// .. code-block:: cpp
//
#begindef rb_iter_decl_m(type, iter, elem)
    type* iter = NULL;
    type* elem = NULL;
#enddef

#begindef rb_iter_decl_cx_m(cx, iter, elem)
    cx##_type_t* iter = NULL;
    cx##_type_t* elem = NULL;
#enddef

// rb_iter_init_m
// --------------
//
// Bound: cx##_iter_init
//
// Initialize iterator. It will point to the first element.
//
// tree
//    The root node of the tree. Pointer to NULL represents an empty tree.
//
// iter
//    The iterator.
//
// elem
//    The pointer to the current element.
//
//
// .. code-block:: cpp
//
#begindef rb_iter_init_m(nil, left, tree, elem)
{
    if(tree == nil)
        elem = nil;
    else {
        elem = tree;
        while(left(elem) != nil)
            elem = left(elem);
    }
    if(elem == nil)
        elem = NULL;
}
#enddef

// rb_iter_next_m
// --------------
//
// Bound: cx##_iter_next
//
// Initialize iterator. It will point to the first element. The element fill be
// NULL, if the iteration is at the end.
//
// elem
//    The pointer to the current element.
//
// .. code-block:: cpp
//
#begindef _rb_iter_next_m(
    nil,
    parent,
    left,
    right,
    elem,
    tmp
)
do {
    tmp = right(elem);
    if(tmp != nil) {
        elem = tmp;
        while(left(elem) != nil)
            elem = left(elem);
        break;
    }
    for(;;) {
        /* Next would be the root, we are done */
        if(parent(elem) == nil) {
            elem = nil;
            break;
        }
        tmp = parent(elem);
        /* tmp is a left node, therefore it is the next node */
        if(elem == left(tmp)) {
            elem = tmp;
            break;
        }
        elem = tmp;
    }
    if(elem == nil)
        elem = NULL;
} while(0)
#enddef

#begindef rb_iter_next_m(
    nil,
    type,
    parent,
    left,
    right,
    elem
)
{
    type* __rb_next_tmp_;
    _rb_iter_next_m(
        nil,
        parent,
        left,
        right,
        elem,
        __rb_next_tmp_
    );
}
#enddef

// rb_insert_m
// ------------
//
// Bound: cx##_insert
//
// Insert the node into the tree. This function might replace the root node
// (tree). If an equal node exists in the tree, the node will not be added and
// will still be RB_WHITE.
//
// The bound function will return 0 on success.
//
// cmp
//    Comparator (rb_pointer_cmp_m or rb_value_cmp_m could be used)
//
// tree
//    The root node of the tree.
//
// node
//    The node to insert.
//
// .. code-block:: cpp
//
#begindef _rb_insert_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        cmp,
        tree,
        node,
        c, /* current */
        p, /* parent */
        r  /* result */
)
do {
    assert(node != nil && "Cannot insert nil node");
    assert(
        parent(node) == nil &&
        left(node) == nil &&
        right(node) == nil &&
        tree != node &&
        "Node already used or not initialized"
    );
    if(tree == nil) {
        tree = node;
        rb_make_black_m(color(tree));
        break;
    } else {
        assert((
            parent(tree) == nil &&
            rb_is_black_m(color(tree))
        ) && "Tree is not root");
    }
    c = tree;
    p = NULL;
    r = 0;
    while(c != nil) {
        /* The node is already in the rbtree, we break */
        r = cmp(c, node);
        if(r == 0)
            break;
        p = c;
        /* Smaller on the left, bigger on the right */
        c = r > 0 ? left(c) : right(c);
    }
    /* The node is already in the rbtree, we break */
    if(c != nil)
        break;

    parent(node) = p;
    rb_make_red_m(color(node));

    /* Lesser on the left, greater on the right */
    if(r > 0)
        left(p) = node;
    else
        right(p) = node;

    _rb_insert_fix_m(
            type,
            nil,
            color,
            parent,
            left,
            right,
            tree,
            node
    );
} while(0);
#enddef

#begindef rb_insert_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        cmp,
        tree,
        node
)
{
    type* __rb_ins_current_;
    type* __rb_ins_parent_;
    int   __rb_ins_result_;
    _rb_insert_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        cmp,
        tree,
        node,
        __rb_ins_current_,
        __rb_ins_parent_,
        __rb_ins_result_
    )
}
#enddef

// rb_delete_node_m
// ----------------
//
// Bound: cx##_delete_node
//
// Insert delete a node from the tree. This function acts on an actual tree
// node. If you don't have it use rb_find_m first or rb_delete_m. The root node
// (tree) can change.
//
// tree
//    The root node of the tree.
//
// node
//    The node to delete.
//
// .. code-block:: cpp
//
#begindef _rb_delete_node_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        x,
        y
)
{
    assert(tree != nil && "Cannot remove node from empty tree");
    assert(node != nil && "Cannot delete nil node");
    assert((
        parent(node) != nil ||
        left(node) != nil ||
        right(node) != nil ||
        rb_is_black_m(color(node))
    ) && "Node is not in a tree");
    /* This node has at least one nil node, delete is simple */
    if(left(node) == nil || right(node) == nil)
        /* The node is suitable for deletion */
        y = node;
    else {
        /* We need to find another node for deletion that as
         * only one child */
        y = right(node);
        while(left(y) != nil)
            y = left(y);
    }

    /* If y has a child we have to attach it to the parent */
    if(left(y) != nil)
        x = left(y);
    else
        x = right(y);

    /* Remove y from the tree */
    parent(x) = parent(y);
    if(parent(y) != nil) {
        if(y == left(parent(y)))
            left(parent(y)) = x;
        else
            right(parent(y)) = x;
    } else
        tree = x;

    /* A black node was removed, to fix the problem pretend to have pushed the
     * blackness onto x. Therefore x is double black and violates property 1. */
    if(rb_is_black_m(color(y))) {
        _rb_delete_fix_m(
                type,
                nil,
                color,
                parent,
                left,
                right,
                tree,
                x
        );
    }

    /* Replace y with the node since we don't control memory */
    if(node != y) {
        if(parent(node) == nil) {
            tree = y;
            parent(y) = nil;
        } else {
            if(node == left(parent(node)))
                left(parent(node)) = y;
            else if(node == right(parent(node)))
                right(parent(node)) = y;
        }
        if(left(node) != nil)
            parent(left(node)) = y;
        if(right(node) != nil)
            parent(right(node)) = y;
        parent(y) = parent(node);
        left(y) = left(node);
        right(y) = right(node);
        color(y) = color(node);
    }
    /* Clear the node */
    parent(node) = nil;
    left(node) = nil;
    right(node) = nil;
    color(node) = 0;
}
#enddef

#begindef rb_delete_node_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node
)
{
    type* __rb_del_x_;
    type* __rb_del_y_;
    _rb_delete_node_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        __rb_del_x_,
        __rb_del_y_
    )
}
#enddef

// rb_find_m
// ---------
//
// Bound: cx##_find
//
// Find a node using another node as key. The node will be set to NULL if the
// key was not found.
//
// The bound function will return 0 on success.
//
// tree
//    The root node of the tree. Pointer to NULL represents an empty tree.
//
// key
//    The node used as search key.
//
// node
//    The output node.
//
// .. code-block:: cpp

#begindef rb_find_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        cmp,
        tree,
        key,
        node
)
{
    assert(key != nil && "Do not use nil as search key");
    if(tree == nil)
        node = nil;
    else {
        node = tree;
        int __rb_find_result_ = 1;
        while(__rb_find_result_ && node != nil) {
            __rb_find_result_  = cmp(node, key);
            if(__rb_find_result_ == 0)
                break;
            node = __rb_find_result_ > 0 ? left(node) : right(node);
        }
    }
}
#enddef

// rb_replace_node_m
// -----------------
//
// Bound: cx##_replace_node
//
// Replace a node with another. The cmp(old, new) has to return 0 or the
// function won't do anything.
//
// The bound function will return 0 on success.
//
// tree
//    The root node of the tree. Pointer to NULL represents an empty tree.
//
// old
//    The node to be replaced.
//
// new
//    The new node.
//
// .. code-block:: cpp

#begindef rb_replace_node_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        cmp,
        tree,
        old,
        new
)
{
    assert(tree != nil && "The tree can't be nil");
    assert(old != nil && "The old node can't be nil");
    assert(new != nil && "The new node can't be nil");
    assert(new != old && "The old and new node must differ");
    if(cmp(old, new) == 0) {
        if(old == tree)
            tree = new;
        else {
            if(old == left(parent(old)))
                left(parent(old)) = new;
            else
                right(parent(old)) = new;
        }
        if(left(old) != nil)
            parent(left(old)) = new;
        if(right(old) != nil)
            parent(right(old)) = new;
        parent(new) = parent(old);
        left(new) = left(old);
        right(new) = right(old);
        color(new) = color(old);
        /* Clear the old node */
        parent(old) = nil;
        left(old) = nil;
        right(old) = nil;
        color(old) = 0;
    }
}
#enddef

// rb_bind_decl_m
// --------------
//
// Bind rbtree functions to a context. This only generates declarations.
//
// rb_bind_decl_cx_m is just an alias for consistency.
//
// cx
//    Name of the new context.
//
// type
//    The type of the nodes in the red-black tree.
//
// .. code-block:: cpp
//
#begindef rb_bind_decl_cx_m(cx, type)
    rb_new_context_m(cx, type)
    void
    cx##_tree_init(
            type** tree
    );
    void
    cx##_iter_init(
            type* tree,
            cx##_iter_t* iter,
            type** elem
    );
    void
    cx##_iter_next(
            cx##_iter_t* iter,
            type** elem
    );
    void
    cx##_node_init(
            type* node
    );
    int
    cx##_insert(
            type** tree,
            type* node
    );
    void
    cx##_delete_node(
            type** tree,
            type* node
    );
    int
    cx##_delete(
            type** tree,
            type* key
    );
    int
    cx##_replace_node(
            type** tree,
            type* old,
            type* new
    );
    int
    cx##_replace(
            type** tree,
            type* key,
            type* new
    );
    int
    cx##_find(
            type* tree,
            type* key,
            type** node
    );
    void
    cx##_check_tree(type* tree);
    void
    cx##_check_tree_rec(
            type* node,
            int depth,
            int *pathdepth
    );
#enddef
#define rb_bind_decl_m(cx, type) rb_bind_decl_cx_m(cx, type)

// rb_bind_impl_m
// --------------
//
// Bind rbtree functions to a context. This only generates implementations.
//
// rb_bind_impl_m uses the standard traits: rb_color_m, rb_parent_m,
// rb_left_m, rb_right_m, whereas rb_bind_impl_cx_m expects you to create:
// cx##_color_m, cx##_parent_m, cx##_left_m, cx##_right_m.
//
// cx
//    Name of the new context.
//
// type
//    The type of the nodes in the red-black tree.
//
// .. code-block:: cpp
//
#begindef _rb_bind_impl_tr_m(
        cx,
        type,
        color,
        parent,
        left,
        right,
        cmp
)
    cx##_type_t cx##_nil_mem;
    cx##_type_t* const cx##_nil_ptr = &cx##_nil_mem;
    void
    cx##_tree_init(
            type** tree
    )
    {
        rb_node_init_m(
                cx##_nil_ptr,
                color,
                parent,
                left,
                right,
                cx##_nil_ptr
        );
        *tree = cx##_nil_ptr;
    }
    void
    cx##_iter_init(
            type* tree,
            cx##_iter_t* iter,
            type** elem
    )
    {
        (void)(iter);
        rb_iter_init_m(
            cx##_nil_ptr,
            left,
            tree,
            *elem
        );
    }
    void
    cx##_iter_next(
            cx##_iter_t* iter,
            type** elem
    )
    {
        (void)(iter);
        rb_iter_next_m(
            cx##_nil_ptr,
            type,
            parent,
            left,
            right,
            *elem
        )
    }
    void
    cx##_node_init(
            type* node
    )
    {
        rb_node_init_m(
                cx##_nil_ptr,
                color,
                parent,
                left,
                right,
                node
        );
    }
    int
    cx##_insert(
            type** tree,
            type* node
    )
    {
        rb_insert_m(
            type,
            cx##_nil_ptr,
            color,
            parent,
            left,
            right,
            cmp,
            *tree,
            node
        );
        return !(
            parent(node) != cx##_nil_ptr ||
            left(node) != cx##_nil_ptr ||
            right(node) != cx##_nil_ptr ||
            *tree == node
        );
    }
    void
    cx##_delete_node(
            type** tree,
            type* node
    ) rb_delete_node_m(
        type,
        cx##_nil_ptr,
        color,
        parent,
        left,
        right,
        *tree,
        node
    )
    int
    cx##_delete(
            type** tree,
            type* key
    )
    {
        type* node;
        if(cx##_find(*tree, key, &node) == 0) {
            cx##_delete_node(tree, node);
            return 0;
        }
        return 1;
    }
    int
    cx##_replace_node(
            type** tree,
            type* old,
            type* new
    )
    {
        rb_replace_node_m(
            type,
            cx##_nil_ptr,
            color,
            parent,
            left,
            right,
            cmp,
            *tree,
            old,
            new
        );
        return !(
            parent(old) == cx##_nil_ptr &&
            left(old) == cx##_nil_ptr &&
            right(old) == cx##_nil_ptr &&
            old != *tree
        );
    }
    int
    cx##_replace(
            type** tree,
            type* key,
            type* new
    )
    {
        type* old;
        if(cx##_find(*tree, key, &old) == 0) {
            return cx##_replace_node(tree, old, new);
        }
        return 1;
    }
    int
    cx##_find(
            type* tree,
            type* key,
            type** node
    )
    {
        rb_find_m(
            type,
            cx##_nil_ptr,
            color,
            parent,
            left,
            right,
            cmp,
            tree,
            key,
            *node
        );
        return *node == cx##_nil_ptr;
    }
    void
    cx##_check_tree(type* tree)
    {
        int pathdepth = -1;
        cx##_check_tree_rec(tree, 0, &pathdepth);
    }
    void
    cx##_check_tree_rec(
            type* node,
            int depth,
            int *pathdepth
    ) rb_check_tree_m(
        cx,
        type,
        color,
        parent,
        left,
        right,
        cmp,
        node,
        depth,
        *pathdepth
    )
#enddef

#begindef rb_bind_impl_cx_m(cx, type)
    _rb_bind_impl_tr_m(
        cx,
        type,
        cx##_color_m,
        cx##_parent_m,
        cx##_left_m,
        cx##_right_m,
        cx##_cmp_m
    )
#enddef

#begindef rb_bind_impl_m(cx, type)
    _rb_bind_impl_tr_m(
        cx,
        type,
        rb_color_m,
        rb_parent_m,
        rb_left_m,
        rb_right_m,
        cx##_cmp_m
    )
#enddef

#begindef rb_bind_cx_m(cx, type)
    rb_bind_decl_cx_m(cx, type)
    rb_bind_impl_cx_m(cx, type)
#enddef

#begindef rb_bind_m(cx, type)
    rb_bind_decl_m(cx, type)
    rb_bind_impl_m(cx, type)
#enddef

// rb_check_tree_m
// ----------------
//
// Recursive: only works bound cx##_check_tree
//
// Check consistency of a tree
//
// node
//    Node to check
//
// result
//    Zero on success, other on failure
//
// .. code-block:: cpp
//
#begindef _rb_check_tree_m(
        cx,
        type,
        color,
        parent,
        left,
        right,
        cmp,
        node,
        depth,
        pathdepth,
        tmp
)
{
    type* nil = cx##_nil_ptr;
    if(node == nil) {
        if(pathdepth < 0)
            pathdepth = depth;
        else
            assert(pathdepth == depth);
    } else {
        tmp = left(node);
        if(tmp != nil) {
            assert(parent(tmp) == node);
            assert(cmp(tmp, node) < 0);
        }
        tmp = right(node);
        if(tmp != nil) {
            assert(parent(tmp) == node);
            assert(cmp(tmp, node) > 0);
        }
        if(rb_is_red_m(color(node))) {
            tmp = left(node);
            if(tmp != nil)
                assert(rb_is_black_m(color(tmp)));
            tmp = right(node);
            if(tmp != nil)
                assert(rb_is_black_m(color(tmp)));
            cx##_check_tree_rec(left(node), depth, &pathdepth);
            cx##_check_tree_rec(right(node), depth, &pathdepth);
        } else {
            cx##_check_tree_rec(left(node), depth + 1, &pathdepth);
            cx##_check_tree_rec(right(node), depth + 1, &pathdepth);
        }
    }
}
#enddef
#begindef rb_check_tree_m(
        cx,
        type,
        color,
        parent,
        left,
        right,
        cmp,
        node,
        depth,
        pathdepth
)
{
    type* __rb_check_tmp_;
    _rb_check_tree_m(
        cx,
        type,
        color,
        parent,
        left,
        right,
        cmp,
        node,
        depth,
        pathdepth,
        __rb_check_tmp_
    )
}
#enddef

// Private
// =======
//
// Functions that are used internally.
//
// _rb_rotate_left_m
// ------------------
//
// Internal: not bound
//
// A rotation is a local operation in a search tree that preserves in-order
// traversal key ordering. Used to fix insert/deletion discrepancies. This
// operation might change the current root.
//
// _rb_rotate_right_m is _rb_rotate_left_m where left and right had been
// switched.
//
// tree
//    The root node of the tree.
//
// node
//    The node to initialize.
//
// .. code-block:: text
//
//               .---.   rotate_right   .---.
//               | y |     ------->     | x |
//               .---.                  .---.
//              /     ∖                /     ∖
//         .---'     .-'-.        .---'      .'--.
//         | x |     | C |        | A |      | y |
//         .---.     '---'        '---'      .---.
//        /     ∖                           /     ∖
//     .-'-.    .'--.                    .-'-.    .'--.
//     | A |    | B |      <------       | B |    | C |
//     '---'    '---'    rotate_left     '---'    '---'
//
// .. code-block:: cpp
//
#begindef __rb_rotate_left_m(
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        x,
        y
)
{
    x = node;
    y = right(x);

    /* Turn y's left sub-tree into x's right sub-tree */
    right(x) = left(y);
    if(left(y) != nil)
        parent(left(y)) = x;
    /* y's new parent was x's parent */
    parent(y) = parent(x);
    if(parent(x) == nil)
        /* If x is root y becomes the new root */
        tree = y;
    else {
        /* Set the parent to point to y instead of x */
        if(x == left(parent(x)))
            /* x was on the left of its parent */
            left(parent(x)) = y;
        else
            /* x must have been on the right */
            right(parent(x)) = y;
    }
    /* Finally, put x on y's left */
    left(y) = x;
    parent(x) = y;
}
#enddef

#begindef _rb_rotate_left_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node
)
{
    type* __rb_rot_x_;
    type* __rb_rot_y_;
    __rb_rotate_left_m(
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        __rb_rot_x_,
        __rb_rot_y_
    );
}
#enddef

#begindef _rb_rotate_left_tr_m(cx, tree, node)
    _rb_rotate_left_m(
        cx##_type_t,
        cx##_nil_ptr,
        rb_color_m,
        rb_parent_m,
        rb_left_m,
        rb_right_m,
        tree,
        node
    )
#enddef

#begindef _rb_rotate_right_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node
)
    _rb_rotate_left_m(
        type,
        nil,
        color,
        parent,
        right, /* Switched */
        left,  /* Switched */
        tree,
        node
    )
#enddef

#begindef _rb_rotate_right_tr_m(cx, tree, node)
    _rb_rotate_right_m(
        cx##_type_t,
        cx##_nil_ptr,
        rb_color_m,
        rb_parent_m,
        rb_left_m,
        rb_right_m,
        tree,
        node
    )
#enddef

// _rb_insert_fix_m
// ----------------
//
// Internal: not bound
//
// After insert new node is labeled red, and possibly destroys the red-black
// property. The main loop moves up the tree, restoring the red-black property.
//
// tree
//    The root node of the tree.
//
// node
//    The start-node to fix.
//
// .. code-block:: cpp
//
#begindef __rb_insert_fix_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        x,
        y
)
{
    x = node;
    /* Move up the tree and fix property 3. */
    while(
            (x != tree) &&
            rb_is_red_m(color(parent(x)))
    ) {
        if(parent(x) == left(parent(parent(x)))) {
            _rb_insert_fix_node_m(
                type,
                nil,
                color,
                parent,
                left,
                right,
                _rb_rotate_left_m,
                _rb_rotate_right_m,
                tree,
                x,
                y
            );
        } else {
            _rb_insert_fix_node_m(
                type,
                nil,
                color,
                parent,
                right, /* Switched */
                left, /* Switched */
                _rb_rotate_left_m,
                _rb_rotate_right_m,
                tree,
                x,
                y
            );
        }
    }
    rb_make_black_m(color(tree));
}
#enddef

#begindef _rb_insert_fix_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node
)
{
    type* __rb_insf_x_;
    type* __rb_insf_y_;
    __rb_insert_fix_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        __rb_insf_x_,
        __rb_insf_y_
    );
}
#enddef

#begindef _rb_insert_fix_node_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        rot_left,
        rot_right,
        tree,
        x,
        y
)
{
    y = right(parent(parent(x)));
    /* Case 1: z’s uncle y is red. */
    if(rb_is_red_m(color(y))) {
        rb_make_black_m(color(parent(x)));
        rb_make_black_m(color(y));
        rb_make_red_m(color(parent(parent(x))));
        /* Locally property 3 is fixed, but changing the color of the
         * grandparent might have created a new violation. We continue with the
         * grandparent. */
        x = parent(parent(x));
    } else {
        /* Case 2: z’s uncle y is black and z is a right child. */
        if(x == right(parent(x))) {
            x = parent(x);
            rot_left(
                type,
                nil,
                color,
                parent,
                left,
                right,
                tree,
                x
            );
        }
        /* Case 3: z’s uncle y is black and z is a left child. */
        rb_make_black_m(color(parent(x)));
        rb_make_red_m(color(parent(parent(x))));
        rot_right(
            type,
            nil,
            color,
            parent,
            left,
            right,
            tree,
            parent(parent(x))
        );
    }
}
#enddef

// _rb_delete_fix_m
// ----------------
//
// Internal: not bound
//
// After delete the node was labeled black, and possibly destroys the red-black
// property. The main loop moves up the tree, restoring the red-black property.
//
// tree
//    The root node of the tree.
//
// node
//    The start-node to fix.
//
// .. code-block:: cpp
//
#begindef __rb_delete_fix_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        x,
        y
)
{
    x = node;
    /* Move up extra blackness till x is red. */
    while(
            (x != tree) &&
            rb_is_black_m(color(x))
    ) {
        if(x == left(parent(x))) {
            _rb_delete_fix_node_m(
                type,
                nil,
                color,
                parent,
                left,
                right,
                _rb_rotate_left_m,
                _rb_rotate_right_m,
                tree,
                x,
                y
            );
        } else {
            _rb_delete_fix_node_m(
                type,
                nil,
                color,
                parent,
                right, /* Switched */
                left, /* Switched */
                _rb_rotate_left_m,
                _rb_rotate_right_m,
                tree,
                x,
                y
            );
        }
    }
    /* If x is red we can introduce a real black node. */
    rb_make_black_m(color(x));
}
#enddef

#begindef _rb_delete_fix_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node
)
{
    type* __rb_delf_x_;
    type* __rb_delf_y_;
    __rb_delete_fix_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        tree,
        node,
        __rb_delf_x_,
        __rb_delf_y_
    );
}
#enddef

#begindef _rb_delete_fix_node_m(
        type,
        nil,
        color,
        parent,
        left,
        right,
        rot_left,
        rot_right,
        tree,
        x,
        w
)
{
    /* X is double (extra) black. Goal: introduce a real black node. */
    w = right(parent(x));
    /* Case 1: x’s sibling w is red. */
    if(rb_is_red_m(color(w))) {
        rb_make_black_m(color(w));
        rb_make_red_m(color(parent(x)));
        rot_left(
            type,
            nil,
            color,
            parent,
            left,
            right,
            tree,
            parent(x)
        );
        /* Transforms into case 2, 3 or 4 */
        w = right(parent(x));
    }
    if(
            rb_is_black_m(color(left(w))) &&
            rb_is_black_m(color(right(w)))
    ) {
        /* Case 2: x’s sibling w is black, and both of w’s children are black. */
        rb_make_red_m(color(w));
        /* Double blackness move up. Reenter loop. */
        x = parent(x);
    } else {
        /* Case 3: x’s sibling w is black, w’s left child is red, and w’s right
         * child is black. */
        if(rb_is_black_m(color(right(w)))) {
            rb_make_black_m(color(left(w)));
            rb_make_red_m(color(w));
            rot_right(
                type,
                nil,
                color,
                parent,
                left,
                right,
                tree,
                w
            );
            w = right(parent(x));
        }
        /* Case 3: x’s sibling w is black, w’s left child is red, and w’s right
         * child is black. */
        color(w) = color(parent(x));
        rb_make_black_m(color(parent(x)));
        rb_make_black_m(color(right(w)));
        rot_left(
            type,
            nil,
            color,
            parent,
            left,
            right,
            tree,
            parent(x)
        );
        /* Terminate the loop. */
        x = tree;
    }
    /* When the loop ends x is red and will be colored black. */
}
#enddef
