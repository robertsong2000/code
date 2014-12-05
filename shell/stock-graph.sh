#!/bin/sh

FILE_DIR="/tmp"
MY_STOCK_FILE="/tmp/stock.txt"

function usage()
{
   cat << END >&2
Usage: $progname [options]
Options:
    {-f|--file}
        Set stock list file.
    {-d|--dir}
        Set dir that saving the file.
    {-g|--get}
        Download file.
    {-s|--show}
        Show in google chrome.
    {-h|--help}
        Show the help usage.
END

}

function download_daily()
{
    cd $FILE_DIR
    rm $FILE_DIR/*.gif
    cat $MY_STOCK_FILE | while read line ; do
        wget http://image.sinajs.cn/newchart/daily/n/$line.gif
    done
}

function show()
{
    add_html
    google-chrome /tmp/test.html
}

function add_html()
{
    rm /tmp/test.html
    cat $MY_STOCK_FILE | while read line ; do
        echo "<img src=\"file://$FILE_DIR/$line.gif\">" >> /tmp/test.html
    done
}

while [ $# != 0 ]; do
    case $1 in
    -h | --help)        cmd_mode="help";;
    -d | --dir)         FILE_DIR="$2"; shift ;;
    -g | --get)         cmd_mode="download";;
    -s | --show)        cmd_mode="show";;
    -f | --file)        MY_STOCK_FILE="$2"; shift ;;
              *)        ARGUMENT="$ARGUMENT $1";;

    esac
    shift
done

case "$cmd_mode" in
    help)          usage ;;
    download)      download_daily ;;
    show)          show ;;
    *)             usage ;;
esac
