#!/bin/bash

#
# Если мы вызываем этот скрипт для сборки под ARM,
# например из среды buildroot, то необходимо в
# в качестве параметра передать путь к скрипту:
#
# source ${HOME}/bardy/BardyEnv.sh ${HOME}/bardy
#
# В остальных случаях использовать стандартно:
#
# source ${HOME}/bardy/BardyEnv.sh
#

SETTINGS_FILE='BardyEnv.sh'
BARDY_SCRIPT_LOC="./"

if [ $# != 0 ]; then
        BARDY_SCRIPT_LOC="$1"
else
        #  BARDY_SCRIPT_LOC should point to script location
        if [ "$0" == "ksh" ]; then
                BARDY_SCRIPT_LOC_TMP_UNI=`readlink -f ${BRD_ARG_}`
        else
                BARDY_SCRIPT_LOC_TMP_UNI=$BASH_SOURCE
        fi
        BARDY_SCRIPT_LOC_TMP_UNI=${BARDY_SCRIPT_LOC_TMP_UNI%/*}
        if [ "$BARDY_SCRIPT_LOC_TMP_UNI" != "" ]; then
                if [ "$BARDY_SCRIPT_LOC_TMP_UNI" == "BardyEnv.sh" ]; then
                        BARDY_SCRIPT_LOC_TMP_UNI="./"
                fi
                BARDY_SCRIPT_LOC_TMP_UNI=`readlink -f ${BARDY_SCRIPT_LOC_TMP_UNI}`
                if [ $? == 0 ]; then
                        BARDY_SCRIPT_LOC=${BARDY_SCRIPT_LOC_TMP_UNI}
                fi
        fi
        unset BARDY_SCRIPT_LOC_TMP_UNI
fi


export BARDYDIR=`readlink -f "${BARDY_SCRIPT_LOC}"`
export BARDYLIB=${BARDYDIR}/bin

if [ -z "$GIPCYDIR" ]; then
    export GIPCYDIR=${BARDYDIR}/gipcy
fi

export GIPCYLIB=${GIPCYDIR}/lib
export GIPCYINC=${GIPCYDIR}/include
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${GIPCYLIB}:${BARDYLIB}
export PS1="\[\e[32;1m\][bardy-${ARCH}]\[\e[0m\]:\w> "
