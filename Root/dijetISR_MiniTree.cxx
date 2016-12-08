#include "dijetISR/dijetISR_MiniTree.h"

dijetISR_MiniTree::dijetISR_MiniTree(xAOD::TEvent *event, TTree *tree, TFile *file) : HelpTreeBase(event, tree, file, 1e3) {
}

dijetISR_MiniTree::~dijetISR_MiniTree() {
}

void dijetISR_MiniTree::AddEventUser(const std::string) {
    m_tree->Branch("weight", &m_weight, "weight/F");
}

void dijetISR_MiniTree::AddFatJetsUser(const std::string, const std::string, const std::string) {
    m_tree->Branch("fatjet_pt_ungroomed", &m_fatjet_pt_ungroomed);
    m_tree->Branch("fatjet_tau21_wta_ungroomed", &m_fatjet_tau21_wta_ungroomed);
}

void dijetISR_MiniTree::FillEventUser(const xAOD::EventInfo *eventInfo) {
    if (eventInfo->isAvailable<float>("weight")) m_weight = eventInfo->auxdecor<float>("weight");
}

void dijetISR_MiniTree::FillFatJetsUser(const xAOD::Jet *fatjet, const std::string, const std::string) {
    const xAOD::Jet *parent = 0;
    auto el = fatjet->auxdata<ElementLink<xAOD::JetContainer> >("Parent");
    if (el.isValid()) parent = *el;
    if (parent) {
        m_fatjet_pt_ungroomed.push_back(parent->pt() / 1000.);
        static SG::AuxElement::ConstAccessor<float> acc_tau21_wta_ungroomed("Tau21_wta");
        static SG::AuxElement::ConstAccessor<float> acc_tau2_wta_ungroomed("Tau2_wta");
        static SG::AuxElement::ConstAccessor<float> acc_tau1_wta_ungroomed("Tau1_wta");
        if (acc_tau21_wta_ungroomed.isAvailable(*parent)) m_fatjet_tau21_wta_ungroomed.push_back(acc_tau21_wta_ungroomed(*parent));
        else if (acc_tau2_wta_ungroomed.isAvailable(*parent) && acc_tau1_wta_ungroomed.isAvailable(*parent)) m_fatjet_tau21_wta_ungroomed.push_back(acc_tau2_wta_ungroomed(*parent) / acc_tau1_wta_ungroomed(*parent));
        else m_fatjet_tau21_wta_ungroomed.push_back(-999.);
    }
    else {
        m_fatjet_pt_ungroomed.push_back(-999.);
        m_fatjet_tau21_wta_ungroomed.push_back(-999.);
    }
}

void dijetISR_MiniTree::ClearEventUser() {
    m_weight = -999;
}

void dijetISR_MiniTree::ClearFatJetsUser(const std::string, const std::string) {
    m_fatjet_pt_ungroomed.clear();
    m_fatjet_tau21_wta_ungroomed.clear();
}
