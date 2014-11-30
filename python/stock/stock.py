#!/usr/bin/python
# coding: UTF-8

import urllib

def get_price(code):
        url = "http://hq.sinajs.cn/?list=%s" % code
        opener = urllib.FancyURLopener()
        html = opener.open(url).read().decode('gbk')
        data = html.split('"')[1].split(',')
        name = "%-6s" %data[0]
        price_current = "%-6s" % float(data[3])
        change_percent = (float(data[3]) - float(data[2])) * 100 / float(data[2])
        change_percent = "%-6s" % round (change_percent, 2)
        print(u"股票名称:{0} 涨跌幅:{1}% 最新价:{2}元".format(name, change_percent, price_current))

def get_all_price(code_list):
    for code in code_list:
        get_price(code)

code_list = ['sh600219', 'sz000623', 'sh600362', 'sz000725', 'sh600298', 'sh600881', 'sz000002', 'sz000999', 'sh600600',
             'sh600739', 'sh600660', 'sz002582', 'sh600166', 'sh600750', 'sh601088', 'sz002372', 'sz000338', 'sz000895',
             'sz000792']
get_all_price(code_list)
