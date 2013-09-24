a = 'server=mpilgrim;uid=sa;database=master;pwd=secret'
aa = {}
for i in a.split(';'):
    aa[i.split('=',1)[0]] = i.split('=',1)[1]

print aa
