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

        EL::StatusCode addTree(std::string name);
        EL::StatusCode executeSingle(std::string resolvedSys = "", std::string boostedSys = "");

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
