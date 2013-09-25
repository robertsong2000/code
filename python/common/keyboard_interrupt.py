from time import sleep

def do_some_func():
    sleep(10)

try:
    do_some_func()
except KeyboardInterrupt:
    print "User Press Ctrl+C,Exit"
except EOFError:
    print "User Press Ctrl+D,Exit"
