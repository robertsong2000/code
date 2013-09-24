#!/usr/bin/env python

track_file = "test.txt"
fd = open(track_file)
content_list = fd.readlines()
fd.close()

for line in content_list:
    print line  

fd = open(track_file)
fd.seek(0)
title = fd.readline()
keyword = fd.readline()
uuid = fd.readline()
print title, keyword, uuid
fd.close()  
