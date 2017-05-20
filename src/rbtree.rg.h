// ==============
// Red-Black Tree
// ==============
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
// Requirements
// ------------
//
// * clang-format
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
// Implementation
// ==============
//
// Based on the following references: auckland_
//
// .. _auckland: https://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.txt
//
// Basic traits
// ============
//
// Traits used by default (x_tr_m macros)
//
// .. code-block:: cpp
//
#define rb_left_m(x) (x)->left
#define rb_right_m(x) (x)->right
#define rb_parent_m(x) (x)->parent

// API
// ===
//
// Functions that are part of the API.
//
// .. code-block:: cpp
//
#begindef rb_node_init_m(left, right, parent, node)
{
    left(node) = NULL;
    right(node) = NULL;
    parent(node) = NULL;
}
#enddef

#begindef rb_node_init_tr_m(node)
    rb_node_init_m(rb_left_m, rb_right_m, rb_parent_m, node)
#enddef

// Private
// =======
//
// Functions that are used internally.
//
