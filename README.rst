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

      my_node_init(node);

   There is also a short if you know your are going to use all standard
   fields in your struct (color, parent, left right)

   .. code-block:: cpp

      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
      rb_bind_m(my, node_t)

   .. code-block:: cpp

      my_node_init(node);

   Of course usually you want to split declaration and implementation of the
   function so it is. example.h:

   .. code-block:: cpp

      #define my_cmp_m(x, y) rb_value_cmp_m(x, y)
      rb_bind_decl_m(my, node_t)

   And example.c:

   .. code-block:: cpp

      rb_bind_impl_m(my, node_t)

      int main(void) { my_node_init(node); return 0; }

rb_x_m
   These functions are macros and take a type and traits as standard
   arguments and are the most verbose. Used to build upon rbtree. For
   example prbtree (persistent rbtree) will use these function.

   .. code-block:: cpp

      rb_node_init_m(
          node_t,
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


Implementation
==============

Based on the following references: auckland1_, auckland2_, sglib_

.. _auckland1: https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
.. _auckland2: https://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.txt
.. _sglib: http://sglib.sourceforge.net/doc/index.html#rbtree_api1

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
       typedef type cx##_iter_t;
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

   #define RB_WHITE  0
   #define RB_BLACK (1 << 0)
   #define RB_ROOT  (1 << 1)
   #define RB_COPY  (1 << 2) /* Used in future for persistent rbtrees */
   
   #define rb_is_white_m(x)   (x == RB_WHITE)
   #define rb_is_red_m(x)   (!(x & RB_BLACK))
   #define rb_is_black_m(x)   (x & RB_BLACK)
   #ifdef NDEBUG
   #   define rb_is_root_m(x)  1
   #else
   #   define rb_is_root_m(x) (x & RB_ROOT) /* Special black :-p */
   #endif
   #define rb_needs_copy_m(x) (x & RB_COPY
   
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

rb_node_init_m
--------------

Bound: cx##_node_init

Initializes a node by setting the color to RB_WHITE and all pointers to
NULL.

node
   The node to initialize.

.. code-block:: cpp

   #begindef rb_node_init_m(
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

   #begindef rb_iter_init_m(left, tree, elem)
   {
       if(tree == NULL)
           elem = NULL;
       else {
           elem = tree;
           while(left(elem) != NULL)
               elem = left(elem);
       }
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
       parent,
       left,
       right,
       elem,
       tmp
   )
   do {
       tmp = right(elem);
       if(tmp != NULL) {
           elem = tmp;
           while(left(elem) != NULL)
               elem = left(elem);
           break;
       }
       for(;;) {
           /* Next would be the root, we are done */
           if(parent(elem) == NULL) {
               elem = NULL;
               break;
           }
           tmp = parent(elem);
           /* Are a left node, therefor we are the next node */
           if(elem == left(tmp)) {
               elem = tmp;
               break;
           }
           elem = tmp;
       }
   } while(0)
   #enddef
   
   #begindef rb_iter_next_m(
       type,
       parent,
       left,
       right,
       elem
   )
   {
       type* __rb_next_tmp_;
       _rb_iter_next_m(
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
(tree). If an equal node exists in the tree node will note added an will
still be RB_WHITE.

cmp
   Comparator (rb_pointer_cmp_m or rb_value_cmp_m could be used)

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

node
   The node to initialize.

.. code-block:: cpp

   #begindef _rb_insert_m(
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
           break;
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
           c = r > 0 ? left(c) : right(c);
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
       /* print_tree(0, tree, NULL); */
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
   
   #begindef rb_insert_m(
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
       _rb_insert_m(
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
       int
       cx##_check_tree(type* tree);
       int
       cx##_check_tree_rec(
               type* tree,
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
       void
       cx##_iter_init(
               type* tree,
               cx##_iter_t* iter,
               type** elem
       )
       {
           (void)(iter);
           rb_iter_init_m(left, tree, *elem);
       }
       void
       cx##_iter_next(
               cx##_iter_t* iter,
               type** elem
       )
       {
           (void)(iter);
           rb_iter_next_m(
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
               color,
               parent,
               left,
               right,
               cmp,
               *tree,
               node
           );
           return rb_is_white_m(color(node));
       }
       int
       cx##_check_tree(type* tree)
       {
           int pathdepth = 0;
           return cx##_check_tree_rec(tree, 0, &pathdepth);
       }
       int
       cx##_check_tree_rec(
               type* tree,
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
           tree,
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

tree
   The root node of the tree. Pointer to NULL represents an empty tree.

result
   Zero on success, other on failure

.. code-block:: cpp

   #begindef rb_check_tree_m(
           cx,
           type,
           color,
           parent,
           left,
           right,
           cmp,
           tree,
           depth,
           pathdepth
   )
   {
       (void)(tree);
       (void)(depth);
       (void)(pathdepth);
       return 0;
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

   #begindef __rb_rotate_left_m(
           color,
           parent,
           left,
           right,
           tree,
           node,
           x,
           y
   )
   do {
       x = node;
       y = right(node);
       /* No need to rotate */
       if(y == NULL)
           break;
   
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
   } while(0)
   #enddef
   
   #begindef _rb_rotate_left_m(
           type,
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
           color,
           parent,
           left,
           right,
           tree,
           node
   )
       _rb_rotate_left_m(
           type,
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
           if(parent(x) == left(parent(parent(x)))) {
               _rb_insert_fix_node_m(
                   type,
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
                   color,
                   parent,
                   right, /* Switched */
                   left, /* Switched */
                   _rb_rotate_right_m, /* Switched */
                   _rb_rotate_left_m, /* Switched */
                   tree,
                   x,
                   y
               );
           }
       }
       /* TODO Move this back to insert if tests are ok */
       rb_make_root_m(color(tree));
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
       type* __rb_insf_x_;
       type* __rb_insf_y_;
       __rb_insert_fix_m(
           type,
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
       /* If x's parent is a left, y is x's right 'uncle' */
       y = right(parent(parent(x)));
       /* Null means the node is black by spec */
       if(y != NULL && rb_is_red_m(color(y))) {
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
               rot_left(
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
           if(parent(parent(x)) != NULL) {
               rb_make_red_m(color(parent(parent(x))));
               rot_right(
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
   }
   #enddef
