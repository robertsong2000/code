#!/bin/sh

web_info=""
total=""
stock_value=""
stock_rate=""
gf_stock_value=""
yt_stock_value=""
yt_total=""
ad_total=""
ad_all_total=""
yt_pert=""
ad_pert=""
gf_pert=""

function usage()
{
   cat << END >&2
Usage: $progname [options]
Options:
    {-t|--top}
        Show all captical top 100.
    {-c|--cmd}
        Show user defined top 100.
    {-h|--help}
        Show the help usage.
END

}

function output()
{
    line=$(cat /tmp/stock.txt | tail -1)
    stock_value="$(echo $line | sed -e 's/[-+].*//')"
    stock_rate="$(echo $line | sed -e 's/.*(//' -e 's/)//')"
    echo "$1 当前股价: $stock_value, $stock_rate"
}

function get_file()
{
    w3m -cols 180 -dump www.baidu.com/s?wd=$1 2>&1 |grep "$1.*同花顺财经" -A 2 > /tmp/stock.txt
}

function show()
{
    get_file $1
    output $1
}

function show_top()
{
    echo "========================"
    show "广发证券"
    total=$(echo "scale=4; $stock_value * 59.19291464 / 1" | bc -l)
    gf_stock_value="$stock_value"
    echo "广发证券 市值:     $total 亿"
    echo "========================"
    show "亚泰集团"
    total=$(echo "scale=4; $stock_value * 18.947320 / 1" | bc -l)
    yt_stock_value="$stock_value"
    echo "亚泰集团 市值:     $total 亿"
    echo "========================"
    show "吉林敖东"
    total=$(echo "scale=4; $stock_value * 8.94438433 / 1" | bc -l)
    echo "吉林敖东 市值:     $total 亿"
    gf_total=$(echo "$gf_stock_value * 12.4" | bc -l)
    gf_pert=$(echo "scale=4; $gf_total / $total * 100" | bc -l)
    echo "吉林敖东 包含广发证券市值: $gf_total 亿, 占比: $gf_pert %"

    yt_total=$(echo "scale=4; $yt_stock_value * 1.29477298 / 1" | bc -l)
    yt_pert=$(echo "scale=4; $yt_total / $total * 100" | bc -l)
    echo "吉林敖东 包含亚泰集团市值: $yt_total 亿, 占比: $yt_pert %"

    ad_total="72.60"
    ad_pert=$(echo "scale=4; $ad_total / $total * 100" | bc -l)
    echo "吉林敖东 自身医药部分市值: $ad_total 亿, 占比: $ad_pert %"

    ad_all_total=$(echo "scale=4; ($ad_total + $yt_total + $gf_total) / 1" | bc -l)
    echo "吉林敖东 自身合理市值: $ad_all_total 亿"
    echo "========================"
    show "辽宁成大"
    total=$(echo "scale=4; $stock_value * 14.29709816 / 1" | bc -l)
    echo "辽宁成大 市值:     $total 亿"
    gf_total=$(echo "$gf_stock_value * 12.5" | bc -l)
    gf_pert=$(echo "scale=4; $gf_total / $total * 100" | bc -l)
    echo "辽宁成大 包含广发证券市值: $gf_total 亿, 占比：$gf_pert %"
    echo "========================"
}

function show_user()
{
    show "$1"
}

while [ $# != 0 ]; do
    case $1 in
    -h | --help)        cmd_mode="help";;
    -t | --top)         cmd_mode="top";;
    -c | --cmd)         cmd_mode="user"; web_info=$2; shift;;
              *)        ARGUMENT="$ARGUMENT $1";;

    esac
    shift
done

case "$cmd_mode" in
    help)          usage ;;
    top)           show_top;;
    user)          show_user $web_info;;
    *)             usage ;;
esac
