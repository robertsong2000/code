#!/bin/sh

export LANG=C

PREFIX_PATH=""
FILE_LIST=""

function usage()
{
   cat << END >&2
Usage: $progname [options] package_name
Options:
    {-p|--prefix}
        Set path prefix.
    {-h|--help}
        Show the help usage.
package_name:
    Package name to be created.
END

}

function create_tarball()
{
    :
}

function create_package()
{
    to_package=$1

    if [ "$to_package" == "" ] ; then
        echo "no pkg name input!"
        exit 1
    fi 

    if [ "$PREFIX_PATH" == "" ] ; then
        echo "prefix dir is not set!"
        exit 1
    fi 

    create_tarball $to_package
}

while [ $# != 0 ]; do
    case $1 in
    -h | --help)        cmd_mode="help"      ;;
    -p  | --path)       PREFIX_PATH=$2; shift;;
              *)        ARGUMENT="$ARGUMENT $1";;

    esac
    shift
done

case "$cmd_mode" in
    help)          usage ;;
    *)             create_package $ARGUMENT ;;
esac
