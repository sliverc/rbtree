"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib
from test_all import call_ffi


def test_traits():
    call_ffi(lib.test_traits)
