"""Test if the tree stays consistent."""
from build._rbtree_tests import lib, ffi
from hypothesis.stateful import GenericStateMachine
from hypothesis.strategies import tuples, sampled_from, just, integers


class Node(object):
    """Represent a node in the tree."""

    def __init__(self, node, value):
        node.value = value
        self.node = node

    def __eq__(self, other):
        """Eq comparison."""
        return self.node.value == other.node.value

    def __lt__(self, other):
        """Lt comparison."""
        return self.node.value < other.node.value

    def __hash__(self):
        """Hash function."""
        return self.node.value


class GenTree(GenericStateMachine):
    """Test if the stays consistent."""

    def __init__(self):
        self.comparison = set()
        lib.test_init()

    def steps(self):
        add_strategy = tuples(
            just("add"),
            integers(
                min_value=-2**30,
                max_value=(2**30) - 1
            )
        )
        check_strategy = tuples(just("check"), just(None))
        if not self.comparison:
            return add_strategy | check_strategy
        else:
            return (
                add_strategy | check_strategy |
                tuples(just("delete"), sampled_from(sorted(self.comparison))))

    def execute_step(self, step):
        action, value = step
        if action == 'delete':
            lib.test_remove(value.node)
            self.comparison.remove(value)
            assert value not in self.comparison
            value.node = None
        elif action == 'add':
            node = Node(ffi.new("node_t*"), value)
            lib.test_add(node.node)
            self.comparison.add(node)
            assert node in self.comparison
        else:
            assert value is None
            assert action == 'check'


TestTree = GenTree.TestCase
