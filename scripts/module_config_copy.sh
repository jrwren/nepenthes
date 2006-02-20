#!/bin/sh
PWD=`pwd`
MODNAME=`basename ${PWD}`

COPY=/bin/cp


CONFIG=${MODNAME}.conf
DISTCONFIG=${CONFIG}.dist

if [ ! -f ${CONFIG} ]; then
	echo Module ${MODNAME} has no configuration.
	echo copying ${DISTCONFIG} to ${CONFIG}.
	${COPY} ${DISTCONFIG} ${CONFIG}
else
	echo Module ${MODNAME} has configuration, skipping.
fi

