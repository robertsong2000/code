a = [('2011-03-17', '2.26', 6429600, '0.0'), ('2011-03-16', '2.26', 12036900, '-3.0'),
 ('2011-03-15', '2.33', 15615500,'-19.1')]

print a[0][0]

b = sorted(a, key=lambda result: result[1],reverse=True)
print b

c = sorted(a, key=lambda result: result[2],reverse=True)
print c
