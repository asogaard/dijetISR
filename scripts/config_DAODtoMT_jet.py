from xAH_config import xAH_config

# Script is called from xAODAnaHelpers/scripts/, and dijetISR/scripts/ is not (necessarily) in PYTHONPATH, so set manually
import os, sys, inspect
pwd = os.path.dirname(os.path.abspath(inspect.stack()[0][1]))
sys.path.append(pwd)

# Get default algorithm configurations
from config_DAODtoMT_common import get_defaults
defaults = get_defaults(args)

# Single jet triggers
triggersList = [
    "HLT_j340",
    "HLT_j380",
    "HLT_j360_a10_lcw_L1J100",
    "HLT_j360_a10r_L1J100",
    "HLT_j400_a10_lcw_L1J100",
    "HLT_j400_a10r_L1J100",
    "HLT_j420_a10_lcw_L1J100",
    "HLT_j420_a10r_L1J100",
    "HLT_ht700_L1J75",
    "HLT_ht1000",
    "HLT_ht1000_L1J100",
]
triggers = ",".join(triggersList)

# xAODAnaHelper algorithm configuration
c = xAH_config()

# Basic event selection
BasicEventSelection = defaults["BasicEventSelection"]
BasicEventSelection["m_triggerSelection"] = triggers
c.setalg("BasicEventSelection", BasicEventSelection)

# Large-radius jet calibrator
c.setalg("JetCalibrator", defaults["FatJetCalibrator"])

# Large-radius jet selector
c.setalg("JetSelector", defaults["FatJetSelector"])

# Small-radius jet calibrator
c.setalg("JetCalibrator", defaults["JetCalibrator"])

# Small-radius jet selector
c.setalg("JetSelector", defaults["JetSelector"])

# Dijet + ISR analysis algorithm
dijetISR_DAODtoMT = defaults["dijetISR_DAODtoMT"]
dijetISR_DAODtoMT["m_doJets"] = True
c.setalg("dijetISR_DAODtoMT", dijetISR_DAODtoMT)
