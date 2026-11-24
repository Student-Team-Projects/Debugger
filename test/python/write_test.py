import sys, os
from pathlib import Path

project_dir = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(project_dir))

from python_library.debug_new import debug

debug_active_flag = True



@debug
def child():
    print("stdout print")
    print("stderr print", file=sys.stderr)

    sys.stdout.write("stdout write\n")
    sys.stderr.write("stderr write\n")

    sys.stdout.writelines(["stdout line 1\n", "stdout line 2\n"])
    sys.stderr.writelines(["stdout line 1\n", "stdout line 2\n"])

    sys.stdout.buffer.write(b"stdout buffer write\n")
    sys.stderr.buffer.write(b"stderr buffer write\n")

    sys.stdout.buffer.writelines([b"stdout buffer line 1\n", b"stdout buffer line 2\n"])
    sys.stderr.buffer.writelines([b"stdout buffer line 1\n", b"stdout buffer line 2\n"])

    os.write(1, b"stdout os\n")
    os.write(2, b"stderr os\n")

@debug
def parent():
    print("parent started")
    child()
    print("parent finished")


parent()