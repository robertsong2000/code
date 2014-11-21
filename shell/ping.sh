#!/bin/sh
ip="192.168.1"

if [ $# == 1 ] ; then
    ip="$1"
fi

ping_command()
{
    ping -c 3 -W 1 $ip.$1 > /dev/null 2>&1
    if [ $? -eq 0 ] ; then
        echo "ping $ip.$1 successed!"
    fi
}

for i in `seq 1 254`; do
    ping_command $i &
done
