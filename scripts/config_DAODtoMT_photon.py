from xAH_config import xAH_config

# Script is called from xAODAnaHelpers/scripts/, and dijetISR/scripts/ is not (necessarily) in PYTHONPATH, so set manually
import os, sys, inspect
pwd = os.path.dirname(os.path.abspath(inspect.stack()[0][1]))
sys.path.append(pwd)

# Get default algorithm configurations
from config_DAODtoMT_common import get_defaults
defaults = get_defaults(args)

# Triggers
triggersList = [
    #"HLT_g120_loose",
    "HLT_g140_loose",
    #"HLT_g200_etcut",
    #"HLT_g300_etcut",
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
FatJetSelector = defaults["FatJetSelector"]
FatJetSelector["m_pT_min"]  = 100e3
FatJetSelector["m_eta_max"] = 3.0
c.setalg("JetSelector", FatJetSelector)

# Photon calibrator
c.setalg("PhotonCalibrator", defaults["PhotonCalibrator"])

# Photon selector
c.setalg("PhotonSelector", defaults["PhotonSelector"])

# Dijet + ISR analysis algorithm
dijetISR_DAODtoMT = defaults["dijetISR_DAODtoMT"]
dijetISR_DAODtoMT["m_doPhotons"] = True
c.setalg("dijetISR_DAODtoMT", dijetISR_DAODtoMT)

