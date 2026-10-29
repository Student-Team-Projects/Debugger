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



def print_def(*args, sep=" ", end="\n", **kwargs):
    text = sep.join(str(a) for a in args) + end
    payload = text.encode()

    header = struct.pack(
        _HEADER_FMT,
        HEADER_KEY,
        TYPE_PRINT,
        0,
        417,
        time.time_ns() // 1_000_000,      # or whatever unit your C++ side uses
        len(payload),
    )

    sys.stdout.flush()
    sys.stdout.buffer.write(header + payload)
    sys.stdout.buffer.flush()


def debug(fun):
    global debug_active_flag
    if not debug_active_flag:
        fun()
        return
    global print
    _orig_print = builtins.print
    builtins.print = print_def
    try:
        print(fun.__name__)
        fun()
    finally:
        builtins.print = _orig_print