#ifndef dijetISR_DAODtoMT_H
#define dijetISR_DAODtoMT_H

#include <xAODAnaHelpers/Algorithm.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>

#include <dijetISR/dijetISR_MiniTree.h>

class dijetISR_DAODtoMT : public xAH::Algorithm {
    public:
        dijetISR_DAODtoMT();

        virtual EL::StatusCode setupJob(EL::Job& job);
        virtual EL::StatusCode initialize();
        virtual EL::StatusCode execute();
        virtual EL::StatusCode histFinalize();

	// Enum class for labeling systematic type
	enum class SystType { Nominal, SmallRadius, LargeRadius, Photon };
	inline static std::string s_treeName (const std::string& systName, const SystType& systType) {
	  std::string prefix = "";
	  switch (systType) {
	    case SystType::SmallRadius : prefix = "SMALLR_"; break;
	    case SystType::LargeRadius : prefix = "LARGER_"; break;
	    case SystType::Photon      : prefix = "PHOTON_"; break;
	    default : break;
	  }
	  return prefix + systName;
	}


        EL::StatusCode addTree(std::string name);
	EL::StatusCode executeSingle(const std::string& systName, const SystType& systType);

        bool m_doJets;
        bool m_doPhotons;
        std::string m_fatJetContainerName;
        std::string m_jetContainerName;
        std::string m_photonContainerName;
        std::string m_eventInfoDetailStr;
        std::string m_trigDetailStr;
        std::string m_fatJetDetailStr;
        std::string m_jetDetailStr;
        std::string m_photonDetailStr;

        std::map<std::string, dijetISR_MiniTree*> m_trees; //!
        
        ClassDef(dijetISR_DAODtoMT, 1);
};

#endif
