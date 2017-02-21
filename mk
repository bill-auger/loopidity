#!/bin/bash

ROOT_DIR=`pwd`
LOG_FILE="debug.log"

if [ "$1" == "debug" ]
then TARGET=release ; CMD="./bin/loopidity" ;
else TARGET=debug ;   CMD="gdb" ;
fi
[ "$1" == "clean" -o "$2" == "clean" ] && SHOULD_CLEAN=1


trace() { echo ; echo $* ; }


cd build
(($SHOULD_CLEAN)) && trace "cleaning $TARGET build" && make clean CONFIG=$TARGET

make CONFIG=$TARGET || exit

trace "launching executable"
$CMD

trace "executable terminated"
[ -f $LOG_FILE ] && trace "$LOG_FILE -->" && cat $LOG_FILE
cd $ROOT_DIR
