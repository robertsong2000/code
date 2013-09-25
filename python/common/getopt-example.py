#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys,os,getopt

def usage():
    print '''
    Usage: analyse_stock.py [options...]
    Options:
    -e : Exchange Name
    -c : User-Defined Category Name
    -f : Read stock info from file and save to db
    -d : delete from db by stock code
    -n : stock name
    -s : stock code
    -h : this help info
    test.py -s haha -n "HA Ha"
    '''
 
try:
    opts, args = getopt.getopt(sys.argv[1:],'he:c:f:d:n:s:')
except getopt.GetoptError:
    usage()
    sys.exit()

if len(opts) == 0:
    usage()
    sys.exit()  
 
for opt, arg in opts:
    if opt in ('-h', '--help'):
      usage()
      sys.exit()
    elif opt == '-d':
      print "del stock %s" % arg
    elif opt == '-f':
      print "read file %s" % arg
    elif opt == '-c':
      print "user-defined %s " % arg
    elif opt == '-e':
      print "Exchange Name %s" % arg
    elif opt == '-s':
      print "Stock code %s" % arg
    elif opt == '-n':
      print "Stock name %s" % arg  
 
sys.exit()
