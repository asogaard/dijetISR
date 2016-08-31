#ifndef dijetISR_MTtoTT_H
#define dijetISR_MTtoTT_H

#include <xAODAnaHelpers/Algorithm.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>

#include <GoodRunsLists/GoodRunsListSelectionTool.h>
#include <PileupReweighting/PileupReweightingTool.h>

#include "TTree.h"
#include "TH1.h"

class dijetISR_MTtoTT : public xAH::Algorithm {
    public:
        dijetISR_MTtoTT();

        virtual EL::StatusCode initialize();
        virtual EL::StatusCode changeInput(bool firstFile);
        virtual EL::StatusCode execute();

        void initializeVectors();
        void initializeInTree();
        void initializeOutTree();
        void copyMetaData();

        // options
        bool m_applyGRL;
        std::string m_GRLs;
        bool m_applyTrigger;
        bool m_doPRW;
        std::string m_lumiCalcFiles;
        std::string m_PRWFiles;
        int m_PRWDefaultChannel;

        // tools
        GoodRunsListSelectionTool *m_GRLTool; //!
        CP::PileupReweightingTool *m_PRWTool; //!

        // input tree branches
        int in_runNumber; //! 
        long long in_eventNumber; //!
        int in_lumiblock; //!
        float in_weight; //!
        std::vector<std::string> *in_passedTriggers; //!
        int in_nJ; //!
        std::vector<float> *in_mJ; //!
        std::vector<float> *in_ptJ; //!
        std::vector<float> *in_etaJ; //!
        std::vector<float> *in_D2J; //!
        std::vector<float> *in_C2J; //!
        std::vector<float> *in_tau21J; //!
        int in_nj; //!
        std::vector<float> *in_ptj; //!
        std::vector<float> *in_etaj; //!
        int in_ngamma; //!
        std::vector<float> *in_ptgamma; //!
        std::vector<float> *in_etagamma; //!

        // output tree branches
        TTree *m_outTree; //!
        int out_runNumber; //!
        long long out_eventNumber; //!
        float out_weight; //!
        float out_mJ; //!
        float out_ptJ; //!
        float out_etaJ; //!
        float out_D2J; //!
        float out_C2J; //!
        float out_tau21J; //!
        float out_ptj; //!
        float out_etaj; //!
        float out_ptgamma; //!
        float out_etagamma; //!
        float out_dRJj; //!
        float out_dEtaJj; //!
        float out_dPhiJj; //!
        float out_dRJgamma; //!
        float out_dEtaJgamma; //!
        float out_dPhiJgamma; //!

        // MetaData
        TH1F *m_metaData; //!

        ClassDef(dijetISR_MTtoTT, 1);
};

#endif
