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

code_list = ['sh600219',           ##南山铝业
             'sz000623',           ##吉林敖东
             'sz000725',           ##京东方Ａ
             'sz000002',           ##万  科Ａ
             'sh600036',           ##招商银行
             'sh601166',           ##兴业银行
             'sh600298',           ##安琪酵母
             'sh600881',           ##亚泰集团
             'sz002582',           ##好想你  
             'sh600750',           ##江中药业
             'sh601088',           ##中国神华
             'sz000338',           ##潍柴动力
             'sz000895',           ##双汇发展
             'sz000792']           ##盐湖股份
get_all_price(code_list)
