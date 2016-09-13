#include "dijetISR/dijetISR_MiniTree.h"

dijetISR_MiniTree::dijetISR_MiniTree(xAOD::TEvent *event, TTree *tree, TFile *file) : HelpTreeBase(event, tree, file, 1e3) {
}

dijetISR_MiniTree::~dijetISR_MiniTree() {
}

void dijetISR_MiniTree::AddEventUser(const std::string) {
    m_tree->Branch("weight", &m_weight, "weight/F");
}

void dijetISR_MiniTree::FillEventUser(const xAOD::EventInfo *eventInfo) {
    if (eventInfo->isAvailable<float>("weight")) m_weight = eventInfo->auxdecor<float>("weight");
}

void dijetISR_MiniTree::ClearEventUser() {
    m_weight = -999;
}
