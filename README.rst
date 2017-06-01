TODO: Check all instances of NULL
TODO: When insert and delete are working shrink again
TODO: Grep for TODOs
TODO: User docu
TODO: Review

==============
Red-Black Tree
==============

* Textbook implementation
* Has parent pointers
* No optimizations
* Composable
* Readable
* Generic
* Still fast
* Easy to use, a bit complex to extend because it is generic [1]_

.. [1] My rgc preprocessor and its MACRO_DEBUG mode are very helpful.

WORK IN PROGRESS
================

Installation
============

Copy rbtree.h into your source.

Development
===========

Developed on github_

.. _github: https://github.com/adfinis-sygroup/rbtree

Requirements
------------

* gcc or clang
* hypothesis
* cffi
* rst2html

Build and test
--------------

.. code-block:: bash

   make

Debug macros
------------

.. code-block:: bash

   make clean all MACRO_DEBUG=True

This will expand the macros into .c files, so gdb can step into them.

Usage
=====

Short
-----

You have to bind your functions to a context first. The context has two
function:

* Defining the type the bound function operate on

* Allowing function composition. For example my_reduce, will use the
  functions my_iter_*.

Detailed
--------

Every function x comes in two flavors

cx_x
   These functions are bound to a type. Traits and the comparator are mapped
   to the context. You have to define the type and the traits for the
   context and then you bind the function.

   .. code-block:: cpp

      #define my_color_m(x) (x)->color
      #define my_parent_m(x) (x)->parent
      #define my_left_m(x) (x)->left
      #define my_right_m(x) (x)->right
      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
      rb_bind_cx_m(my, node_t)

   .. code-block:: cpp

      my_tree_init(&tree);
      my_node_init(node);

   There is also a short if you know your are going to use all standard
   fields in your struct (color, parent, left right)

   .. code-block:: cpp

      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
      rb_bind_m(my, node_t)

   .. code-block:: cpp

      my_tree_init(&tree);
      my_node_init(node);

   Of course usually you want to split declaration and implementation of the
   function so it is. example.h:

   .. code-block:: cpp

      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
      rb_bind_decl_m(my, node_t)

   And example.c:

   .. code-block:: cpp

      #include "example.h"
      rb_bind_impl_m(my, node_t)

      int main(void) { my_node_init(node); return 0; }

rb_x_m
   These functions are macros and take a type and traits as standard
   arguments and are the most verbose. Used to build upon rbtree. For
   example prbtree (persistent rbtree) will use these function.

   .. code-block:: cpp

      rb_node_init_m(
          rb_color_m,
          rb_parent_m,
          rb_left_m,
          rb_right_m,
          node
      );

Questions
=========

Why don't you just generate typed functions from the beginning?
   I want to be able to reuse and compose my code. Especially for
   composability I need access to the generic functions. Thats how st_map_m,
   st_filter_m and st_reduce_m can use the rbtree. Thats the reason we need
   granular/late binding (generation of typed functions).

Why is the iterator so complicated?
   rbtree is part of a larger set of data-structures, some need more
   complicated iterator setups, to make the data-structures interchangeable,
   all have to follow the iterator protocol. use rb_for_cx_m.

Performance
===========

.. image:: https://github.com/ganwell/rbtree/raw/master/perf01.png
   :width: 90%
   :align: center
   :alt: insert

Implementation
==============

Based on the following references: auckland1_, auckland2_, sglib_

.. _auckland1: https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
.. _auckland2: https://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.txt
.. _sglib: http://sglib.sourceforge.net/doc/index.html#rbtree_api1

Assertion
=========

.. code-block:: cpp

   #include <assert.h>
   

Basic traits
============

Traits used by default (x_m macros)

.. code-block:: cpp

   #define rb_color_m(x) (x)->color
   #define rb_parent_m(x) (x)->parent
   #define rb_left_m(x) (x)->left
   #define rb_right_m(x) (x)->right
   #define rb_value_m(x) (x)->value
   
   #begindef rb_new_context_m(cx, type)
       typedef type cx##_type_t;
       typedef type cx##_iter_t;
       extern cx##_type_t* cx##_nil_ptr;
   #enddef
   
Comparators
===========

Some basic comparators usually you would define your own.

rb_pointer_cmp_m
----------------

Compares pointers.

x, y
   Nodes to compare

.. code-block:: cpp

   #begindef rb_pointer_cmp_m(x, y)
       ((int) (x - y))
   #enddef
   
rb_value_cmp_m
----------------

Compares nodes that have the rb_value_m trait.

x, y
   Nodes to compare

.. code-block:: cpp

   #begindef rb_value_cmp_m(x, y)
       (rb_value_m(x) - rb_value_m(y))
   #enddef
   
Colors
======

The obvious color plus white which is used for nodes that are currently not
in the rbtree. This way we can assert if a node is added twice.

.. code-block:: cpp

   #define RB_RED   (1 << 0)
   #define RB_ROOT  (1 << 1)
   #define RB_COPY  (1 << 2) /* Used in future for persistent rbtrees */
   
   #define rb_is_red_m(x)     (x & RB_RED)
   #define rb_is_black_m(x) (!(x & RB_RED))
   #define rb_needs_copy_m(x) (x & RB_COPY)
   
   #define rb_make_black_m(x) x &= ~RB_RED
   #define rb_make_red_m(x)   x |= RB_RED
   #define rb_set_copy_m(x)   x |= RB_COPY
   #define rb_unset_copy_m(x) x &= ~RB_COPY
   
API
===

Functions that are part of the API. The standard arguments are documented
once:

type
   The type of the nodes in the red-black tree.

nil
   A pointer to the nil object.

color
   The color trait of the nodes in the rbtree.

parent
   The parent trait of the nodes in the rbtree is a pointer back to the
   parent node.

left
   The left trait of the nodes in the rbtree is a pointer to the left branch
   of the node.

right
   The right trait of the nodes in the rbtree is a pointer to the right
   branch of the node.

rb_node_init_m
--------------

Bound: cx##_node_init

Initializes a node by setting the color to 0 and all pointers to nil.

node
   The node to initialize.

.. code-block:: cpp

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
   
rb_for_cx_m
------------

Generates a for loop header using the iterator.

iter
   The new iterator variable.

elem
   The pointer to the current element.

.. code-block:: cpp

   #begindef rb_for_cx_m(cx, tree, iter, elem)
       for(
               cx##_iter_init(tree, iter, &elem);
               elem != NULL;
               cx##_iter_next(iter, &elem)
       )
   #enddef
   
rb_iter_decl_m
---------------

Also: rb_iter_decl_cx_m

Declare iterator variables.

iter
   The new iterator variable.

elem
   The pointer to the current element.

.. code-block:: cpp

   #begindef rb_iter_decl_m(type, iter, elem)
       type* iter = NULL;
       type* elem = NULL;
   #enddef
   
   #begindef rb_iter_decl_cx_m(cx, iter, elem)
       cx##_type_t* iter = NULL;
       cx##_type_t* elem = NULL;
   #enddef
   
rb_iter_init_m
--------------

Bound: cx##_iter_init

Initialize iterator. It will point to the first element.

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

iter
   The iterator.

elem
   The pointer to the current element.


.. code-block:: cpp

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
   
rb_iter_next_m
--------------

Bound: cx##_iter_next

Initialize iterator. It will point to the first element. The element fill be
NULL, if the iteration is at the end.

elem
   The pointer to the current element.

.. code-block:: cpp

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
   
rb_insert_m
------------

Bound: cx##_insert

Insert the node into the tree. This function might replace the root node
(tree). If an equal node exists in the tree, the node will not be added and
will still be RB_WHITE.

The bound function will return 0 on success.

cmp
   Comparator (rb_pointer_cmp_m or rb_value_cmp_m could be used)

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

node
   The node to insert.

.. code-block:: cpp

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
       assert(node != NULL && "Cannot insert NULL node");
       assert(node != nil && "Cannot insert nil node");
       assert(((
           parent(node) == nil &&
           left(node) == nil &&
           right(node) == nil
       ) || rb_is_black_m(color(node))) && "Node already used or not initialized");
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
   
       /* Smaller on the left, bigger on the right */
       if(r > 0) {
           assert(left(p) == nil);
           left(p) = node;
       } else {
           assert(right(p) == nil);
           right(p) = node;
       }
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
   
rb_delete_node_m
------------

Bound: cx##_delete_node

Insert delete a node from the tree. This function acts on an actual tree
node. If you don't have it use rb_find_m first or rb_delete_m. The root node
(tree) can change.

tree
   The root node of the tree.

node
   The node to delete.

.. code-block:: cpp

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
       assert(node != nil && "Cannot insert nil node");
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
   
       /* If node (y) has a child we have to attach it to the parent */
       if(left(y) != nil)
           x = left(y);
       else
           x = right(y);
   
       /* Remove node from the tree */
       if(x != nil)
           parent(x) = parent(y);
       if(parent(y) != nil) {
           if(y == left(parent(y)))
               left(parent(y)) = x;
           else
               right(parent(y)) = x;
       } else
           tree = x;
   
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
   
       /* This code is here instead of copying the data, if we own the memory we
        * could do rb_value_m(node) = rb_value_m(y) */
       if(node != y) {
           if(parent(node) == nil) {
               tree = y;
               parent(y) = nil;
           } else {
               if(node == left(parent(node)))
                   left(parent(node)) = y;
               if(node == right(parent(node)))
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
   
rb_bind_decl_m
--------------

Bind rbtree functions to a context. This only generates declarations.

rb_bind_decl_cx_m is just an alias for consistency.

cx
   Name of the new context.

type
   The type of the nodes in the red-black tree.

.. code-block:: cpp

   #begindef rb_bind_decl_cx_m(cx, type)
       rb_new_context_m(cx, type)
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
   
rb_bind_impl_m
--------------

Bind rbtree functions to a context. This only generates implementations.

rb_bind_impl_m uses the standard traits: rb_color_m, rb_parent_m,
rb_left_m, rb_right_m, whereas rb_bind_impl_cx_m expects you to create:
cx##_color_m, cx##_parent_m, cx##_left_m, cx##_right_m.

cx
   Name of the new context.

type
   The type of the nodes in the red-black tree.

.. code-block:: cpp

   #begindef _rb_bind_impl_tr_m(
           cx,
           type,
           color,
           parent,
           left,
           right,
           cmp
   )
       cx##_type_t* cx##_nil_ptr = NULL;
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
           return (
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
   
rb_check_tree_m
----------------

Recursive: only works bound cx##_check_tree

Check consistency of a tree

node
   Node to check

result
   Zero on success, other on failure

.. code-block:: cpp

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
   
Private
=======

Functions that are used internally.

_rb_rotate_left_m
------------------

Internal: not bound

A rotation is a local operation in a search tree that preserves in-order
traversal key ordering. Used to fix insert/deletion discrepancies. This
operation might change the current root.

_rb_rotate_right_m is _rb_rotate_left_m where left and right had been
switched.

tree
   The root node of the tree.

node
   The node to initialize.

.. code-block:: text

              .---.   rotate_right   .---.
              | y |     ------->     | x |
              .---.                  .---.
             /     ∖                /     ∖
        .---'     .-'-.        .---'      .'--.
        | x |     | C |        | A |      | y |
        .---.     '---'        '---'      .---.
       /     ∖                           /     ∖
    .-'-.    .'--.                    .-'-.    .'--.
    | A |    | B |      <------       | B |    | C |
    '---'    '---'    rotate_left     '---'    '---'

.. code-block:: cpp

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
       /* Set the parent to point to y instead of x */
       /* First see whether we're at the root */
       if(parent(x) != nil) {
           if(x == left(parent(x)))
               /* x was on the left of its parent */
               left(parent(x)) = y;
           else
               /* x must have been on the right */
               right(parent(x)) = y;
       } else
           tree = y;
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
   
_rb_insert_fix_m
----------------

Internal: not bound

After insert new node is labeled red, and possibly destroys the red-black
property. The main loop moves up the tree, restoring the red-black property.

tree
   The root node of the tree.

node
   The start-node to fix.

.. code-block:: cpp

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
       if(y != nil && rb_is_red_m(color(y))) {
           rb_make_black_m(color(parent(x)));
           rb_make_black_m(color(y));
           rb_make_red_m(color(parent(parent(x))));
           x = parent(parent(x));
       } else {
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
   
_rb_delete_fix_m
----------------

Internal: not bound

TODO

tree
   The root node of the tree.

node
   The start-node to fix.

.. code-block:: cpp

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
       if(x != nil) {
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
           rb_make_black_m(color(x));
       }
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
           y
   )
   {
       y = right(parent(x));
       if(rb_is_red_m(color(y))) {
           rb_make_black_m(color(y));
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
           y = right(parent(x));
       }
       if(
               (left(y) == nil || rb_is_black_m(color(left(y)))) &&
               (right(y) == nil || rb_is_black_m(color(right(y))))
       ) {
           rb_make_red_m(color(y));
           x = parent(x);
       } else {
           if(right(y) == nil || rb_is_black_m(color(right(y)))) {
               rb_make_black_m(color(left(y)));
               rb_make_red_m(color(y));
               rot_right(
                   type,
                   nil,
                   color,
                   parent,
                   left,
                   right,
                   tree,
                   y
               );
               y = right(parent(x));
           }
           color(y) = color(parent(x));
           rb_make_black_m(color(parent(x)));
           rb_make_black_m(color(right(y)));
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
           x = tree;
       }
   }
   #enddef
   
_rb_switch_node_m
------------

Internal not bound.

Switch two nodes.

tree
   The root node of the tree.

x
   The node to switch.

y
   The node to switch.

.. code-block:: cpp

   #begindef __rb_switch_node_m(
           type,
           nil,
           parent,
           left,
           right,
           tree,
           x,
           y,
           t /* tmp parent */
   )
   {
       if(x != y) {
           /* Switch parents */
           if(y == parent(x)) {
               parent(x) = parent(y);
               parent(y) = x;
           } else if(x == parent(y)) {
               parent(y) = parent(x);
               parent(x) = y;
           } else {
               t = parent(x);
               parent(x) = parent(y);
               parent(y) = t;
           }
           /* Switch childs */
           t = left(x);
           left(x) = left(y);
           left(y) = t;
           t = right(x);
           right(x) = right(y);
           right(y) = t;
           /* Fix parents */
           t = parent(x);
           if(t == parent(y)) {
               /* Siblings */
               if(x == left(t)) {
                   left(t) = y;
                   right(t) = x;
               } else {
                   left(t) = x;
                   right(t) = y;
               }
           } else {
               if(t != nil) {
                   if(y == left(t))
                       left(t) = x;
                   else
                       right(t) = x;
               } else
                   tree = x;
               t = parent(y);
               if(t != nil) {
                   if(x == left(t))
                       left(t) = y;
                   else
                       right(t) = y;
               } else
                   tree = y;
           }
           /* Fix other nodes */
           if (left(x) != nil)
               parent(left(x)) = x;
           if (left(y) != nil)
               parent(left(y)) = y;
           if (right(x) != nil)
               parent(right(x)) = x;
           if (right(y) != nil)
               parent(right(y)) = y;
       }
   }
   #enddef
   
   #begindef _rb_switch_node_m(
           type,
           nil,
           parent,
           left,
           right,
           tree,
           node,
           new
   )
   {
       type *__rb_repl_tmp_;
       __rb_switch_node_m(
           type,
           nil,
           parent,
           left,
           right,
           tree,
           node,
           new,
           __rb_repl_tmp_ /* t */
       )
   }
   #enddef
