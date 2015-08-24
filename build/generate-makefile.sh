#!/bin/bash


GENERATE_MAKEFILE_ERROR="cbp2make is not installed - please edit the Makefile manually or install cbp2make and execute this script if any files or dependencies have been added to or removed from the c::b project - get cbp2make at --> http://sourceforge.net/projects/cbp2make/"


which cbp2make > /dev/null
IS_CBP2MAKE_INSTALLED=$?

if (($IS_CBP2MAKE_INSTALLED))
then echo $GENERATE_MAKEFILE_ERROR
else echo "generating makefile"
     cbp2make -in ./loopidity.cbp -out ./Makefile
fi

exit $IS_CBP2MAKE_INSTALLED
