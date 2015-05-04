#!/bin/sh

LANG=C
year=`date +"%Y"`
month=`date +"%m"`
day="Su"

function usage()
{
   cat << END >&2
Usage: $progname [options]
Options:
    {-m|--month}
        Set the month.
    {-d|--day}
        Set day info, the available value is:
        Su, Mo, Tu, We, Th, Fr, Sa 
    {-h|--help}
        Show the help usage.
END

}

function print_all_day()
{
    for i in $(cal -N -m $month | grep $day | sed -e "s/$day//"); do
        echo $year-$month-$i
    done
}

while [ $# != 0 ]; do
    case $1 in
    -h | --help)        cmd_mode="help";;
    -m | --month)       cmd_mode="print"; month=$2; shift;;
    -d | --day)         cmd_mode="print"; day=$2; shift;;
              *)        ARGUMENT="$ARGUMENT $1";;
    esac
    shift
done

case "$cmd_mode" in
    help)          usage ;;
    print)         print_all_day;;
    *)             usage ;;
esac

