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

dijetISR_MTtoTT::dijetISR_MTtoTT() : 
    in_mJ(0),
    in_ptJ(0),
    in_ptgamma(0),
    m_outTree(nullptr) {
}

EL::StatusCode dijetISR_MTtoTT::setupJob(EL::Job& /*job*/) {
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::initialize() {
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::changeInput(bool firstFile) {
    if (firstFile) initializeOutTree();
   
    initializeInTree();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::execute() {
    wk()->tree()->GetEntry(wk()->treeEntry());

    if (in_nJ > 0) {
        out_runNumber = in_runNumber;
        out_eventNumber = in_eventNumber;
        out_mJ = in_mJ->at(0);
        out_weight = in_weight;
        m_outTree->Fill();
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_MTtoTT::finalize() {
    return EL::StatusCode::SUCCESS;
}
    
void dijetISR_MTtoTT::initializeInTree() {
    wk()->tree()->SetBranchAddress("runNumber", &in_runNumber);
    wk()->tree()->SetBranchAddress("eventNumber", &in_eventNumber);
    wk()->tree()->SetBranchAddress("weight", &in_weight);
    wk()->tree()->SetBranchAddress("nfatjets", &in_nJ);
    wk()->tree()->SetBranchAddress("fatjet_m", &in_mJ);
    wk()->tree()->SetBranchAddress("fatjet_pt", &in_ptJ);
    wk()->tree()->SetBranchAddress("nph", &in_ngamma);
    wk()->tree()->SetBranchAddress("ph_pt", &in_ptgamma);
}

void dijetISR_MTtoTT::initializeOutTree() {
    m_outTree = new TTree("tree", "tree");
    
    m_outTree->Branch("runNumber", &out_runNumber, "runNumber/I");
    m_outTree->Branch("eventNumber", &out_eventNumber, "eventNumber/LI");
    m_outTree->Branch("mJ", &out_mJ, "mJ/F");
    m_outTree->Branch("weight", &out_weight, "weight/F");

    wk()->addOutput(m_outTree);
}
