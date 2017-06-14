"""Test if the stack stays consistent."""
from build._rbtree_tests import lib, ffi
from hypothesis import settings
from hypothesis.stateful import GenericStateMachine
from hypothesis.strategies import tuples, just, integers


class Item(object):
    """Represent a item in the stack."""

    def __init__(self, item, value):
        item.value = value
        self.item = item


def test_stackx():
    """Test if stack works as expected."""
    lib.test_stack_init()
    stack = []
    for x in [1, 2, 3, 4]:
        item = Item(ffi.new("item_t*"), x)
        stack.append(item)
        assert lib.test_push(item.item) == 0
    assert lib.test_pop(4) == 0
    assert lib.test_stack_iter([3, 2, 1]) == 0
    assert lib.test_pop(3) == 0


class GenStack(GenericStateMachine):
    """Test if stack the stays consistent."""

    def __init__(self):
        self.comparison = []
        lib.test_stack_init()

    def steps(self):
        push_item = tuples(
            just("push"),
            integers(
                min_value=-2**30,
                max_value=(2**30) - 1
            )
        )
        pop_item = tuples(just(
            "pop"
        ), just(None))
        if not self.comparison:
            return push_item
        else:
            return (
                push_item | pop_item
            )

    def execute_step(self, step):
        action, value = step
        if action == 'push':
            item = Item(ffi.new("item_t*"), value)
            self.comparison.append(item)
            assert lib.test_push(item.item) == 0
            lib.test_stack_iter([
                item.item.value for item in reversed(self.comparison)
            ])
        elif action == 'pop':
            item = self.comparison.pop()
            assert lib.test_pop(item.item.value) == 0
            assert item.item.next == ffi.NULL
            if not self.comparison:
                assert lib.test_stack_null() == 0
        else:
            assert False


# with settings(max_examples=2000):
TestStack = GenStack.TestCase
