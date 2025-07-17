#!/usr/bin/env python3
# coding: UTF-8

"""This script parse stock info"""
import sys,os,getopt
import urllib
import requests
def is_empty_line(line):
    return len(line.strip()) == 0

def get_price(code):
    '''print one stock info in details'''
    headers = {'referer': 'http://finance.sina.com.cn'}
    resp = requests.get('http://hq.sinajs.cn/list=' + code, headers=headers, timeout=6)
    for line in resp.text.split('\n'):
        ##print(line)
        if (is_empty_line(line)):
            break
        data = line.split('"')[1].split(',')
        name = "%-20s" % data[0]
        price_current = "%-6s" % float(data[3])
        change_percent = (float(data[3]) - float(data[2])) * 100 / float(data[2])
        change_percent = "%-6s" % round (change_percent, 2)
        print(u"股票名称:{0} 涨跌幅:{1}% 最新价:{2}元".format(name,
            change_percent, price_current))

def get_all_price(code_list):
    '''process all stock'''
    all_code=""
    for code in code_list:
        all_code = all_code + code + ","
    if all_code[-1] == ",":
        all_code = all_code[:-1]
    
    get_price(all_code)

ZHISHU = ['sz159532',
            'sh512100',
            'sh000001',
            'sh000688',
            'sz399006',
            'sh000300']

ZHUANZHAI = ['sh113668', 'sh603051',
             'sh111014', 'sh605337',
             'sz127068', 'sz002996',
             'sz123087', 'sz300739',
             'sz123196', 'sz300645', 
             'sh113569', 'sh603660',
             'sh111007', 'sh605020',
             'sh111016', 'sh605228',
             'sz123100', 'sz300543',
             'sh113593', 'sh603131',
             'sz123190', 'sz300409',
             'sh113678', 'sh603220',
             'sz123131', 'sz300738',
             'sh111012', 'sh605488',
             'sz123205', 'sz300879',
             'sz127051', 'sz002975',
             'sz123162', 'sz300486',
             'sz123232', 'sz300830', 
             'sz127065', 'sz002997',
             'sz123160', 'sz300992',
             'sz123204', 'sz300829',
             'sz123219', 'sz300790',
             'sz123187', 'sz301186',
             'sz123168', 'sz300891',
             'sz123170', 'sz300346',
             'sh113524', 'sh603677',
             'sz123085', 'sz300057',
             'sz123127', 'sz300818',
             'sz127084', 'sz000528',
             'sh110077', 'sh600461',
             'sh110091', 'sh600761']

STOCK = ['sh603198', 'sh510300', 'sh600489', 'sh601899', 'sz000426', 'sh601225', 'sh600023', 'sh600674' ]

def usage():
    print('''
    Usage: stock.py [options...]
    Options:
    -z : zhishu
    -Z : zhuangzhai
    -s : stock
    ''')
 
try:
    opts, args = getopt.getopt(sys.argv[1:],'hzZe:c:f:dn:s')
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
    elif opt == '-z':
        get_all_price(ZHISHU)
    elif opt == '-Z':
        get_all_price(ZHUANZHAI)
    elif opt == '-s':
        get_all_price(STOCK)

sys.exit()
