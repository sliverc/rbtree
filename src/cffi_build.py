"""Defines binding for rbtree tests."""
from cffi import FFI
import os
import os.path as path
import glob
import codecs

_build = os.environ["BUILD"]
_base = os.environ["BASE"]
os.environ["CFLAGS"] = " ".join([
    c for c in os.environ["CFLAGS"].split(" ")
    if c not in ("-pedantic",)
])

_headers = glob.glob(path.join(
    _base,
    "src",
    "test_*.h"
))

_includes = "\n".join([
    '#include "%s"' % path.basename(f)
    for f in _headers
])

_defs = []
for h in sorted(_headers):
    with codecs.open(h, encoding="UTF-8") as f:
        _defs.append(f.read())

_defs = "\n".join(_defs)

ffi = FFI()
ffi.set_source(
    "_rbtree_tests",
    _includes,
    libraries=["m"],
    extra_link_args=["rbtests.a"],
)
ffi.cdef(_defs)

if __name__ == "__main__":
    ffi.compile()
