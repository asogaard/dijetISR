#ifndef dijetISR_MiniTree_H
#define dijetISR_MiniTree_H

#include "xAODAnaHelpers/HelpTreeBase.h"

#include "xAODEventInfo/EventInfo.h"

class dijetISR_MiniTree : public HelpTreeBase {
    private:
        float m_weight;
        std::vector<float> m_fatjet_pt_ungroomed;
        std::vector<float> m_fatjet_tau21_wta_ungroomed;

    public:
        dijetISR_MiniTree(xAOD::TEvent *event, TTree *tree, TFile *file);
        ~dijetISR_MiniTree();

        void AddEventUser(const std::string detailStr = "");
        void AddFatJetsUser(const std::string, const std::string, const std::string);
        void FillEventUser(const xAOD::EventInfo *eventInfo);
        void FillFatJetsUser(const xAOD::Jet *fatjet, const std::string, const std::string);
        void ClearEventUser();
        void ClearFatJetsUser(const std::string, const std::string);
};

#endif
