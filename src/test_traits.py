"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib


def test_traits():
    assert(lib.test_traits() == 0)
