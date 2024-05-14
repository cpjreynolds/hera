#!/bin/sh

if [ -d "$1" ]; then
    DELPATH=$1
else
    DELPATH=$PWD
fi

find $DELPATH -name ".DS_Store" -delete
