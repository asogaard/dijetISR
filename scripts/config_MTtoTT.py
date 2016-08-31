from xAH_config import xAH_config

c = xAH_config()

c.setalg("dijetISR_MTtoTT", { "m_applyGRL"          : False,
                              "m_GRLs"              : "",
                              "m_applyTrigger"      : False,
                              "m_doPRW"             : False,
                              "m_lumiCalcFiles"     : "",
                              "m_PRWFiles"          : "",
                              "m_PRWDefaultChannel" : 0,
                            } )
