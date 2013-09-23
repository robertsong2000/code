class f1 ():
    def __init__(self, name):
        self.name = name
        print "f1 __init__"
    def write(self, offset):
        print "f1 write"
    def read(self, offset):
        print "f1 read"

class f2 ():
    def __init__(self, name):
        self.name = name
        print "f2 __init__"
    def write(self, offset):
        print "f2 write"
    def read(self, offset):
        print "f2 read"

class s1 (f1, f2):
    def __init__(self, name):
        f1.__init__(self, name)
        f2.__init__(self, name)
    def write(self, offset):
        f1.write(self, offset)
        f2.write(self, offset)

if __name__=='__main__':
    s = s1("hello")
    s.write(1)
    s.read(1)

