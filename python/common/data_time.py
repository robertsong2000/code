import datetime
import time

t1 = time.strptime('2011-01-20 14:05',"%Y-%m-%d %H:%M")
t2 = time.strptime('2011-01-20 16:05',"%Y-%m-%d %H:%M")
print t1 > t2
print  t1 < t2

print datetime.datetime.now().strftime("%Y-%m-%d %H:%M")
print (datetime.datetime.now() - datetime.timedelta(hours=8)).strftime("%Y-%m-%d %H:%M")
endtime=datetime.datetime.strptime('20100701',"%Y%m%d")
print type(endtime)
print endtime

a = 1302153828
print time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(a))
