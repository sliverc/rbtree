"""Helpers for all tests."""
import os


def call_ffi(fn, *args, **kwargs):
    """Fork, reset the error call a test and raise if the error was set."""
    pid = os.fork()
    if pid == 0:
        ret = fn(*args, **kwargs)
        os._exit(ret)
    else:
        ret = os.waitpid(pid, 0)
        assert ret[1] == 0


def except_ffi(value, fn, *args, **kwargs):
    """Fork, reset the error call a test and raise if the error was set."""
    pid = os.fork()
    if pid == 0:
        ret = fn(*args, **kwargs)
        os._exit(ret)
    else:
        ret = os.waitpid(pid, 0)
        assert ret[1] == value
