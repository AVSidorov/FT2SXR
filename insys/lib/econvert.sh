#!/bin/bash

#
# convert file *.cpp *.h *.ini ...
# from cp1251 encoding to utf8 using enconv
# sudo apt-get install enca
#

LANG=russian
TO=utf8

list=`find $1 -type f -o -name "*.htm" -o -name "*.v" \
-o -name "*.ini" -o -name "*.vhd"  -o -name "*.tcl" \
-o -name "*.txt" -o -name ".cpp" -o -name ".h"`

for FILE in ${list}
do
    echo -n Converting file ${FILE} ...
    enconv -x ${TO} ${FILE} -L ${LANG}
    echo DONE.
done

exit 0
