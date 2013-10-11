#!/usr/bin/env python
from sys import argv, stderr
from getopt import getopt

def usage(name):
    print "Usage: %s [options]" % name
    print "    -p <pid>                specify id"
    print "    -h, --help              display the help and exit"
    print "    --version               display version"
    return

PID_NAME = {
            'swapper':            'Idle',
        }

def get_name_from_pid(pid):
    pid_name=pid
    real_name=PID_NAME.get(pid_name)
    return real_name

class UsageException(Exception):
    pass

args = argv[1:]
VERSION = '1.0'
test_id = None

try:
    while len(args) > 0:
        options, args = getopt(args, 'hp:v', ['help', 'version'])

        for option in options:
            if option[0] in ('-h', '--help'):
                usage(argv[0])
                exit(0)
            elif option[0] == '--version':
                print '%s version is %s' % (argv[0], VERSION)
                exit(0)
            elif option[0] == '-p':
                test_id = option[1]

        if len(args) > 0:
            args = args[1:]

except UsageException:
    usage(argv[0])
    exit(1)

if test_id == None:
    print "pid should be specified"
    usage(argv[0])
    exit(1)

real_name = get_name_from_pid(test_id)
print "the name of pid: %s is %s" %(test_id, real_name)
