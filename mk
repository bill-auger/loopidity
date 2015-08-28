#!/bin/bash

echo $COMSPEC | grep WINDOWS > /dev/null
IS_NOT_WINDOWS=$?
ROOT_DIR=`pwd`


if (($IS_NOT_WINDOWS))
then cd build &&         make release && cd bin/Release/ && ./loopidity
else cd build && MINGW=1 make release && cd bin/Release/ && ./loopidity.exe
fi
cd $ROOT_DIR
