#ifndef dijetISR_Minitree_H
#define dijetISR_Minitree_H

#include "xAODAnaHelpers/HelpTreeBase.h"

#include "xAODEventInfo/EventInfo.h"

class dijetISR_Minitree : public HelpTreeBase {
    private:
        float m_weight;

    public:
        dijetISR_Minitree(xAOD::TEvent *event, TTree *tree, TFile *file);
        ~dijetISR_Minitree();

        void AddEventUser(const std::string detailStr = "");
        void FillEventUser(const xAOD::EventInfo *eventInfo);
        void ClearEventUser();
};

#endif
