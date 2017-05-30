"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib
from hypothesis import given
import hypothesis.strategies as st


def deduplicate(seq):
    """Deduplicate a list."""
    seen = set()
    seen_add = seen.add
    return [x for x in seq if not (x in seen or seen_add(x))]


@given(st.lists(
    st.integers(
        min_value=-(2 ** 30),
        max_value=2 ** 30
    )
))
def test_insert(ints):
    """Test if rbtree is consistent after generated inserts."""
    ss = sorted(deduplicate(ints))
    sc = len(ss)
    s = sum(ss)
    c = len(ints)
    do_sum = True
    if abs(s) > (2**32 / 2) - 1:
        do_sum = False
        s = 0
    assert(lib.test_insert(c, ints, ss, sc, s, do_sum) == 0)


def test_insert_fix():
    """Test if rbtree is consistent after generated inserts."""
    ints = [-1, 0, 2, 4, 1, 0]
    ss = sorted(deduplicate(ints))
    sc = len(ss)
    s = sum(ss)
    c = len(ints)
    assert(lib.test_insert(c, ints, ss, sc, s, True) == 0)


def test_insert_static():
    """Test if rbtree is consistent after inserts."""
    assert(lib.test_insert_static() == 0)


def test_rotate():
    """Test if rotate left/right work."""
    assert(lib.test_rotate() == 0)
