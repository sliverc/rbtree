"""Test if we can delete from the tree."""
from build._rbtree_tests import lib
from hypothesis import given, assume
import hypothesis.strategies as st
from test_all import call_ffi


def deduplicate(seq):
    """Deduplicate a list."""
    seen = set()
    seen_add = seen.add
    return [x for x in seq if not (x in seen or seen_add(x))]


@given(st.lists(
    st.integers(
        min_value=-2**30,
        max_value=(2**30) - 1
    ),
    min_size=2
))
def test_delete(ints):
    """Test if rbtree is consistent after generated inserts and one deleted."""
    ints = deduplicate(ints)
    c = len(ints)
    assume(c > 1)
    dl = ints[:-1]
    s = sum(dl)
    ss = sorted(dl)
    do_sum = True
    if abs(s) > (2**32 / 4) - 1:
        do_sum = False
        s = 0
    call_ffi(lib.test_delete, c, ints, ss, c - 1, s, do_sum)
