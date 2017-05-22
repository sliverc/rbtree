==============
Red-Black Tree
==============

WORK IN PROGRESS
================

Installation
============

Copy rbtree.h into your source.

Development
===========

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

Detailed
--------

Every function x comes in three flavors

x_m
   These functions only take a context as standard argument, but they assume
   you use the default traits (rb_color, rb_parent_m, rb_left_m, rb_right_m).
   Therefore you only have to define the type and comparator.

   .. code-block:: cpp

      rb_new_context_m(my, node_t)
      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)

   .. code-block:: cpp

      rb_node_init_m(my, node);


x_cx_m
   These functions only take a context as standard argument. Type and traits
   are mapped to the context. You have to define the type and the traits for
   the context.

   .. code-block:: cpp

      rb_new_context_m(my, node_t)
      #define my_color_m(x) (x)->color
      #define my_parent_m(x) (x)->parent
      #define my_left_m(x) (x)->left
      #define my_right_m(x) (x)->right
      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)

   .. code-block:: cpp

      rb_node_init_cx_m(my, node);

x_tr_m
   These functions take a type and traits as standard arguments and are the
   most verbose.

   .. code-block:: cpp

      rb_node_init_tr_m(
          node_t,
          rb_color_m,
          rb_parent_m,
          rb_left_m,
          rb_right_m,
          node
      );


Usually you would the x_m functions and add color, parent, left and right
fields to the structure used. If you want to use different fields you need
to use x_cx_m. The x_tr_m function can usually just be ignored and are only
interesting if you are extending the functionality of rbtree.

Questions
=========

Why don't you just generate typed functions from the beginning?
   I want to be able to reuse and compose my code. Especially for
   composability I need access to the generic functions. Thats how st_map_m,
   st_filter_m and st_reduce_m can use the rbtree. Thats the reason we need
   granular/late binding (generation of typed functions).


Implementation
==============

Based on the following references: auckland1_, auckland2_

.. _auckland1: https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
.. _auckland2: https://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.txt

Assertion
=========

.. code-block:: cpp

   #ifndef RB_A
   #   define RB_A(x) assert(x)
   #   include <assert.h>
   #endif


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
       rb_value_m(x) - rb_value_m(y)
   #enddef
   
Colors
======

The obvious color plus white which is used for nodes that are currently not
in the rbtree. This way we can assert if a node is added twice.

.. code-block:: cpp

   #define RB_WHITE  0
   #define RB_BLACK (1 << 0)
   #define RB_ROOT  (1 << 1)
   #define RB_COPY  (1 << 2) /* Used in future for persistent rbtrees */
   
   #define rb_is_white_m(x)   x == RB_WHITE
   #define rb_is_red_m(x)   !(x & RB_BLACK)
   #define rb_is_black_m(x)   x & RB_BLACK
   #ifdef NDEBUG
   #   define rb_is_root_m(x)    1
   #else
   #   define rb_is_root_m(x)    x & RB_ROOT /* Special black :-p */
   #endif
   #define rb_needs_copy_m(x) x & RB_COPY
   
   #define rb_make_white_m(x) x = RB_WHITE
   #define rb_make_black_m(x) x |= RB_BLACK
   #define rb_make_red_m(x)   x &= ~RB_BLACK
   #ifdef NDEBUG
   #   define rb_make_root_m(x)  x = RB_BLACK
   #   define rb_set_root_m(x)
   #   define rb_unset_root_m(x)
   #else
   #   define rb_make_root_m(x)  x = RB_BLACK | RB_ROOT
   #   define rb_set_root_m(x)   x |= RB_ROOT
   #   define rb_unset_root_m(x) x &= ~RB_ROOT
   #endif
   #define rb_set_copy_m(x)   x |= RB_COPY
   #define rb_unset_copy_m(x) x &= ~RB_COPY
   
API
===

Functions that are part of the API. The standard arguments are documented
once:

type
   The type of the nodes in the red-black tree.

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

rb_node_init_tr_m
-----------------

Also: rb_node_init_cx_m, rb_node_init_m

Initializes a node by setting the color to RB_WHITE and all pointers to
NULL.

node
   The node to initialize.

.. code-block:: cpp

   #begindef rb_node_init_tr_m(
           type,
           color,
           parent,
           left,
           right,
           node
   )
   {
       rb_make_white_m(color(node));
       parent(node) = NULL;
       left(node) = NULL;
       right(node) = NULL;
   }
   #enddef
   
   #begindef rb_node_init_cx_m(cx, node)
       rb_node_init_tr_m(
           void,
           cx##_color_m,
           cx##_parent_m,
           cx##_left_m,
           cx##_right_m,
           node
       )
   #enddef
   
   #begindef rb_node_init_m(cx, node)
       rb_node_init_cx_m(rb, node)
   #enddef
   
rb_insert_tr_m
--------------

Also: rb_insert_cx_m, rb_insert_m

Insert the node into the tree. This function might replace the root node
(tree). If an equal node exists in the tree node will note added an will
still be RB_WHITE.

cmp
   Comparator (rb_pointer_cmp_m or rb_value_cmp_m could be used)

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

node
   The node to initialize.

.. code-block:: cpp

   #begindef _rb_insert_tr_m(
           type,
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
       RB_A(node != NULL);
       RB_A(rb_is_white_m(color(node)));
       if(tree == NULL) {
           tree = node;
           rb_make_root_m(color(tree));
       } else {
           RB_A(rb_is_root_m(color(tree)));
       }
       c = tree;
       p = NULL;
       r = 0;
       while(c != NULL) {
           /* The node is already in the rbtree, we break */
           r = cmp(c, node);
           if(r == 0)
               break;
           p = c;
           /* Smaller on the left, bigger on the right */
           c = r > 0 ? left(node) : right(node);
       }
       /* The node is already in the rbtree, we break */
       if(c != NULL)
           break;
   
       parent(node) = p;
       rb_make_red_m(color(node));
   
       /* Smaller on the left, bigger on the right */
       if(r > 0) {
           RB_A(left(p) == NULL);
           left(p) = node;
       } else {
           RB_A(right(p) == NULL);
           right(p) = node;
       }
       _rb_insert_fix_m(
               type,
               color,
               parent,
               left,
               right,
               tree,
               node
       );
   } while(0);
   #enddef
   
   #begindef rb_insert_tr_m(
           type,
           color,
           parent,
           left,
           right,
           cmp,
           tree,
           node
   )
   {
       type* __rb_current_;
       type* __rb_parent_;
       int   __rb_result_;
       _rb_insert_tr_m(
           type,
           color,
           parent,
           left,
           right,
           cmp,
           tree,
           node,
           __rb_current_,
           __rb_parent_,
           __rb_result_
       )
   }
   #enddef
   
   #begindef rb_insert_cx_m(cx, tree, node)
       rb_insert_tr_m(
           cx##_type_t,
           cx##_color_m,
           cx##_parent_m,
           cx##_left_m,
           cx##_right_m,
           cx##_cmp_m,
           tree,
           node
       )
   #enddef
   
   #begindef rb_insert_m(cx, tree, node)
       rb_insert_tr_m(
           cx##_type_t,
           rb_color_m,
           rb_parent_m,
           rb_left_m,
           rb_right_m,
           cx##_cmp_m,
           tree,
           node
       )
   #enddef
   
Private
=======

Functions that are used internally.

_rb_rotate_left_tr_m
---------------------

Also: _rb_rotate_right_tr_m

A rotation is a local operation in a search tree that preserves in-order
traversal key ordering. Used to fix insert/deletion discrepancies. This
operation might change the current root.

_rb_rotate_right_tr_m is _rb_rotate_left_tr_m where left and right had been
switched.

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

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

   #begindef __rb_rotate_left_tr_m(
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
       y = right(node);
   
       /* Turn y's left sub-tree into x's right sub-tree */
       right(x) = left(y);
       if(left(y) != NULL)
           parent(left(y)) = x;
       /* y's new parent was x's parent */
       parent(y) = parent(x);
       /* Set the parent to point to y instead of x */
       /* First see whether we're at the root */
       if(parent(x) == NULL) {
           rb_unset_root_m(color(tree));
           rb_make_root_m(color(y));
           tree = y;
       } else {
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
   
   #begindef _rb_rotate_left_tr_m(
           type,
           color,
           parent,
           left,
           right,
           tree,
           node
   )
   {
       type* __rb_x_;
       type* __rb_y_;
       __rb_rotate_left_tr_m(
           color,
           parent,
           left,
           right,
           tree,
           node,
           __rb_x_,
           __rb_y_
       );
   }
   #enddef
   
   #begindef _rb_rotate_left_m(cx, tree, node)
       _rb_rotate_left_tr_m(
           cx##_type_t,
           rb_color_m,
           rb_parent_m,
           rb_left_m,
           rb_right_m,
           tree,
           node
       )
   #enddef
   
   #begindef _rb_rotate_right_tr_m(
           type,
           color,
           parent,
           left,
           right,
           tree,
           node
   )
       _rb_rotate_left_tr_m(
           type,
           color,
           parent,
           right, /* Switched */
           left,  /* Switched */
           tree,
           node
       )
   #enddef
   
   #begindef _rb_rotate_right_m(cx, tree, node)
       _rb_rotate_right_tr_m(
           cx##_type_t,
           rb_color_m,
           rb_parent_m,
           rb_left_m,
           rb_right_m,
           tree,
           node
       )
   #enddef
   
_rb_insert_fix_tr_m
---------------------

After insert new node is labelled red, and possibly destroys the red-black
property. The main loop moves up the tree, restoring the red-black property.

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

node
   The node to initialize.

.. code-block:: cpp

   #begindef __rb_insert_fix_m(
           type,
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
           if(parent(x) == parent(parent(left(x)))) {
               _rb_insert_fix_node_m(
                   type,
                   color,
                   parent,
                   left,
                   right,
                   tree,
                   x,
                   y
               );
           } else {
               _rb_insert_fix_node_m(
                   type,
                   color,
                   parent,
                   right, /* Switched */
                   left, /* Switched */
                   tree,
                   x,
                   y
               );
           }
       }
   }
   #enddef
   
   #begindef _rb_insert_fix_m(
           type,
           color,
           parent,
           left,
           right,
           tree,
           node
   )
   {
       type* __rb_x;
       type* __rb_y_;
       __rb_insert_fix_m(
           type,
           color,
           parent,
           left,
           right,
           tree,
           node,
           __rb_x,
           __rb_y_
       );
   }
   #enddef
   
   #begindef _rb_insert_fix_node_m(
           type,
           color,
           parent,
           left,
           right,
           tree,
           x,
           y
   )
   {
       /* If x's parent is a left, y is x's right 'uncle' */
       y = parent(parent(right(x)));
       if(rb_is_red_m(color(y))) {
           /* case 1 - change the colors */
           rb_make_black_m(color(parent(x)));
           rb_make_black_m(color(y));
           rb_make_red_m(color(parent(parent(x))));
           /* Move x up the tree */
           x = parent(parent(x));
       } else {
           /* y is a black node */
           if(x == right(parent(x))) {
               /* and x is to the right
                * case 2 - move x up and rotate */
               x = parent(x);
               _rb_rotate_left_tr_m(
                   type,
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
           _rb_rotate_right_tr_m(
               type,
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
