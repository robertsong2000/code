#!/bin/sh

IS_TOP=0
rate="0.0552"
web_info=""
index=1
#input_file=$1

function usage()
{
   cat << END >&2
Usage: $progname [options]
Options:
    {-t|--top}
        Show all captical top 100.
    {-b|--bank}
        Show bank top 100.
    {-s|--steal}
        Show steal top 100.
    {-S|--stock}
        Show stock top 100.
    {-m|--medicine}
        Show medicine top 100.
    {-f|--food}
        Show food top 100.
    {-i|--insurance}
        Show insurance top 100.
    {-c|--cmd}
        Show user defined top 100.
    {-h|--help}
        Show the help usage.
END

}

function output()
{
    cat /tmp/stock.txt |grep "^[0-9]" | awk '{print $3,$5}' | while read line ; do
        if [ -n "$line" ] ; then
            stock_name=$(echo $line | sed -e 's/ .*//');
            stock_chinese_name=$(grep "^$stock_name " table.txt | awk '{print $2,$3}')
            ##echo $stock_chinese_name
            stock_value=$(echo $line | sed -e 's/.* //'| sed -e 's/,//g');
            to_chn=$(echo "$stock_value / 1000 / 1000 / 100 * $rate" | bc -l);
            to_chn=$(printf "%.2f亿\n" $to_chn);
            echo "<tr><th>$index</th><th>$stock_chinese_name</th><th>$to_chn</th></tr>"
            index=$((index + 1))
            #printf "%-30s\t%.2f亿\n" "$stock_chinese_name" $to_chn ;
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

function show_head()
{
    cur_date=$(date +%F)
    echo "相关资料由 <a href=\"http://robertsong.cn\">http://robertsong.cn</a> 整理, 转载请注明出处"
    echo "本日($cur_date)汇率: 1日元=$rate人民币"
}

function show_tail()
{
    echo "信息来源: http://www.nikkei.com/"
}

function show_top()
{
    show_head
    echo "日本股市市值100名"
    echo "<table>"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=2&Gyosyu=00"
    echo "</table>"
    show_tail
}

function show_bank()
{
    echo "日本银行排名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=&Gyosyu=47"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=2&Gyosyu=47"
}

function show_steal()
{
    echo "日本钢铁业排名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=&Gyosyu=17"
}

function show_stock()
{
    echo "日本证券业排名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=&Gyosyu=49"
}

function show_medicine()
{
    echo "日本医药业排名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=&Gyosyu=09"
}

function show_food()
{
    echo "日本饮食业排名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=&Gyosyu=01"
}

function show_insurance()
{
    echo "日本保险业排名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=&Gyosyu=51"
}

function show_user()
{
    show "$1"
}

while [ $# != 0 ]; do
    case $1 in
    -h | --help)        cmd_mode="help";;
    -t | --top)         cmd_mode="top";;
    -b | --bank)        cmd_mode="bank";;
    -s | --steal)       cmd_mode="steal";;
    -S | --stock)       cmd_mode="stock";;
    -m | --medicine)    cmd_mode="medicine";;
    -f | --food)        cmd_mode="food";;
    -i | --insurance)   cmd_mode="insurance";;
    -c | --cmd)         cmd_mode="user"; web_info=$2; shift;;
              *)        ARGUMENT="$ARGUMENT $1";;

    esac
    shift
done

case "$cmd_mode" in
    help)          usage ;;
    top)           show_top;;
    bank)          show_bank;;
    steal)         show_steal;;
    stock)         show_stock;;
    medicine)      show_medicine;;
    food)          show_food;;
    insurance)     show_insurance;;
    user)          show_user $web_info;;
    *)             usage ;;
esac
