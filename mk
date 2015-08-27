#!/bin/bash

echo $COMSPEC | grep WINDOWS > /dev/null
IS_NOT_WINDOWS=$?
PWD=`pwd`

if (($IS_NOT_WINDOWS))
then cd build &&         make && cd bin/Release/ && ./loopidity
else cd build && MINGW=1 make && cd bin/Release/ && ./loopidity.exe
fi
cd $PWD
