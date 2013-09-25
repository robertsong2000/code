str="abcdefg"
print "%.3s" % str
str="abcdefg"
print "%10s" % str
str="abcdefg"
print "%10.3s" % str
import fpformat
a= 0.0030000000005
b=fpformat.fix(a,6)
print b
from decimal import *
a ="2.26"
b ="2.29"
c = Decimal(a) - Decimal(b)
print c
