#ifndef dijetISR_MTtoTT_H
#define dijetISR_MTtoTT_H

#include <xAODAnaHelpers/Algorithm.h>

#include "TTree.h"

class dijetISR_MTtoTT : public xAH::Algorithm {
    public:
        dijetISR_MTtoTT();

        virtual EL::StatusCode setupJob(EL::Job& job);
        virtual EL::StatusCode initialize();
        virtual EL::StatusCode changeInput(bool firstFile);
        virtual EL::StatusCode execute();
        virtual EL::StatusCode finalize();

        void initializeInTree();
        void initializeOutTree();

        int in_runNumber; //! 
        long long in_eventNumber; //!
        float in_weight; //!
        int in_nJ; //!
        std::vector<float> *in_mJ; //!
        std::vector<float> *in_ptJ; //!
        int in_ngamma; //!
        std::vector<float> *in_ptgamma; //!

        TTree *m_outTree; //!
        int out_runNumber; //!
        long int out_eventNumber; //!
        float out_mJ; //!
        float out_weight; //!

        ClassDef(dijetISR_MTtoTT, 1);
};

#endif
