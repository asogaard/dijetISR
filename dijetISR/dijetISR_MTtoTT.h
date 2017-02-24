#ifndef dijetISR_MTtoTT_H
#define dijetISR_MTtoTT_H

#include <xAODAnaHelpers/Algorithm.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>

#include <GoodRunsLists/GoodRunsListSelectionTool.h>
#include <PileupReweighting/PileupReweightingTool.h>

#include "TTree.h"
#include "TH1.h"

enum dijetISRCategory {
    _none,
    _jet,
    _gamma
};

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
        void setSumOfWeights();

        void resetBranches();
        bool passPhotonTrigger(int runNumber);
        bool passJetTrigger(int runNumber);

        // options
        bool m_doJets;
        bool m_doPhotons;
        bool m_mc;
        float m_lumi;
        bool m_applyFinalWeight;
        bool m_applyXSFix;
        bool m_applyGRL;
        std::string m_GRLs;
        bool m_doPRW;
        std::string m_lumiCalcFiles;
        std::string m_PRWFiles;
        int m_PRWDefaultChannel;

        // tools
        GoodRunsListSelectionTool *m_GRLTool; //!
        CP::PileupReweightingTool *m_PRWTool; //!

        // input tree branches
        int in_runNumber; //! 
        int in_mcChannelNumber; //!
        long long in_eventNumber; //!
        int in_lumiblock; //!
        float in_weight; //!
        std::vector<std::string> *in_passedTriggers; //!
        int in_NPV; //!
        float in_avgmu; //!
        int in_nJ; //!
        std::vector<float> *in_EJ; //!
        std::vector<float> *in_ptJ; //!
        std::vector<float> *in_etaJ; //!
        std::vector<float> *in_phiJ; //!
        std::vector<float> *in_D2J; //!
        std::vector<float> *in_C2J; //!
        std::vector<float> *in_tau21J; //!
        std::vector<float> *in_ptJ_ungroomed; //!
        std::vector<float> *in_tau21J_ungroomed; //!
        std::vector<int> *in_ntrk; //!
        //std::vector<float> *in_mJ_trackAssisted; //!
        //std::vector<float> *in_mJ_softDrop; //!
        //std::vector<float> *in_ptJ_softDrop; //!
        //std::vector<float> *in_tau21J_softDrop; //!
        //std::vector<std::vector<float> > *in_constituentsJ_pt; //!
        //std::vector<std::vector<float> > *in_constituentsJ_eta; //!
        //std::vector<std::vector<float> > *in_constituentsJ_phi; //!
        //std::vector<std::vector<float> > *in_constituentsJ_E; //!
        int in_ngamma; //!
        std::vector<float> *in_ptgamma; //!
        std::vector<float> *in_etagamma; //!
        std::vector<float> *in_phigamma; //!
        int in_nj; //!
        std::vector<float> *in_Ej; //!
        std::vector<float> *in_ptj; //!
        std::vector<float> *in_etaj; //!
        std::vector<float> *in_phij; //!

        // output tree branches
        TTree *m_outTree; //!
        int out_runNumber; //!
        long long out_eventNumber; //!
        int out_NPV; //!
        float out_avgmu; //!
        int out_category; //!
        float out_weight; //!
        float out_mJ; //!
        float out_ptJ; //!
        float out_etaJ; //!
        float out_phiJ; //!
        float out_D2J; //!
        float out_C2J; //!
        float out_tau21J; //!
        float out_ptJ_ungroomed; //!
        float out_tau21J_ungroomed; //!
        int out_ntrk; //!
        //float out_mJ_trackAssisted; //!
        //float out_mJ_softDrop; //!
        //float out_ptJ_softDrop; //!
        //float out_tau21J_softDrop; //!
        float out_tau21JDDT; //!
        //float out_constituentsJ_pt0; 
        //float out_constituentsJ_eta0; 
        //float out_constituentsJ_phi0; 
        //float out_constituentsJ_E0; 
        //float out_constituentsJ_pt1; 
        //float out_constituentsJ_eta1; 
        //float out_constituentsJ_phi1; 
        //float out_constituentsJ_E1; 
        float out_ptgamma; //!
        float out_etagamma; //!
        float out_phigamma; //!
        float out_mj; //!
        float out_ptj; //!
        float out_etaj; //!
        float out_phij; //1
        float out_dEtaJgamma; //!
        float out_dPhiJgamma; //!
        float out_dRJgamma; //!
        float out_dEtaJj; //!
        float out_dPhiJj; //!
        float out_dRJj; //!

        // MetaData
        TH1F *m_metaData; //!

        // sum of weights
        float m_sumOfWeights; //!

        // map of XS
        std::map<int, float> m_crossSections; //!

        ClassDef(dijetISR_MTtoTT, 1);
};

#endif
