"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib
from hypothesis import given
import hypothesis.strategies as st


@given(st.lists(
    st.integers(
        min_value=-2**32 / 2,
        max_value=(2**32 / 2) - 1
    ),
    max_size=3
))
def test_insert(ints):
    """Test if rbtree is consistent after generated inserts."""
    s = sum(ints)
    if abs(s) > (2**32 / 2) - 1:
        s = 0
    c = len(ints)
    print(ints)
    lib.test_insert(c, ints, s)


def test_insert_fix():
    """Test if rbtree is consistent after generated inserts."""
    ints = [1, 2, 3]
    s = sum(ints)
    c = len(ints)
    assert(lib.test_insert(c, ints, s) == 0)


def test_insert_static():
    """Test if rbtree is consistent after inserts."""
    assert(lib.test_insert_static() == 0)


def test_rotate():
    """Test if rotate left/right work."""
    assert(lib.test_rotate() == 0)
