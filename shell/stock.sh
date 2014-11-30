#!/bin/sh

IS_TOP=0
rate="0.0517790009"
web_info=""
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

function show_top()
{
    echo "日本股市市值100名"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx"
    show "http://www.nikkei.com/markets/ranking/stock/caphigh.aspx?Babu=11&PageNo=2&Gyosyu=00"
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
