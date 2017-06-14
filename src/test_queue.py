"""Test if the queue stays consistent."""
from build._rbtree_tests import lib, ffi
from hypothesis import settings
from hypothesis.stateful import GenericStateMachine
from hypothesis.strategies import tuples, just, integers


class Item(object):
    """Represent a item in the queue."""

    def __init__(self, item, value):
        item.value = value
        self.item = item


def test_queuex():
    """Test if queue works as expected."""
    lib.test_queue_init()
    queue = []
    for x in [1, 2, 3, 4]:
        item = Item(ffi.new("item_t*"), x)
        queue.insert(0, item)
        assert lib.test_enqueue(item.item) == 0
    assert lib.test_dequeue(1) == 0
    assert lib.test_queue_iter([2, 3, 4]) == 0


class GenQueue(GenericStateMachine):
    """Test if the queue stays consistent."""

    def __init__(self):
        self.comparison = []
        lib.test_queue_init()

    def steps(self):
        enqueue_item = tuples(
            just("enqueue"),
            integers(
                min_value=-2**30,
                max_value=(2**30) - 1
            )
        )
        dequeue_item = tuples(just(
            "dequeue"
        ), just(None))
        if not self.comparison:
            return enqueue_item
        else:
            return (
                enqueue_item | dequeue_item
            )

    def execute_step(self, step):
        action, value = step
        if action == 'enqueue':
            item = Item(ffi.new("item_t*"), value)
            self.comparison.insert(0, item)
            assert lib.test_enqueue(item.item) == 0
            lib.test_queue_iter([
                item.item.value for item in self.comparison
            ])
        elif action == 'dequeue':
            item = self.comparison.pop()
            assert lib.test_dequeue(item.item.value) == 0
            assert item.item.next == ffi.NULL
            if not self.comparison:
                assert lib.test_queue_null() == 0
        else:
            assert False


with settings(max_examples=2000):
    TestQueue = GenQueue.TestCase
