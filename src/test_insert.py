"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib
from hypothesis import given
import hypothesis.strategies as st


@given(st.sets(
    st.integers(
        min_value=-2**32 / 2,
        max_value=(2**32 / 2) - 1
    )
))
def test_insert(ints):
    """Test if rbtree is consistent after generated inserts."""
    ints = list(ints)
    s = sum(ints)
    do_sum = True
    if abs(s) > (2**32 / 2) - 1:
        do_sum = False
        s = 0
    c = len(ints)
    assert(lib.test_insert(c, ints, s, do_sum) == 0)


def test_insert_fix():
    """Test if rbtree is consistent after generated inserts."""
    ints = [-7, -3, 0, 1]
    s = sum(ints)
    c = len(ints)
    assert(lib.test_insert(c, ints, s, True) == 0)


def test_insert_static():
    """Test if rbtree is consistent after inserts."""
    assert(lib.test_insert_static() == 0)


def test_rotate():
    """Test if rotate left/right work."""
    assert(lib.test_rotate() == 0)
