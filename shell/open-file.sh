#!/bin/sh

function raopen {
    if [ -z "$1" ] ; then
      echo "no file"
    else
      x="${1%%"."*}"
      if [[ $x = $1 ]]; then
        vi "$@"
      else
    len="${#x}"
    strlen="${#1}"
    leftlen="$(($len - $strlen + 1))"
    subfix="${1:leftlen}"
    if [ "$subfix" == "pdf" ]; then
      mupdf "$@"
    elif [ "$subfix" == "doc" ]; then
      ooffice "$@"
    else
      vi "$@"
    fi
      fi
    fi
}

raopen "testfile.pdf"
