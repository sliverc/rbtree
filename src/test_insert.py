"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib


def test_insert_static():
    assert(lib.test_insert_static() == 0)
