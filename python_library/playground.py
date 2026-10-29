
super = 3

def fun1():
    print("Hello")
    global super
    super = 4

from python_library.debug import debug
debug_active_flag = True

debug(fun1)
fun1()


