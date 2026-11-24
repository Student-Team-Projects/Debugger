
import os
import sys
import time
import struct
import builtins
import functools


# Configuration

HEADER_KEY = 4278057967

TYPE_OUTPUT = 0

STREAM_STDOUT = 1
STREAM_STDERR = 2

_HEADER_FMT = "@IBiiqI0q"

debug_active_flag = True


# Virtual PID management

parent_id = 0
actual_id = 0
new_id = 4000

parent_map = {0: 0}


def update_id():
    global new_id, parent_id, actual_id

    parent_id = actual_id
    actual_id = new_id

    parent_map[actual_id] = parent_id
    new_id += 1


def decrease_id():
    global parent_id, actual_id

    actual_id = parent_id
    parent_id = parent_map[parent_id]


def get_id():
    return actual_id


def get_parent_id():
    return parent_id


# Output interception

_original_stdout = sys.stdout
_original_stderr = sys.stderr

_original_stdout_buffer = sys.stdout.buffer
_original_stderr_buffer = sys.stderr.buffer

_original_os_write = os.write
_original_print = builtins.print


def emit(payload, stream):
    if stream == STREAM_STDOUT:
        original = _original_stdout
        buffer = _original_stdout_buffer
    elif stream == STREAM_STDERR:
        original = _original_stderr
        buffer = _original_stderr_buffer
    else:
        raise ValueError("Invalid stream")

    original.flush()

    header = struct.pack(
        _HEADER_FMT,
        HEADER_KEY,
        TYPE_OUTPUT,
        get_parent_id(),
        get_id(),
        time.time_ns() // 1_000_000,
        len(payload),
        )

    buffer.write(header + payload)
    buffer.flush()


class BinaryOutput:
    def __init__(self, stream):
        self.stream = stream

    def write(self, data):
        if not isinstance(data, (bytes, bytearray, memoryview)):
            raise TypeError("a bytes-like object is required")

        data = bytes(data)

        if data:
            emit(data, self.stream)

        return len(data)

    def writelines(self, lines):
        for line in lines:
            self.write(line)

    def flush(self):
        if self.stream == STREAM_STDOUT:
            _original_stdout_buffer.flush()
        else:
            _original_stderr_buffer.flush()

    def writable(self):
        return True

    def fileno(self):
        if self.stream == STREAM_STDOUT:
            return _original_stdout.fileno()
        return _original_stderr.fileno()


class Output:
    def __init__(self, original, stream):
        self.original = original
        self.stream = stream

        self.encoding = original.encoding
        self.errors = original.errors

        self.buffer = BinaryOutput(stream)

    def write(self, text):
        if not isinstance(text, str):
            raise TypeError("write() argument must be str")

        payload = text.encode(self.encoding, self.errors)

        if payload:
            emit(payload, self.stream)

        return len(text)

    def writelines(self, lines):
        for line in lines:
            self.write(line)

    def flush(self):
        self.buffer.flush()

    def writable(self):
        return True

    def isatty(self):
        return self.original.isatty()

    def fileno(self):
        return self.original.fileno()


_output = Output(_original_stdout, STREAM_STDOUT)
_error = Output(_original_stderr, STREAM_STDERR)


def print_def(*args, sep=" ", end="\n", file=None, flush=False):
    if file is None:
        file = sys.stdout

    if (
            file is sys.stdout
            or file is _output
            or file is _original_stdout
    ):
        output = _output

    elif (
            file is sys.stderr
            or file is _error
            or file is _original_stderr
    ):
        output = _error

    else:
        return _original_print(
            *args,
            sep=sep,
            end=end,
            file=file,
            flush=flush,
        )

    text = sep.join(str(arg) for arg in args) + end
    output.write(text)

    if flush:
        output.flush()


def intercepted_os_write(fd, data):
    if fd == 1:
        stream = STREAM_STDOUT
    elif fd == 2:
        stream = STREAM_STDERR
    else:
        return _original_os_write(fd, data)

    data = bytes(data)

    if data:
        emit(data, stream)

    return len(data)


# Interception lifecycle

_debug_depth = 0


def install_output_interception():
    global _debug_depth

    if _debug_depth == 0:
        sys.stdout = _output
        sys.stderr = _error

        os.write = intercepted_os_write
        builtins.print = print_def

    _debug_depth += 1


def uninstall_output_interception():
    global _debug_depth

    if _debug_depth <= 0:
        raise RuntimeError("Output interception is not installed")

    _debug_depth -= 1

    if _debug_depth == 0:
        _output.flush()
        _error.flush()

        sys.stdout = _original_stdout
        sys.stderr = _original_stderr

        os.write = _original_os_write
        builtins.print = _original_print


# Debug decorator

def debug(fun):
    @functools.wraps(fun)
    def wrapper(*args, **kwargs):
        if not debug_active_flag:
            return fun(*args, **kwargs)

        install_output_interception()

        id_updated = False

        try:
            update_id()
            id_updated = True

            print(fun.__name__)

            return fun(*args, **kwargs)

        finally:
            try:
                if id_updated:
                    decrease_id()
            finally:
                uninstall_output_interception()

    return wrapper
