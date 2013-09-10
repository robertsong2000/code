#!/bin/sh

export LANG=C

to_package=""
trap 'if [ -z $to_package ] ; then exit ; fi ; rm -rf -- "$to_package"' EXIT
trap 'if [ -z $to_package ] ; then exit ; trap - EXIT; rm -rf -- "$to_package"; exit 1' HUP INT QUIT TERM

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

function save_config()
{
    #save vim
    cp $HOME/.vimrc $to_package
    cp -r $HOME/.vim $to_package

    #save bashrc
    cp $HOME/.bashrc $to_package

    #save Xmodmap
    cp $HOME/.Xmodmap $to_package
}

function clean()
{
    find $to_package -name "*.swp" | xargs rm
}

function create_tarball()
{
    tar jcvf $to_package.tar.bz2 $to_package
}

function create_backup()
{
    to_package=$1
    
    if [ "$to_package" == "" ] ; then
        echo "no pkg name input!"
        exit 1
    fi 

    mkdir -p $to_package
    save_config
    clean
    create_tarball $to_package
}

while [ $# != 0 ]; do
    case $1 in
    -h | --help)        cmd_mode="help"      ;;
              *)        ARGUMENT="$ARGUMENT $1";;

    esac
    shift
done

case "$cmd_mode" in
    help)          usage ;;
    *)             create_backup $ARGUMENT ;;
esac
