#ifndef dijetISR_MiniTree_H
#define dijetISR_MiniTree_H

#include "xAODAnaHelpers/HelpTreeBase.h"

#include "xAODEventInfo/EventInfo.h"

class dijetISR_MiniTree : public HelpTreeBase {
    private:
        float m_weight;

    public:
        dijetISR_MiniTree(xAOD::TEvent *event, TTree *tree, TFile *file);
        ~dijetISR_MiniTree();

        void AddEventUser(const std::string detailStr = "");
        void FillEventUser(const xAOD::EventInfo *eventInfo);
        void ClearEventUser();
};

#endif
