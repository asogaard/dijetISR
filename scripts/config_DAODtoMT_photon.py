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

# Large-radius jet calibrator # @TODO: Make common for ISR jet and photon?
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
dijetISR_DAODtoMT["m_doPhotons"]           = True
dijetISR_DAODtoMT["m_photonContainerName"] = "SelPhotons"
dijetISR_DAODtoMT["m_photonDetailStr"]     = "kinematic"
c.setalg("dijetISR_DAODtoMT", dijetISR_DAODtoMT)



""" Reference.
# Basic event selection
c.setalg("BasicEventSelection", { "m_name"                  : "BasicEventSelection",
                                  "m_debug"                 : False,
                                  "m_derivationName"        : deriv,
                                  "m_applyGRLCut"           : False,
                                  "m_doPUreweighting"       : True, # False, -- depends on PhotonCalibrator
                                  "m_vertexContainerName"   : "PrimaryVertices",
                                  "m_PVNTrack"              : 2,
                                  "m_applyPrimaryVertexCut" : True,
                                  "m_applyEventCleaningCut" : True,
                                  "m_applyCoreFlagsCut"     : True,
                                  "m_triggerSelection"      : triggers,
                                  "m_storeTrigDecisions"    : True,
                                  "m_applyTriggerCut"       : False,
                                  "m_useMetaData"           : True,
                                  "m_isMC"                  : args.is_MC, # @asogaard
                                } )

# Large-radius jet calibrator # @TODO: Make common for ISR jet and photon?
c.setalg("JetCalibrator", { "m_name"                  : "FatJetCalibrator",
                            "m_inContainerName"       : "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                            "m_jetAlgo"               : "AntiKt10LCTopoTrimmedPtFrac5SmallR20",
                            "m_outputAlgo"            : "AntiKt10LCTopoTrimmedPtFrac5SmallR20_Calib_Algo",
                            "m_outContainerName"      : "CalibFatJets",
                            "m_debug"                 : False,
                            "m_verbose"               : False,
                            "m_sort"                  : True,
                            "m_saveAllCleanDecisions" : True,
                            "m_jetCleanCutLevel"      : "LooseBad",
                            "m_jetCleanUgly"          : True,
                            "m_cleanParent"           : True,
                            "m_applyFatJetPreSel"     : True,
                            "m_calibConfigFullSim"    : "JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config",
                            "m_calibConfigData"       : "JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config",
                            #"m_doCleaning"            : False,
                            "m_calibSequence"         : "EtaJES_JMS",
                            "m_JESUncertConfig"       : "UJ_2016/Moriond2017/UJ2016_CombinedMass_medium.config",
                            "m_JESUncertMCType"       : "MC15c",
                            "m_setAFII"               : False,
                            "m_systName"              : systName,
                            "m_systVal"               : systVal,
                          } )

# Large-radius jet selector # @TODO: Make common for ISR jet and photon?
c.setalg("JetSelector", { "m_name"                    : "FatJetSelector",
                          "m_inContainerName"         : "CalibFatJets",
                          "m_inputAlgo"               : "AntiKt10LCTopoTrimmedPtFrac5SmallR20_Calib_Algo",
                          "m_outContainerName"        : "SelFatJets",
                          "m_outputAlgo"              : "SelFatJets_Algo",
                          "m_decorateSelectedObjects" : False,
                          "m_createSelectedContainer" : True,  
                          "m_cleanJets"               : True,
                          "m_pT_min"                  : 100e3,
                          "m_eta_max"                 : 3.0,
                          "m_mass_min"                : 0.1, 
                          "m_useCutFlow"              : True,
                        } )

# Photon calibrator
c.setalg("PhotonCalibrator", { "m_name"                    : "PhotonCalibrator",
                               "m_inContainerName"         : "Photons",
                               "m_outContainerName"        : "CalibPhotons",
                               "m_outputAlgoSystNames"     : "Photons_Calib_Algo",
                               "m_esModel"                 : "es2015cPRE", # @TODO: Update
                               "m_decorrelationModel"      : "1NP_v1", # @TODO: Update
                               "m_useAFII"                 : False,
                               "m_systName"                : systName,
                               "m_systVal"                 : systVal,
                               "m_sort"                    : True,
                               "m_randomRunNumber"         : 123456, # @TODO: Update
                              } )

# Photon selector
c.setalg("PhotonSelector", { "m_name"                    : "PhotonsSelector",
                             "m_inContainerName"         : "CalibPhotons",
                             "m_inputAlgoSystNames"      : "Photons_Calib_Algo",
                             "m_outContainerName"        : "SelPhotons",
                             "m_outputAlgoSystNames"     : "SelPhotons_Algo",
                             "m_decorateSelectedObjects" : False,
                             "m_createSelectedContainer" : True,
                             "m_pass_min"                : 0,
                             "m_pT_min"                  : 100e3,
                             "m_eta_max"                 : 2.37,
                             "m_vetoCrack"               : True,
                             "m_doAuthorCut"             : True,
                             "m_doOQCut"                 : True,
                             "m_photonIdCut"             : "Tight", # @TODO: Want to propagate loose as well?
                             "m_MinIsoWPCut"             : "FixedCutTightCaloOnly"
                           } )

# Dijet + ISR analysis algorithm
c.setalg("dijetISR_DAODtoMT", { "m_doJets"               : False,
                                "m_doPhotons"            : True,
                                "m_fatJetContainerName"  : "SelFatJets",
                                "m_photonContainerName"  : "SelPhotons",
                                "m_eventInfoDetailStr"   : "pileup truth",
                                "m_trigDetailStr"        : "passTriggers",
                                "m_fatJetDetailStr"      : "kinematic substructure",
                                "m_photonDetailStr"      : "kinematic"
                              } )
"""
