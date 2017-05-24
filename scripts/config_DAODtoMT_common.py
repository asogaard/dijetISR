"""
@file: config_DAODtoMT_common.py
@date: 7 March 2017

Common defintions for both ISR jet and photon channel in boosted dijet + ISR analysis. For use with xAODAnaHelpers framework. 
"""

def get_defaults(args):
    """ Utility function, returning common, default algorithm definitions. """

    # Get derivation from file list name (default is EXOT18)
    deriv = ''
    if args.use_inputFileList:
        inputstring = str.split((args.input_filename)[0], ".")
        deriv = inputstring[-2]
        deriv += "Kernel"
        if ("JETM" not in deriv and "EXOT" not in deriv):
            raise Exception("Invalid file list name!  Must be of form <name>.<derivation>.list")
    else:
        inputstring = str.split((args.input_filename)[0], "/")
        dname = str.split(inputstring[-1], ".")
        dname1 = str.split(dname[0], "_")
        if "DAOD" in dname1[0]:
            deriv = dname1[1] + "Kernel"
        else:
            deriv = 'EXOT18Kernel'
            pass
        pass
    
    # Deduce systematics setting
    if args.is_MC:
        systName = "All"
        systVal  = 1
    else:
        systName = "Nominal"
        systVal  = 0
        pass
    
    print("systName: '%s', systVal: '%d'" % (systName, systVal))
    
    # Small-R jet sequence
    if args.is_MC:
        jet_calibSeq = 'JetArea_Residual_Origin_EtaJES_GSC'
    else:
        jet_calibSeq = 'JetArea_Residual_Origin_EtaJES_GSC_Insitu'
        pass
    
    # Default algorithms collection
    defaults = dict()
    
    # Basic event selection
    defaults["BasicEventSelection"] = { "m_name"                  : "BasicEventSelection",
                                        "m_debug"                 : False,
                                        "m_derivationName"        : deriv,
                                        "m_applyGRLCut"           : False,
                                        "m_doPUreweighting"       : False,
                                        "m_vertexContainerName"   : "PrimaryVertices",
                                        "m_PVNTrack"              : 2,
                                        "m_applyPrimaryVertexCut" : True,
                                        "m_applyEventCleaningCut" : True,
                                        "m_applyCoreFlagsCut"     : True,
                                        #"m_triggerSelection"      : triggers, # Channel-specific
                                        "m_storeTrigDecisions"    : True,
                                        "m_applyTriggerCut"       : False,
                                        "m_useMetaData"           : True,
                                        }
    
    # Large-radius jet calibrator
    # @TODO: Not doing JER?
    defaults["FatJetCalibrator"] = { "m_name"                  : "FatJetCalibrator",
                                     "m_inContainerName"       : "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                     "m_jetAlgo"               : "AntiKt10LCTopoTrimmedPtFrac5SmallR20",
                                     "m_outContainerName"      : "CalibFatJets",
                                     "m_outputAlgo"            : "AntiKt10LCTopoTrimmedPtFrac5SmallR20_Calib_Algo",
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
                                     "m_calibSequence"         : "EtaJES_JMS",
                                     "m_JESUncertConfig"       : "UJ_2016/Moriond2017/UJ2016_CombinedMass_medium.config",
                                     "m_JESUncertMCType"       : "MC15c",
                                     #"m_JERUncertConfig"       : "...",
                                     #"m_JERFullSys"            : True, 
                                     "m_setAFII"               : False,
                                     "m_systName"              : systName,
                                     "m_systVal"               : systVal,
                                     }
    
    # Large-radius jet selector
    defaults["FatJetSelector"] = { "m_name"                    : "FatJetSelector",
                                   "m_inContainerName"         : "CalibFatJets",
                                   "m_inputAlgo"               : "AntiKt10LCTopoTrimmedPtFrac5SmallR20_Calib_Algo",
                                   "m_outContainerName"        : "SelFatJets",
                                   "m_outputAlgo"              : "SelFatJets_Algo",
                                   "m_decorateSelectedObjects" : False,
                                   "m_createSelectedContainer" : True,  
                                   "m_cleanJets"               : True,
                                   "m_pT_min"                  : 200e3,
                                   "m_eta_max"                 : 2.0,
                                   "m_mass_min"                : 0.1, 
                                   "m_useCutFlow"              : True,
                                   } 
    
    # Small-radius jet calibrator
    defaults["JetCalibrator"] = { "m_name"                  : "JetCalibrator",
                                  "m_inContainerName"       : "AntiKt4EMTopoJets",
                                  "m_jetAlgo"               : "AntiKt4EMTopo",
                                  "m_outContainerName"      : "CalibJets",
                                  "m_outputAlgo"            : "AntiKt4EMTopo_Calib_Algo",
                                  "m_debug"                 : False,
                                  "m_verbose"               : False,
                                  "m_sort"                  : True,
                                  "m_saveAllCleanDecisions" : True,
                                  "m_jetCleanCutLevel"      : "LooseBad",
                                  "m_jetCleanUgly"          : True,
                                  "m_calibConfigFullSim"    : "JES_data2016_data2015_Recommendation_Dec2016.config",
                                  "m_calibConfigData"       : "JES_data2016_data2015_Recommendation_Dec2016.config",
                                  "m_calibSequence"         : jet_calibSeq,
                                  "m_setAFII"               : False,
                                  "m_JESUncertConfig"       : "JES_2016/Moriond2017/JES2016_SR_Scenario1.config",
                                  "m_JESUncertMCType"       : "MC15",
                                  "m_JERUncertConfig"       : "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root",
                                  "m_JERFullSys"            : False, # @TODO: Do JER?
                                  "m_JERApplyNominal"       : False, # @TODO: Do JER?
                                  "m_redoJVT"               : False,
                                  "m_systName"              : systName,
                                  "m_systVal"               : systVal,
                                  }
    
    # Small-radius jet selector
    defaults["JetSelector"] = { "m_name"                    : "JetSelector",
                                "m_inContainerName"         : "CalibJets",
                                "m_inputAlgo"               : "AntiKt4EMTopo_Calib_Algo",
                                "m_outContainerName"        : "SelJets",
                                "m_outputAlgo"              : "SelJets_Algo",
                                "m_decorateSelectedObjects" : False,
                                "m_createSelectedContainer" : True,
                                "m_cleanJets"               : True,
                                "m_pT_min"                  : 20e3,
                                "m_eta_max"                 : 2.4,
                                "m_pass_min"                : 0,
                                "m_useCutFlow"              : True,
                                "m_doBTagCut"               : False,
                                "m_doJVT"                   : True,
                                "m_pt_max_JVT"              : 60e3,
                                "m_eta_max_JVT"             : 2.4,
                                "m_JVTCut"                  : 0.59,
                                }
    
    # Photon calibrator
    defaults["PhotonCalibrator"] = { "m_name"                    : "PhotonCalibrator",
                                     "m_inContainerName"         : "Photons",
                                     "m_outContainerName"        : "CalibPhotons",
                                     "m_outputAlgoSystNames"     : "Photons_Calib_Algo",
                                     "m_esModel"                 : "es2016data_mc15c", # [1]
                                     "m_decorrelationModel"      : "1NP_v1", # or "FULL_ETACORRELATED_v1" # [2,3]
                                     "m_useAFII"                 : False,
                                     "m_systName"                : systName,
                                     "m_systVal"                 : systVal,
                                     "m_sort"                    : True,
                                     "m_randomRunNumber"         : 123456, # @TODO: Update
                                     }
    
    # Photon selector
    defaults["PhotonSelector"] = { "m_name"                    : "PhotonSelector",
                                   "m_inContainerName"         : "CalibPhotons",
                                   "m_inputAlgoSystNames"      : "Photons_Calib_Algo",
                                   "m_outContainerName"        : "SelPhotons",
                                   "m_outputAlgoSystNames"     : "SelPhotons_Algo",
                                   "m_decorateSelectedObjects" : True,
                                   "m_createSelectedContainer" : True,
                                   "m_pass_min"                : 0,
                                   "m_pT_min"                  : 100e3,
                                   "m_eta_max"                 : 2.37,
                                   "m_vetoCrack"               : True,
                                   "m_doAuthorCut"             : True,
                                   "m_doOQCut"                 : True,
                                   "m_photonIdCut"             : "Tight", #  "Loose" / "Tight"
                                   "m_MinIsoWPCut"             : "FixedCutTightCaloOnly" # "FixedCutLoose" / "FixedCutTightCaloOnly" # [4]
                                   }
    
    # Dijet + ISR analysis algorithm
    defaults["dijetISR_DAODtoMT"] = { "m_doJets"               : False,
                                      "m_doPhotons"            : False,
                                      "m_fatJetContainerName"  : "SelFatJets",
                                      "m_jetContainerName"     : "SelJets",
                                      "m_photonContainerName"  : "SelPhotons",
                                      "m_eventInfoDetailStr"   : "pileup truth",
                                      "m_trigDetailStr"        : "passTriggers",
                                      "m_fatJetDetailStr"      : "kinematic substructure",
                                      "m_jetDetailStr"         : "kinematic", 
                                      "m_photonDetailStr"      : "kinematic isolation PID effSF", # purity
                                      }
    # Return
    return defaults


# References:
# --------------------------------------
# [1] https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronPhotonFourMomentumCorrection#Recommendations_for_data15_data1
# [2] https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronPhotonFourMomentumCorrection#New_correlation_model_to_combine
# [3] https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronPhotonFourMomentumCorrection#Configuration_properties
# [4] https://twiki.cern.ch/twiki/bin/view/AtlasProtected/IsolationSelectionTool#Photons
