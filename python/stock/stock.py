#!/usr/bin/python
# coding: UTF-8

"""This script parse stock info"""

import urllib.request
import urllib.error

def get_price(code):
    '''print one stock info in details'''
    url = "http://hq.sinajs.cn/?list=%s" % code
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        "Accept-Language": "zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2",
        "Connection": "keep-alive",
        "Referer": "http://finance.sina.com.cn/"
    }
    request = urllib.request.Request(url, headers=headers)
    html = urllib.request.urlopen(request).read().decode('gbk')
    data = html.split('"')[1].split(',')
    name = "%-6s" % data[0]
    price_current = "%-6s" % float(data[3])
    change_percent = (float(data[3]) - float(data[2])) * 100 / float(data[2])
    change_percent = "%-6s" % round (change_percent, 2)
    print(u"股票名称:{0} 涨跌幅:{1}% 最新价:{2}元".format(name,
        change_percent, price_current))

def get_all_price(code_list):
    '''process all stock'''
    for code in code_list:
        get_price(code)

if __name__ == '__main__':
    STOCK = ['sh512050', 'sh512890', 'sh515450', 'sz200429', 'sz200019']
    get_all_price(STOCK)
