#!/bin/sh

a="The cat. sat on the mat"
b=cat
strindex() { 
  x="${1%%$2*}"
  [[ $x = $1 ]] && echo -1 || echo ${#x}
}
strindex "$a" "$b"   # prints 4
strindex "$a" foo    # prints -1
strindex "$a" "."    # prints 11
