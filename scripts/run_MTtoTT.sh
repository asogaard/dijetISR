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

if [ -z $1 ]
then
    echo "Output dir required!"
    exit
fi

OUTDIR=$1
DATAPATH="eos/atlas/unpledged/group-wisc/users/lkaplan/dijetISR/prod_jet_20161104"

if [ -d $OUTDIR ]
then
    echo "Output dir already exists!"
    exit
fi

mkdir $OUTDIR

trap stop_xAH SIGINT

# data_15
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/data15.root            --treeName MiniTree --submitDir ${OUTDIR}/TT_data15            --force        direct &

# data_16
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/data16.root            --treeName MiniTree --submitDir ${OUTDIR}/TT_data16            --force        direct &

# jetjet
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ0.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ0               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ1.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ1               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ2.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ2               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ3.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ3               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ4.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ4               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ5.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ5               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ6.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ6               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ7.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ7               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ8.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ8               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ9.root               --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ9               --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ10.root              --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ10              --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ11.root              --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ11              --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/JZ12.root              --treeName MiniTree --submitDir ${OUTDIR}/TT_JZ12              --force --isMC direct &

# WHad
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_280_500.root      --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_280_500      --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_500_700.root      --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_500_700      --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_700_1000.root     --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_700_1000     --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_1000_1400.root    --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_1000_1400    --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/WHad_1400.root         --treeName MiniTree --submitDir ${OUTDIR}/TT_WHad_1400         --force --isMC direct &

# ZHad
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_280_500.root      --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_280_500      --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_500_700.root      --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_500_700      --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_700_1000.root     --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_700_1000     --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_1000_1400.root    --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_1000_1400    --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/ZHad_1400.root         --treeName MiniTree --submitDir ${OUTDIR}/TT_ZHad_1400         --force --isMC direct &

# ISR jet signal
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_30.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_30  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_40.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_40  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_50.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_50  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_60.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_60  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_80.root  --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_80  --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_100.root --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_100 --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_130.root --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_130 --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_160.root --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_160 --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_190.root --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_190 --force --isMC direct &
xAH_run.py --config dijetISR/scripts/config_MTtoTT.py --files ${DATAPATH}/Signal_ISRjet_220.root --treeName MiniTree --submitDir ${OUTDIR}/TT_Signal_ISRjet_220 --force --isMC direct &

while [[ `ps x | grep xAH_run | wc -l` > 1 ]]
do 
    sleep 10 
done

echo "All done!"
