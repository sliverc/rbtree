"""Test if we can set traits (yes it is a stupid test)."""
from build._rbtree_tests import lib
from hypothesis import given, assume
import hypothesis.strategies as st


def deduplicate(seq):
    """Deduplicate a list."""
    seen = set()
    seen_add = seen.add
    return [x for x in seq if not (x in seen or seen_add(x))]


@given(st.lists(
    st.integers(
        min_value=-2**32 / 2,
        max_value=(2**32 / 2) - 1
    ),
    min_size=2
))
def test_delete(ints):
    """Test if rbtree is consistent after generated inserts and one deleted."""
    ints = deduplicate(ints)
    c = len(ints)
    assume(c > 1)
    s = sum(ints[:-1])
    do_sum = True
    if abs(s) > (2**32 / 2) - 1:
        do_sum = False
        s = 0
    assert(lib.test_delete(c, ints, c - 1, s, do_sum) == 0)


@given(st.lists(
    st.integers(
        min_value=-2**32 / 2,
        max_value=(2**32 / 2) - 1
    ),
    min_size=2
))
def test_switch(ints):
    """Test if rbtree is consistent after two node were switched."""
    ints = deduplicate(ints)
    s = sum(ints)
    c = len(ints)
    assume(c > 1)
    do_sum = True
    if abs(s) > (2**32 / 2) - 1:
        do_sum = False
        s = 0
    assert(lib.test_switch(c, ints, s, do_sum) == 0)
