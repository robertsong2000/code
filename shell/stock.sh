#!/bin/sh
rate="0.0538477535"
#input_file=$1
echo "日本股市市值100名"

function output()
{
    cat /tmp/stock.txt |grep "^[0-9]" | awk '{print $3,$5}' | while read line ; do
        if [ -n "$line" ] ; then
            stock_name=$(echo $line | sed -e 's/ .*//');
            stock_value=$(echo $line | sed -e 's/.* //'| sed -e 's/,//g');
            to_chn=$(echo "$stock_value / 1000 / 1000 / 100 * $rate" | bc -l);
            printf "%-30s\t%.2f亿\n" $stock_name $to_chn ;
        fi
    done
}

function get_file()
{
    w3m -cols 180 -dump $1 > /tmp/stock.txt
}

function show()
{
    get_file $1
    output
}

show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx"
show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=2&Gyosyu=00"
