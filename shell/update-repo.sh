#!/bin/sh

for i in $(find . -maxdepth 1 -type d -not -name "."); do
  cd $i
  find . -maxdepth 1 -type d -name ".git" | grep git > /dev/null
  if [ $? == 0 ] ; then
    echo "cd $i ; git pull ..."
    git pull
  else
    echo "cd $i ; svn update ..."
    svn update .
  fi
  cd - > /dev/null 2>&1
done
