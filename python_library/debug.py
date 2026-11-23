import os, sys, time, struct, builtins

HEADER_KEY = 4278057967
TYPE_PRINT = 0       
_HEADER_FMT = "@IBiiqI0q"
debug_active_flag = True


def print_def_stable(*args, sep=" ", end="\n", **kwargs):
    text = sep.join(str(a) for a in args) + end
    payload = text.encode()

    header = struct.pack(
        _HEADER_FMT,
        HEADER_KEY,
        TYPE_PRINT,
        os.getppid(),
        os.getpid(),
        time.time_ns() // 1_000_000,      # or whatever unit your C++ side uses
        len(payload),
    )

    sys.stdout.flush()
    sys.stdout.buffer.write(header + payload)
    sys.stdout.buffer.flush()

# Virtual pid-zone 
parent_id = 0
actual_id = 0
new_id = 4000
parent_map = dict()
parent_map[0] = 0

def update_id():
    global new_id, parent_id, actual_id, parent_map
    parent_id = actual_id
    actual_id = new_id
    parent_map[actual_id] = parent_id
    new_id += 1

def decrease_id():
    global parent_id, actual_id, parent_map 
    actual_id = parent_id
    parent_id = parent_map[parent_id]

def get_id():
    global actual_id
    return actual_id

def get_parent_id():
    global parent_id
    return parent_id   
# Virtual pid-zone - end



def print_def(*args, sep=" ", end="\n", **kwargs):
    text = sep.join(str(a) for a in args) + end
    payload = text.encode()

    header = struct.pack(
        _HEADER_FMT,
        HEADER_KEY,
        TYPE_PRINT,
        get_parent_id(),
        get_id(),
        time.time_ns() // 1_000_000,      # or whatever unit your C++ side uses
        len(payload),
    )

    sys.stdout.flush()
    sys.stdout.buffer.write(header + payload)
    sys.stdout.buffer.flush()

import functools

def debug(fun):
    @functools.wraps(fun)
    def wrapper(*args, **kwargs):
        global debug_active_flag
        if not debug_active_flag:
            fun()
            return
        global print
        _orig_print = builtins.print
        builtins.print = print_def
        try:
            update_id()
            print(fun.__name__)
            ans = fun(*args, **kwargs)
        finally:
            decrease_id()
            builtins.print = _orig_print
        return ans
    return wrapper

def debug_old(fun):
    global debug_active_flag
    if not debug_active_flag:
        fun()
        return
    global print
    _orig_print = builtins.print
    builtins.print = print_def
    try:
        print(fun.__name__)
        ans = fun()
    finally:
        builtins.print = _orig_print
    return ans