#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <utility>      
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TSystem.h"

#include <dijetISR/dijetISR_MTtoTT.h>

ClassImp(dijetISR_MTtoTT)

dijetISR_MTtoTT::dijetISR_MTtoTT() {
    // options
    m_applyGRL = false;
    m_GRLs = "";
    m_applyTrigger = false;
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

    // initalize vectors to 0
    initializeVectors();
}

EL::StatusCode dijetISR_MTtoTT::initialize() {
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

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::changeInput(bool firstFile) {
    if (firstFile) initializeOutTree();
   
    initializeInTree();
    copyMetaData();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::execute() {
    wk()->tree()->GetEntry(wk()->treeEntry());

    // LASER - TODO: put in PRW
    
    // GRL
    if (!m_isMC && m_applyGRL) {
        if (!m_GRLTool->passRunLB(in_runNumber, in_lumiblock)) {
            wk()->skipEvent();
            return EL::StatusCode::SUCCESS;
        } 
    }

    if (in_nJ > 0) {
        out_runNumber = in_runNumber;
        out_eventNumber = in_eventNumber;
        out_mJ = in_mJ->at(0);
        out_weight = in_weight;
        m_outTree->Fill();
    }

    return EL::StatusCode::SUCCESS;
}

void dijetISR_MTtoTT::initializeVectors() {
    in_mJ = 0;
    in_ptJ = 0;
    in_etaJ = 0;
    in_D2J = 0;
    in_C2J = 0;
    in_tau21J = 0;
    in_ptj = 0;
    in_etaj = 0;
    in_ptgamma = 0;
    in_etagamma = 0;
}

void dijetISR_MTtoTT::initializeInTree() {
    wk()->tree()->SetBranchAddress("runNumber", &in_runNumber);
    wk()->tree()->SetBranchAddress("eventNumber", &in_eventNumber);
    wk()->tree()->SetBranchAddress("lumiBlock", &in_lumiblock);
    wk()->tree()->SetBranchAddress("weight", &in_weight);
    wk()->tree()->SetBranchAddress("nfatjetssignal", &in_nJ);
    wk()->tree()->SetBranchAddress("fatjet_m_signal", &in_mJ);
    wk()->tree()->SetBranchAddress("fatjet_pt_signal", &in_ptJ);
    wk()->tree()->SetBranchAddress("nph", &in_ngamma);
    wk()->tree()->SetBranchAddress("ph_pt", &in_ptgamma);
}

void dijetISR_MTtoTT::initializeOutTree() {
    m_outTree = new TTree("TinyTree", "TinyTree");
    
    m_outTree->Branch("runNumber", &out_runNumber, "runNumber/I");
    m_outTree->Branch("eventNumber", &out_eventNumber, "eventNumber/LI");
    m_outTree->Branch("mJ", &out_mJ, "mJ/F");
    m_outTree->Branch("weight", &out_weight, "weight/F");

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
