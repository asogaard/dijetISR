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
    m_tree->Branch("fatjet_tam", &m_fatjet_tam);
    m_tree->Branch("fatjet_sd_m", &m_fatjet_sd_m);
    m_tree->Branch("fatjet_sd_pt", &m_fatjet_sd_pt);
    m_tree->Branch("fatjet_sd_tau21_wta", &m_fatjet_sd_tau21_wta);
}

void dijetISR_MiniTree::FillEventUser(const xAOD::EventInfo *eventInfo) {
    if (eventInfo->isAvailable<float>("weight")) m_weight = eventInfo->auxdecor<float>("weight");
}

void dijetISR_MiniTree::FillFatJetsUser(const xAOD::Jet *fatjet, const std::string, const std::string) {
    // ungroomed pt and tau21
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

    // track assisted mass
    static SG::AuxElement::ConstAccessor<float> acc_TrackSumPt("TrackSumPt");
    static SG::AuxElement::ConstAccessor<float> acc_TrackSumMass("TrackSumMass");
    if (acc_TrackSumPt.isAvailable(*fatjet) && acc_TrackSumMass.isAvailable(*fatjet)) m_fatjet_tam.push_back(acc_TrackSumMass(*fatjet) * (fatjet->pt() / acc_TrackSumPt(*fatjet)) / 1000.);
    else m_fatjet_tam.push_back(-999.);

    // soft drop mass
    /*
    static fastjet::contrib::SoftDrop tool_SoftDrop(0.0, 0.1);
    static fastjet::contrib::Nsubjettiness nSub1_beta1(1, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
    static fastjet::contrib::Nsubjettiness nSub2_beta1(2, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));

    float sd_m = -999.;
    float sd_pt = -999.;
    float sd_tau21_wta = -999.;
    if (parent) {
        fastjet::PseudoJet jet_SoftDrop;
        
        // if pseudo jet still available, use that
        const fastjet::PseudoJet *pdj = parent->getPseudoJet();
        if (pdj) {
            jet_SoftDrop = tool_SoftDrop(*pdj);
            sd_m = jet_SoftDrop.m() / 1000.;
            sd_pt = jet_SoftDrop.pt() / 1000.;
            sd_tau21_wta = nSub2_beta1(jet_SoftDrop) / nSub1_beta1(jet_SoftDrop);
        }
        // otherwise, reconstruct it
        else {
            // clusters from jet (if we don't have these, we are SOL anyways)
            xAOD::JetConstituentVector clusters = parent->getConstituents();
            
            // make pseudo jets of the clusters
            std::vector<fastjet::PseudoJet> jet_inputs;
            TLorentzVector temp_p4;
            for (auto c : clusters) {
                if (c->e()<0) continue;
                temp_p4.SetPtEtaPhiM(c->pt(), c->eta(), c->phi(), c->m());
                jet_inputs.push_back(fastjet::PseudoJet(temp_p4.Px(),temp_p4.Py(),temp_p4.Pz(),temp_p4.E()));
            }

            if (jet_inputs.size()) {
                // anti-kt it up
                fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, 1.0, fastjet::E_scheme, fastjet::Best);
                fastjet::ClusterSequence clust_seq = fastjet::ClusterSequence(jet_inputs, jet_def);
                std::vector<fastjet::PseudoJet> pjets =  fastjet::sorted_by_pt(clust_seq.inclusive_jets(0.0));

                // find the one that is our parent jet (pt matching)
                fastjet::PseudoJet jet_Ungroomed;
                float diff = 999999.;
                for (auto jet : pjets) {
                    if (fabs(jet.pt() - fatjet->pt()) < diff) {
                        jet_Ungroomed = jet;
                        diff = fabs(jet.pt() - fatjet->pt());
                    }
                }

                if (jet_Ungroomed.has_constituents()) {
                    jet_SoftDrop = tool_SoftDrop(jet_Ungroomed);
                    sd_m = jet_SoftDrop.m() / 1000.;
                    sd_pt = jet_SoftDrop.pt() / 1000.;
                    sd_tau21_wta = nSub2_beta1(jet_SoftDrop) / nSub1_beta1(jet_SoftDrop);
                }
            }
        }
    }
    m_fatjet_sd_m.push_back(sd_m);
    m_fatjet_sd_pt.push_back(sd_pt);
    m_fatjet_sd_tau21_wta.push_back(sd_tau21_wta);
    */
}

void dijetISR_MiniTree::ClearEventUser() {
    m_weight = -999;
}

void dijetISR_MiniTree::ClearFatJetsUser(const std::string, const std::string) {
    m_fatjet_pt_ungroomed.clear();
    m_fatjet_tau21_wta_ungroomed.clear();
    m_fatjet_tam.clear();
    m_fatjet_sd_m.clear();
    m_fatjet_sd_pt.clear();
    m_fatjet_sd_tau21_wta.clear();
}
