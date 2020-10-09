#!/bin/bash -e
#----------------------------------------------------------
#  Script: zlaunch.sh
#  Author: Michael Benjamin
#  LastEd: Sep 25th 2020
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
VERBOSE="no"
FLOW_DOWN_ARGS=""

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "zlaunch.sh [SWITCHES] [time_warp]                       " 
	echo "  --help, -h        Show this help message              " 
        echo "  --verbose, -v     Enable verbose mode                 "
	echo "  --res, -r         Tell xlaunch to generate report     " 
	echo "  --send, -s        Tell xlaunch to gen and send report " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI}" = "--res" -o "${ARGI}" = "-r" ]; then
        FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI}" = "--send" -o "${ARGI}" = "-s" ]; then
        FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI}" = "--ubu" -o "${ARGI}" = "--pi" ]; then
        FLOW_DOWN_ARGS+="${ARGI} "
    else 
        echo "zlaunch.sh Bad arg:" $ARGI "Exiting with code: 1"
        exit 1
    fi
done

FLOW_DOWN_ARGS+="${TIME_WARP} "

echo "zlaunch.sh FLOW_DOWN_ARGS:[$FLOW_DOWN_ARGS]"

IO=""
if [ ${VERBOSE} = "yes" ]; then
    IO=">& /dev/null"
fi

xlaunch.sh $FLOW_DOWN_ARGS --com=alpha --test=opr_01 $IO
xlaunch.sh $FLOW_DOWN_ARGS --com=alpha --test=opr_02 $IO
xlaunch.sh $FLOW_DOWN_ARGS --com=alpha --test=opr_03 $IO

