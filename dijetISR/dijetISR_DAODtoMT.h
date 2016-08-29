#ifndef dijetISR_DAODtoMT_H
#define dijetISR_DAODtoMT_H

#include <xAODAnaHelpers/Algorithm.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>

#include <dijetISR/dijetISR_Minitree.h>

class dijetISR_DAODtoMT : public xAH::Algorithm {
    public:
        dijetISR_DAODtoMT();

        virtual EL::StatusCode setupJob(EL::Job& job);
        virtual EL::StatusCode initialize();
        virtual EL::StatusCode execute();
        virtual EL::StatusCode histFinalize();

        std::string m_fatJetContainerName;
        std::string m_jetContainerName;
        std::string m_photonContainerName;
        std::string m_eventInfoDetailStr;
        std::string m_fatJetDetailStr;
        std::string m_jetDetailStr;
        std::string m_photonDetailStr;

        dijetISR_Minitree *m_tree; //!
        
        ClassDef(dijetISR_DAODtoMT, 1);
};

#endif
