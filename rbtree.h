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
// Usage
// =====
//
// Short
// -----
//
// Detailed
// --------
//
// Every function comes in three flavors
//
// x_m
//    These functions only take a context as standard argument, but they assume
//    you use the default traits (rb_color, rb_parent_m, rb_left_m, rb_right_m).
//    Therefore you only have to define the type.
//
//    .. code-block:: cpp
//
//       rb_new_context_m(my, node_t)
//
//    .. code-block:: cpp
//
//       rb_node_init_tr_m(my, node);
//
//
// x_cx_m
//    These functions only take a context as standard argument. Type and traits
//    are mapped to the context. You have to define the type and the traits for
//    the context.
//
//    .. code-block:: cpp
//
//       rb_new_context_m(my, node_t)
//       #define my_color_m(x) (x)->color
//       #define my_parent_m(x) (x)->parent
//       #define my_left_m(x) (x)->left
//       #define my_right_m(x) (x)->right
//
//    .. code-block:: cpp
//
//       rb_node_init_cx_m(my, node);
//
// x_tr_m
//    These functions take a type and traits as standard arguments and are the
//    most verbose.
//
//    .. code-block:: cpp
//
//       rb_node_init_m(
//           node_t,
//           rb_color_m,
//           rb_parent_m,
//           rb_left_m,
//           rb_right_m,
//           node
//       );
//
//
// Usually you would the x_m functions and add color, parent, left and right to
// the structure used. If you want to use different fields you need to use
// x_cx_m. The x_tr_m function can usually just be ignored and are only
// interesting if you are extending the functionality of rbtree.
//
// Implementation
// ==============
//
// Based on the following references: auckland_
//
// .. _auckland:
// https://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.txt
//
// Basic traits
// ============
//
// Traits used by default (x_tr_m macros)
//
// .. code-block:: cpp
//
#define rb_color_m(x) (x)->color
#define rb_parent_m(x) (x)->parent
#define rb_left_m(x) (x)->left
#define rb_right_m(x) (x)->right

// API
// ===
//
// Functions that are part of the API.
//
// .. code-block:: cpp
//
#define rb_node_init_tr_m(type, color, parent, left, right, node)              \
    {                                                                          \
        color(node)  = 0;                                                      \
        parent(node) = NULL;                                                   \
        left(node)   = NULL;                                                   \
        right(node)  = NULL;                                                   \
    }

#define rb_node_init_cx_m(cx, node)                                            \
    rb_node_init_tr_m(                                                         \
        void, cx##_color_m, cx##_parent_m, cx##_left_m, cx##_right_m, node)

#define rb_node_init_m(cx, node) rb_node_init_cx_m(rb, node)

// Context helpers
// ===============
//
// Functions that help setting up contexts.
//
// .. code-block:: cpp
//
#define rb_new_context_m(cx, type) typedef type cx##_type_m;

// Private
// =======
//
// Functions that are used internally.
//
