#!/bin/bash

if [ $# -ne 1 ]
then
    echo "One argument required. Nothing done."
else
#    export LC_ALL="en_US.utf9"
    export X509_USER_PROXY=/afs/cern.ch/user/f/fravera/x509up_u40737
    export SCRAM_ARCH=slc6_amd64_gcc630
    export CMSSW_BASE=/afs/cern.ch/work/f/fravera/TwoPotsTracking/CMSSW_10_2_0_pre5
    cd $CMSSW_BASE/src
    eval `scramv1 runtime -sh`
    cd $CMSSW_BASE/src/Validation/CTPPS/test/fast_simu_validation
    eval "cmsRun test_acceptance_shape_realVertex_cfg.py numberOfParticles=$1"
fi
