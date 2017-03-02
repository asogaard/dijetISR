from xAH_config import xAH_config

# basic xAH configuration
c = xAH_config()

# single jet triggers
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

# get derivation from file list name (default is EXOT18)
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

# small-R jet sequence
if args.is_MC:
    jet_calibSeq = 'JetArea_Residual_Origin_EtaJES_GSC'
else:
    jet_calibSeq = 'JetArea_Residual_Origin_EtaJES_GSC_Insitu'

# basic event selection
c.setalg("BasicEventSelection", { "m_name"                  : "BasicEventSelection",
                                  "m_debug"                 : False,
                                  "m_derivationName"        : deriv,
                                  "m_applyGRLCut"           : False,
                                  "m_doPUreweighting"       : False,
                                  "m_vertexContainerName"   : "PrimaryVertices",
                                  "m_PVNTrack"              : 2,
                                  "m_applyPrimaryVertexCut" : True,
                                  "m_applyEventCleaningCut" : True,
                                  "m_applyCoreFlagsCut"     : True,
                                  "m_triggerSelection"      : triggers,
                                  "m_storeTrigDecisions"    : True,
                                  "m_applyTriggerCut"       : False,
                                  "m_useMetaData"           : True
                                } )

# fat jet calibrator
c.setalg("JetCalibrator", { "m_name"                  : "FatJetCalibrator",
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
                            "m_setAFII"               : False,
                            "m_systName"              : "All",
                            "m_systVal"               : 1
                          } )

c.setalg("JetSelector", { "m_name"                    : "FatJetSelector",
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
                        } )

c.setalg("JetCalibrator", { "m_name"                  : "JetCalibrator",
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
                            "m_JERFullSys"            : False,
                            "m_JERApplyNominal"       : False,
                            "m_redoJVT"               : False,
                            "m_systName"              : "All",
                            "m_systVal"               : 1
                          } )

c.setalg("JetSelector", { "m_name"                    : "JetSelector",
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
                          "m_JVTCut"                  : 0.59
                        } )

c.setalg("dijetISR_DAODtoMT", { "m_doJets"               : True,
                                "m_doPhotons"            : False,
                                "m_fatJetContainerName"  : "SelFatJets",
                                "m_jetContainerName"     : "SelJets",
                                "m_eventInfoDetailStr"   : "pileup truth",
                                "m_trigDetailStr"        : "passTriggers",
                                "m_fatJetDetailStr"      : "kinematic substructure",
                                "m_jetDetailStr"         : "kinematic",
                              } )
