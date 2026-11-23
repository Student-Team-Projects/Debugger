
super = 3

def fun1():
    print("Hello")
    global super
    super = 4

from python_library.debug import debug
debug_active_flag = True

debug(fun1)()
fun1()

def fun2():
    print("Before fun2")
    debug(fun1)()
    print("After fun2")

def fun3(a):
    print("I get:", a)

fun2()
debug(fun2)()

fun3(2)
debug(fun3)(3)


