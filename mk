#!/bin/bash


if [ "$1" == "debug" ]
then TARGET=debug ;   BIN_DIR=./bin/Debug ;   CMD="gdb ./loopidity-dbg" ;
else TARGET=release ; BIN_DIR=./bin/Release ; CMD="    ./loopidity" ;
fi
[ "$1" == "clean" -o "$2" == "clean" ] && SHOULD_CLEAN=1
ROOT_DIR=`pwd`
LOG_FILE="debug.log"


trace() { echo ; echo $* ; }


cd build
(($SHOULD_CLEAN)) && trace "cleaning $TARGET build" && make clean

make $TARGET || exit

trace "launching executable"
cd $BIN_DIR ; $CMD ;

trace "executable terminated"
[ -f $LOG_FILE ] && trace "$LOG_FILE -->" && cat $LOG_FILE
cd $ROOT_DIR
