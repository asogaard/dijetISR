#include "dijetISR/dijetISR_Minitree.h"

dijetISR_Minitree::dijetISR_Minitree(xAOD::TEvent *event, TTree *tree, TFile *file) : HelpTreeBase(event, tree, file, 1e3) {
}

dijetISR_Minitree::~dijetISR_Minitree() {
}

void dijetISR_Minitree::AddEventUser(const std::string) {
    m_tree->Branch("weight", &m_weight, "weight/F");
}

void dijetISR_Minitree::FillEventUser(const xAOD::EventInfo *eventInfo) {
    if (eventInfo->isAvailable<float>("weight")) m_weight = eventInfo->auxdecor<float>("weight");
}

void dijetISR_Minitree::ClearEventUser() {
    m_weight = -999;
}
