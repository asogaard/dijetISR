#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <utility>      
#include <iostream>
#include <fstream>
#include <math.h>

#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include <dijetISR/dijetISR_MTtoTT.h>

ClassImp(dijetISR_MTtoTT)

dijetISR_MTtoTT::dijetISR_MTtoTT() {
    // options
    m_doJets = false;
    m_doJets = false;
    m_mc = false;
    m_lumi = -1.;
    m_applyFinalWeight = false;
    m_applyXSFix = false;
    m_applyGRL = false;
    m_GRLs = "";
    m_doPRW = false;
    m_lumiCalcFiles = "";
    m_PRWFiles = "";
    m_PRWDefaultChannel = 0;

    // tools
    m_GRLTool = 0;
    m_PRWTool = 0;

    // output TinyTree
    m_outTree = 0;

    // output MetaData
    m_metaData = 0;

    // sum of weights
    m_sumOfWeights = 1.;

    // map of XS
    m_crossSections.clear();

    // initalize vectors to 0
    initializeVectors();
}

EL::StatusCode dijetISR_MTtoTT::initialize() {
    if ((!m_doJets && !m_doPhotons) || (m_doJets && m_doPhotons)) {
        std::cout << "Must select only one of m_doJets or m_doPhotons!" << std::endl;
        return EL::StatusCode::SUCCESS;
    }
    
    // get sum of weights
    if (m_mc && m_applyFinalWeight) setSumOfWeights();

    // GRL tool
    if (m_applyGRL) {
        m_GRLTool = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");
        std::vector<std::string> grlVec;
        std::istringstream s(m_GRLs);
        std::string g;
        while (std::getline(s, g, ',')) grlVec.push_back(gSystem->ExpandPathName(g.c_str()));
        RETURN_CHECK("dijetISR_MTtoTT::initialize()", m_GRLTool->setProperty("GoodRunsListVec", grlVec), "");
        RETURN_CHECK("dijetISR_MTtoTT::initialize()", m_GRLTool->setProperty("PassThrough", false), "");
        RETURN_CHECK("dijetISR_MTtoTT::initialize()", m_GRLTool->initialize(), "");
    }

    // PRW tool
    if (m_doPRW) {
        m_PRWTool = new CP::PileupReweightingTool("PileupReweightingTool");
        std::vector<std::string> lumiCalcVec;
        std::istringstream s1(m_lumiCalcFiles);
        std::string f1;
        while (std::getline(s1, f1, ',')) lumiCalcVec.push_back(f1);
        std::vector<std::string> prwVec;
        std::istringstream s2(m_PRWFiles);
        std::string f2;
        while (std::getline(s2, f2, ',')) prwVec.push_back(f1);
        RETURN_CHECK("dojetISR_MTtoTT::initialize()", m_PRWTool->setProperty("LumiCalcFiles", lumiCalcVec), "");
        RETURN_CHECK("dojetISR_MTtoTT::initialize()", m_PRWTool->setProperty("ConfigFiles", prwVec), "");
        if (m_PRWDefaultChannel) RETURN_CHECK("dijetISR_MTtoTT::initialize()", m_PRWTool->setProperty("DefaultChannel", m_PRWDefaultChannel), "");
        RETURN_CHECK("dojetISR_MTtoTT::initialize()", m_PRWTool->setProperty("DataScaleFactor", 1.0/1.16), "");
        RETURN_CHECK("dojetISR_MTtoTT::initialize()", m_PRWTool->setProperty("DataScaleFactorUP", 1.0), "");
        RETURN_CHECK("dojetISR_MTtoTT::initialize()", m_PRWTool->setProperty("DataScaleFactorDOWN", 1.0/1.23), "");
        RETURN_CHECK("dojetISR_MTtoTT::initialize()", m_PRWTool->initialize(), "");
    }

    // XS fix for samples with no meta data XS
    if (m_mc && m_applyXSFix) {
        std::fstream f(gSystem->ExpandPathName("$ROOTCOREBIN/data/dijetISR/additional_XS.txt"));
        if (!f.good()) return EL::StatusCode::FAILURE;
        std::string line;
        while (std::getline(f, line)) {
            if (line.length() != 0 && line.find("#") != 0) {
                std::stringstream s(line);
                int runNumber;
                float XS, filterEff, kfactor, totalXS;
                s >> runNumber >> XS >> filterEff >> kfactor >> totalXS;
                m_crossSections.insert(std::pair<int, float>(runNumber, totalXS));
            }
        }
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::changeInput(bool firstFile) {
    if (firstFile) initializeOutTree();
   
    initializeInTree();
    copyMetaData();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::execute() {
    // get entry from tree
    wk()->tree()->GetEntry(wk()->treeEntry());

    // reset out tree branches
    resetBranches();
        
    out_runNumber = (m_mc) ? in_mcChannelNumber : in_runNumber;
    out_eventNumber = in_eventNumber;

    // LASER - TODO: put in PRW
    
    // GRL
    if (!m_mc && m_applyGRL) {
        if (!m_GRLTool->passRunLB(in_runNumber, in_lumiblock)) {
            return EL::StatusCode::SUCCESS;
        } 
    }

    // trigger & categorization
    int runNumber = 0;
    if (!m_mc) runNumber = in_runNumber;
    else runNumber = 900000; // LASER - TODO: handle random run number for MC, for now enforce highest triggers
    bool b_passPhotonTrigger = passPhotonTrigger(runNumber);
    bool b_passJetTrigger = passJetTrigger(runNumber);
    if (!b_passPhotonTrigger && !b_passJetTrigger) return EL::StatusCode::SUCCESS;

    // get weight
    float weight = m_mc ? in_weight : 1.;
    if (m_mc && m_applyFinalWeight) weight /= m_sumOfWeights;
    if (m_mc && m_lumi != -1.) weight *= m_lumi;
    if (m_mc && m_applyXSFix) {
        for (auto s : m_crossSections) {
            if (in_mcChannelNumber == s.first) {
                weight *= s.second;
                break;
            }
        }
    }
    out_weight = weight;

    // jet selection
    if (m_doJets && b_passJetTrigger && in_nJ > 0) {
        int iJ = 0;

        // J selection - leave all uncommented for highestPt
        
        /*
        // grab the two highest pt and take the one with the larger mass
        if (in_nJ > 1) {
            TLorentzVector tlvJ0; tlvJ0.SetPtEtaPhiE(in_ptJ->at(0), in_etaJ->at(0), in_phiJ->at(0), in_EJ->at(0));
            TLorentzVector tlvJ1; tlvJ1.SetPtEtaPhiE(in_ptJ->at(1), in_etaJ->at(1), in_phiJ->at(1), in_EJ->at(1));
            iJ = (tlvJ0.M() > tlvJ1.M()) ? 0 : 1;
        }
        */

        /*
        // grab the two highest pt and take the one with the smaller D2
        if (in_nJ > 1) iJ = (in_D2J->at(0) < in_D2J->at(1)) ? 0 : 1;
        */

        /*
        // grab the two highest pt and take the one with the smaller tau21
        if (in_nJ > 1) iJ = (in_tau21J->at(0) < in_tau21J->at(1)) ? 0 : 1;
        */

        // grab the two highest pt and take the one with smaller tau21DDT
        if (in_nJ > 1) {
            TLorentzVector tlvJ0; tlvJ0.SetPtEtaPhiE(in_ptJ->at(0), in_etaJ->at(0), in_phiJ->at(0), in_EJ->at(0));
            TLorentzVector tlvJ1; tlvJ1.SetPtEtaPhiE(in_ptJ->at(1), in_etaJ->at(1), in_phiJ->at(1), in_EJ->at(1));
            float rho0 = log(pow(tlvJ0.M(), 2) / tlvJ0.Pt());
            float rho1 = log(pow(tlvJ1.M(), 2) / tlvJ1.Pt());
            float ddt0 = in_tau21J->at(0) + 0.096605 * (rho0 - 1.5);
            float ddt1 = in_tau21J->at(1) + 0.096605 * (rho1 - 1.5);
            iJ = (ddt0 < ddt1) ? 0 : 1;
        }

        // J pt > 450
        if (!(in_ptJ->at(iJ) > 450.)) goto postj;

        // J information
        TLorentzVector tlvJ; tlvJ.SetPtEtaPhiE(in_ptJ->at(iJ), in_etaJ->at(iJ), in_phiJ->at(iJ), in_EJ->at(iJ));
        out_mJ = tlvJ.M();
        out_ptJ = in_ptJ->at(iJ);
        out_etaJ = in_etaJ->at(iJ);
        out_phiJ = in_phiJ->at(iJ);
        out_D2J = in_D2J->at(iJ);
        out_C2J = in_C2J->at(iJ);
        out_tau21J = in_tau21J->at(iJ);
        out_ptJ_ungroomed = in_ptJ_ungroomed->at(iJ);
        out_tau21J_ungroomed = in_tau21J_ungroomed->at(iJ);

        // explicit rho cut
        float rho = log(pow(tlvJ.M(), 2) / tlvJ.Pt());
        if (rho < 1.5 || rho > 5.) goto postj;
        float ddt = in_tau21J->at(iJ) + 0.096605 * (rho - 1.5);
        out_tau21JDDT = ddt;

        // constituent information
        /*
        if ((*in_constituentsJ_pt)[iJ].size() >= 2) {
            std::vector<TLorentzVector> tlvs;
            for (int i = 0; i < (*in_constituentsJ_pt)[iJ].size(); i++) {
                TLorentzVector tlv;
                tlv.SetPtEtaPhiE((*in_constituentsJ_pt)[iJ][i], (*in_constituentsJ_eta)[iJ][i], (*in_constituentsJ_phi)[iJ][i], (*in_constituentsJ_E)[iJ][i]);
                tlvs.push_back(tlv);
            }
            std::sort(tlvs.begin(), tlvs.end(), [] (TLorentzVector a, TLorentzVector b) -> bool {return (a.Pt() > b.Pt());});
            out_constituentsJ_pt0 = tlvs[0].Pt();
            out_constituentsJ_eta0 = tlvs[0].Eta();
            out_constituentsJ_phi0 = tlvs[0].Phi();
            out_constituentsJ_E0 = tlvs[0].E();
            out_constituentsJ_pt1 = tlvs[1].Pt();
            out_constituentsJ_eta1 = tlvs[1].Eta();
            out_constituentsJ_phi1 = tlvs[1].Phi();
            out_constituentsJ_E1 = tlvs[1].E();
        }
        */
        
        // j selection
        if (in_nj > 0) {
            // dPhi check (make sure we don't use the same jet for both large-R and small-R!)
            for (int ij = 0; ij < in_nj; ij++) {
                TLorentzVector tlvj; tlvj.SetPtEtaPhiE(in_ptj->at(ij), in_etaj->at(ij), in_phij->at(ij), in_Ej->at(ij));
                if (fabs(tlvJ.DeltaPhi(tlvj)) > TMath::Pi() / 2.) {
                    // j pt > 450
                    if (!(in_ptj->at(ij) > 450.)) goto postj;

                    // j information
                    out_mj = tlvj.M();
                    out_ptj = in_ptj->at(ij);
                    out_etaj = in_etaj->at(ij);
                    out_phij = in_phij->at(ij);
                    out_dEtaJj = fabs(tlvJ.Eta() - tlvj.Eta());
                    out_dPhiJj = tlvJ.DeltaPhi(tlvj);
                    out_dRJj = tlvJ.DeltaR(tlvj);

                    // got the information we need
                    break;
                }
            }
        }
        else goto postj;

        // fill tree
        out_category = _jet;
        m_outTree->Fill();
    }

postj: ;

    // photon selection
    if (m_doPhotons && b_passPhotonTrigger && in_nJ > 0) {
        // leading fat jet
        TLorentzVector tlvJ; tlvJ.SetPtEtaPhiE(in_ptJ->at(0), in_etaJ->at(0), in_phiJ->at(0), in_EJ->at(0));
        out_mJ = tlvJ.M();
        out_ptJ = in_ptJ->at(0);
        out_etaJ = in_etaJ->at(0);
        out_phiJ = in_phiJ->at(0);
        out_D2J = in_D2J->at(0);
        out_C2J = in_C2J->at(0);
        out_tau21J = in_tau21J->at(0);

        // photon
        if (in_ngamma > 0) {
            out_category = _gamma;
            out_ptgamma = in_ptgamma->at(0);
            out_etagamma = in_etagamma->at(0);
            out_phigamma = in_phigamma->at(0);
            TLorentzVector tlvgamma; tlvgamma.SetPtEtaPhiM(out_ptgamma, out_etagamma, out_phigamma, 0);
            out_dEtaJgamma = fabs(tlvJ.Eta() - tlvgamma.Eta());
            out_dPhiJgamma = tlvJ.DeltaPhi(tlvgamma);
            out_dRJgamma = tlvJ.DeltaR(tlvgamma);
        }
        
        // fill tree
        m_outTree->Fill();
    }

postgamma: ;

    return EL::StatusCode::SUCCESS;
}

void dijetISR_MTtoTT::initializeVectors() {
    in_passedTriggers = 0;
    in_EJ = 0;
    in_ptJ = 0;
    in_etaJ = 0;
    in_phiJ = 0;
    in_D2J = 0;
    in_C2J = 0;
    in_tau21J = 0;
    in_ptJ_ungroomed = 0;
    in_tau21J_ungroomed = 0;
    //in_constituentsJ_pt = 0;
    //in_constituentsJ_eta = 0;
    //in_constituentsJ_phi = 0;
    //in_constituentsJ_E = 0;
    in_ptgamma = 0;
    in_etagamma = 0;
    in_phigamma = 0;
    in_Ej = 0;
    in_ptj = 0;
    in_etaj = 0;
    in_phij = 0;
}

void dijetISR_MTtoTT::initializeInTree() {
    wk()->tree()->SetBranchAddress("runNumber", &in_runNumber);
    wk()->tree()->SetBranchAddress("mcChannelNumber", &in_mcChannelNumber);
    wk()->tree()->SetBranchAddress("eventNumber", &in_eventNumber);
    wk()->tree()->SetBranchAddress("lumiBlock", &in_lumiblock);
    wk()->tree()->SetBranchAddress("weight", &in_weight);
    wk()->tree()->SetBranchAddress("passedTriggers", &in_passedTriggers);
    wk()->tree()->SetBranchAddress("nfatjet", &in_nJ);
    wk()->tree()->SetBranchAddress("fatjet_E", &in_EJ);
    wk()->tree()->SetBranchAddress("fatjet_pt", &in_ptJ);
    wk()->tree()->SetBranchAddress("fatjet_eta", &in_etaJ);
    wk()->tree()->SetBranchAddress("fatjet_phi", &in_phiJ);
    wk()->tree()->SetBranchAddress("fatjet_D2", &in_D2J);
    wk()->tree()->SetBranchAddress("fatjet_C2", &in_C2J);
    wk()->tree()->SetBranchAddress("fatjet_tau21_wta", &in_tau21J);
    wk()->tree()->SetBranchAddress("fatjet_pt_ungroomed", &in_ptJ_ungroomed);
    wk()->tree()->SetBranchAddress("fatjet_tau21_wta_ungroomed", &in_tau21J_ungroomed);
    //wk()->tree()->SetBranchAddress("fatjet_constituent_pt", &in_constituentsJ_pt);
    //wk()->tree()->SetBranchAddress("fatjet_constituent_eta", &in_constituentsJ_eta);
    //wk()->tree()->SetBranchAddress("fatjet_constituent_phi", &in_constituentsJ_phi);
    //wk()->tree()->SetBranchAddress("fatjet_constituent_e", &in_constituentsJ_E);
    if (m_doJets) {
        wk()->tree()->SetBranchAddress("njets", &in_nj);
        wk()->tree()->SetBranchAddress("jet_E", &in_Ej);
        wk()->tree()->SetBranchAddress("jet_pt", &in_ptj);
        wk()->tree()->SetBranchAddress("jet_eta", &in_etaj);
        wk()->tree()->SetBranchAddress("jet_phi", &in_phij);
    }
    if (m_doPhotons) {
        wk()->tree()->SetBranchAddress("nph", &in_ngamma);
        wk()->tree()->SetBranchAddress("ph_pt", &in_ptgamma);
        wk()->tree()->SetBranchAddress("ph_eta", &in_etagamma);
        wk()->tree()->SetBranchAddress("ph_phi", &in_phigamma);
    }
}

void dijetISR_MTtoTT::initializeOutTree() {
    m_outTree = new TTree("TinyTree", "TinyTree");
    
    m_outTree->Branch("runNumber", &out_runNumber, "runNumber/I");
    m_outTree->Branch("eventNumber", &out_eventNumber, "eventNumber/LI");
    m_outTree->Branch("category", &out_category, "category/I");
    m_outTree->Branch("weight", &out_weight, "weight/F");
    m_outTree->Branch("mJ", &out_mJ, "mJ/F");
    m_outTree->Branch("ptJ", &out_ptJ, "ptJ/F");
    m_outTree->Branch("etaJ", &out_etaJ, "etaJ/F");
    m_outTree->Branch("phiJ", &out_phiJ, "phiJ/F");
    m_outTree->Branch("D2J", &out_D2J, "D2J/F");
    m_outTree->Branch("C2J", &out_C2J, "C2J/F");
    m_outTree->Branch("tau21J", &out_tau21J, "tau21J/F");
    m_outTree->Branch("ptJ_ungroomed", &out_ptJ_ungroomed, "ptJ_ungroomed/F");
    m_outTree->Branch("tau21J_ungroomed", &out_tau21J_ungroomed, "tau21J_ungroomed/F");
    m_outTree->Branch("tau21JDDT", &out_tau21JDDT, "tau21JDDT/F");
    //m_outTree->Branch("constituentsJ_pt0", &out_constituentsJ_pt0, "constituentsJ_pt0/F");
    //m_outTree->Branch("constituentsJ_eta0", &out_constituentsJ_eta0, "constituentsJ_eta0/F");
    //m_outTree->Branch("constituentsJ_phi0", &out_constituentsJ_phi0, "constituentsJ_phi0/F");
    //m_outTree->Branch("constituentsJ_E0", &out_constituentsJ_E0, "constituentsJ_E0/F");
    //m_outTree->Branch("constituentsJ_pt1", &out_constituentsJ_pt1, "constituentsJ_pt1/F");
    //m_outTree->Branch("constituentsJ_eta1", &out_constituentsJ_eta1, "constituentsJ_eta1/F");
    //m_outTree->Branch("constituentsJ_phi1", &out_constituentsJ_phi1, "constituentsJ_phi1/F");
    //m_outTree->Branch("constituentsJ_E1", &out_constituentsJ_E1, "constituentsJ_E1/F");
    if (m_doJets) {
        m_outTree->Branch("mj", &out_mj, "mj/F");
        m_outTree->Branch("ptj", &out_ptj, "ptj/F");
        m_outTree->Branch("etaj", &out_etaj, "etaj/F");
        m_outTree->Branch("phij", &out_phij, "phij/F");
        m_outTree->Branch("dEtaJj", &out_dEtaJj, "dEtaJj/F");
        m_outTree->Branch("dPhiJj", &out_dPhiJj, "dPhiJj/F");
        m_outTree->Branch("dRJj", &out_dRJj, "dRJj/F");
    }
    if (m_doPhotons) {
        m_outTree->Branch("ptgamma", &out_ptgamma, "ptgamma/F");
        m_outTree->Branch("etagamma", &out_etagamma, "etagamma/F");
        m_outTree->Branch("phigamma", &out_phigamma, "phigamma/F");
        m_outTree->Branch("dEtaJgamma", &out_dEtaJgamma, "dEtaJgamma/F");
        m_outTree->Branch("dPhiJgamma", &out_dPhiJgamma, "dPhiJgamma/F");
        m_outTree->Branch("dRJgamma", &out_dRJgamma, "dRJgamma/F");
    }

    wk()->addOutput(m_outTree);
}

void dijetISR_MTtoTT::copyMetaData() {
    if (!m_metaData) {
        m_metaData = new TH1F("MetaData_TinyTree", "MetaData_TinyTree", 1, 0.5, 1.5);
        wk()->addOutput(m_metaData);
    }

    TH1F *md = (TH1F*) wk()->inputFile()->Get("MetaData");
    m_metaData->Fill(1, md->GetBinContent(3));
}

void dijetISR_MTtoTT::setSumOfWeights() {
    TH1F *md = (TH1F*) wk()->inputFile()->Get("MetaData");
    m_sumOfWeights = md->GetBinContent(3);
}

void dijetISR_MTtoTT::resetBranches() {
        out_runNumber = -999;
        out_eventNumber = -999;
        out_category = _none;
        out_weight = -999;
        out_mJ = -999;
        out_ptJ = -999;
        out_etaJ = -999;
        out_phiJ = -999;
        out_D2J = -999;
        out_C2J = -999;
        out_tau21J = -999;
        out_ptJ_ungroomed = -999;
        out_tau21J_ungroomed = -999;
        out_tau21JDDT = -999;
        //out_constituentsJ_pt0 = -999;
        //out_constituentsJ_eta0 = -999;
        //out_constituentsJ_phi0 = -999;
        //out_constituentsJ_E0 = -999;
        //out_constituentsJ_pt1 = -999;
        //out_constituentsJ_eta1 = -999;
        //out_constituentsJ_phi1 = -999;
        //out_constituentsJ_E1 = -999;
        out_ptgamma = -999;
        out_etagamma = -999;
        out_phigamma = -999;
        out_mj = -999;
        out_ptj = -999;
        out_etaj = -999;
        out_phij = -999;
        out_dEtaJgamma = -999;
        out_dPhiJgamma = -999;
        out_dRJgamma = -999;
        out_dEtaJj = -999;
        out_dPhiJj = -999;
        out_dRJj = -999;
}

bool dijetISR_MTtoTT::passPhotonTrigger(int runNumber) {
    // 2015
    if (runNumber <= 284484) {
        for (auto t : *in_passedTriggers) {
            if (t == "HLT_g120_loose" || t == "HLT_g200_etcut") return true;
        }
    }
    // 2016
    else {
        for (auto t : *in_passedTriggers) {
            if (t == "HLT_g140_loose" || t == "HLT_g300_etcut") return true;
        }
    }

    // didn't find a passed trigger
    return false;
}

bool dijetISR_MTtoTT::passJetTrigger(int runNumber) {
    // 2015
    if (runNumber <= 284484) {
        for (auto t : *in_passedTriggers) {
            if (t == "HLT_j360" || t == "HLT_j360_a10r_L1J100" || t == "HLT_j360_a10_sub_L1J100" || t == "HLT_ht700_L1J75") return true;
        }
    }
    // 2016 period A
    else if (runNumber >= 296939 && runNumber <= 300287) {
        for (auto t : *in_passedTriggers) {
            if (t == "HLT_j340" || t == "HLT_j360_a10r_L1J100" || t == "HLT_j360_a10_lcw_L1J100" || t == "HLT_ht700_L1J75") return true;
        }
    }
    // 2016 period B-E
    else if (runNumber >= 300345 && runNumber <= 303892) {
        for (auto t : *in_passedTriggers) {
            if (t == "HLT_j380" || t == "HLT_j400_a10r_L1J100" || t == "HLT_j400_a10_lcw_L1J100" || t == "HLT_ht1000" || t == "HLT_ht1000_L1J100") return true;
        }
    }
    // 2016 period F-
    else if (runNumber >= 303943) {
        for (auto t : *in_passedTriggers) {
            if (t == "HLT_j380" || t == "HLT_j420_a10r_L1J100" || t == "HLT_j420_a10_lcw_L1J100" || t == "HLT_ht1000" || t == "HLT_ht1000_L1J100") return true;
        }
    }

    // didn't find a passed trigger
    return false;
}
