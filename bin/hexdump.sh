#!/bin/sh
BASEDIR="${PWD##*/}"

if [ $# -lt 1 ]; then
  echo "Please enter the file: "
  read tag
else
  tag=$1
fi

xxd -i $tag $tag.h
