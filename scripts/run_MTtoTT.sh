#!/bin/bash

function stop_xAH {
    echo "Violently exiting!"
    ps x | grep xAH_run | while read line
    do
        set $line
        kill -9 $1
    done
    exit
}

trap stop_xAH SIGINT

if [ -z $1 ]
then
    echo "Output dir required!"
    exit
fi

OUTDIR=$1
DATAPATH="eos/atlas/unpledged/group-wisc/users/lkaplan/dijetISR/prod_jet_20161020"

if [ -d $OUTDIR ]
then
    echo "Output dir already exists!"
    exit
fi

mkdir $OUTDIR

# data_15
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/data15.root         --treeName MiniTree --submitDir ${OUTDIR}/TT_data15         --force        direct &

# data_16
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/data16.root         --treeName MiniTree --submitDir ${OUTDIR}/TT_data16         --force        direct &

# jetjet
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ0W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ0W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ1W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ1W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ2W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ2W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ3W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ3W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ4W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ4W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ5W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ5W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ6W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ6W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ7W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ7W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ8W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ8W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ9W.root           --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ9W           --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ10W.root          --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ10W          --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ11W.root          --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ11W          --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ12W.root          --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ12W          --force --isMC direct &

# WHad
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_280_500.root   --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_280_500   --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_500_700.root   --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_500_700   --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_700_1000.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_700_1000  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_1000_1400.root --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_1000_1400 --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_1400.root      --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_1400      --force --isMC direct &

# ZHad
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_280_500.root   --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_280_500   --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_500_700.root   --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_500_700   --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_700_1000.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_700_1000  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_1000_1400.root --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_1000_1400 --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_1400.root      --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_1400      --force --isMC direct &

while [[ `ps x | grep xAH_run | wc -l` > 1 ]]
do 
    sleep 10 
done

echo "All done!"
