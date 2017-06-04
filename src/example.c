#include "example.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

rb_bind_impl_m(bk, book_t)

book_t* tree;

void
register_book(char isbn[14], char* title, char* author)
{
    book_t* book = malloc(sizeof(book_t));
    bk_node_init(book);
    book->title  = title;
    book->author = author;
    memcpy(book->isbn, isbn, 14);
    bk_insert(&tree, book);
}

void
lookup_book(char isbn[14])
{
    book_t* book;
    book_t key;
    memcpy(key.isbn, isbn, 14);
    bk_find(tree, &key, &book);
    printf(
        "ISBN:   %s\nTitle:  %s\nAuthor: %s\n\n",
        book->isbn,
        book->title,
        book->author
    );
}

void
remove_book(book_t* book)
{
    printf("Removing %s\n", book->isbn);
    bk_delete_node(&tree, book);
    free(book);
}

int
main(void)
{
    bk_tree_init(&tree);
    register_book(
        "9780441182008",
        "Taltos",
        "Steven Brust"
    );
    register_book(
        "9780060147891",
        "Doorways in the Sand",
        "Roger Zelazny"
    );
    register_book(
        "9780060855901",
        "Equal Rites",
        "Terry Pratchett"
    );
    assert(bk_size(tree) == 3);
    lookup_book("9780060147891");
    lookup_book("9780060855901");
    lookup_book("9780441182008");
    rb_iter_decl_cx_m(bk, bk_iter, bk_elem);
    printf("Catalog:\n\n");
    /* The red-black tree may not be modified during iteration. */
    rb_for_m(bk, tree, bk_iter, bk_elem) {
        printf("%s\n", bk_elem->isbn);
    }
    printf("\nRemoving:\n\n");
    /* But we can remove the root till the tree is empty */
    while(tree != bk_nil_ptr) {
        remove_book(tree);
    }
    return 0;
}
