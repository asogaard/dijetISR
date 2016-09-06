#!/bin/bash

if [ -z $1 ]
then
    echo "Suffix required!"
    exit
fi
SUFFIX=$1

PRUN="prun --optGridOutputSampleName=user.%nickname%.%in:name[1]%.%in:name[2]%.%in:name[3]%.${SUFFIX}"

if [ ! -z $2 ]
then
    DESTSITE=$2
    echo "Submitting to ${DESTSITE}"
    PRUN="${PRUN} --optGridDestSE=${DESTSITE}"
fi

# data15
#./xAODAnaHelpers/scripts/xAH_run.py --config dijetISR/scripts/config_DAODtoMT.py --files dijetISR/etc/data15.JETM6.list --inputList --inputDQ2 --submitDir ${SUFFIX}_data15 --force ${PRUN}

# data16
#./xAODAnaHelpers/scripts/xAH_run.py --config dijetISR/scripts/config_DAODtoMT.py --files dijetISR/etc/data16.JETM6.list --inputList --inputDQ2 --submitDir ${SUFFIX}_data16 --force ${PRUN}

# jetjet
./xAODAnaHelpers/scripts/xAH_run.py --config dijetISR/scripts/config_DAODtoMT.py --files dijetISR/etc/jetjet.JETM8.list --inputList --inputDQ2 --submitDir ${SUFFIX}_jetjet --force --isMC ${PRUN}

# WHad
./xAODAnaHelpers/scripts/xAH_run.py --config dijetISR/scripts/config_DAODtoMT.py --files dijetISR/etc/WHad.JETM8.list --inputList --inputDQ2 --submitDir ${SUFFIX}_WHad --force --isMC ${PRUN}

# ZHad
./xAODAnaHelpers/scripts/xAH_run.py --config dijetISR/scripts/config_DAODtoMT.py --files dijetISR/etc/ZHad.JETM8.list --inputList --inputDQ2 --submitDir ${SUFFIX}_ZHad --force --isMC ${PRUN}

# ttbar
./xAODAnaHelpers/scripts/xAH_run.py --config dijetISR/scripts/config_DAODtoMT.py --files dijetISR/etc/ttbar.JETM8.list --inputList --inputDQ2 --submitDir ${SUFFIX}_ttbar --force --isMC ${PRUN}
