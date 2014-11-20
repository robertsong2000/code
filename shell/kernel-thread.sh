#!/bin/sh

if [ $# == 0 ] ; then
    echo "input pid value!"
    exit 1
fi

if [ ! -f "/proc/$1/stat" ] ; then
    echo "no such process!"
    exit 1
fi

flag=$(cat /proc/$1/stat  | awk '{print $9}' )
result=$(gdb -q -ex "p/x $flag & 0x00200000" --batch)
result=$(echo $result | sed -e 's/$1 =//')

if [ $result == "0x0" ] ; then
    echo " not kernel thread"
else
    echo " kernel thread"
fi
