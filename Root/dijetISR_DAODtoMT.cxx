#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

#include <SampleHandler/MetaFields.h>

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "TH1.h"

#include <dijetISR/dijetISR_DAODtoMT.h>

ClassImp(dijetISR_DAODtoMT)

dijetISR_DAODtoMT::dijetISR_DAODtoMT() {
    m_doJets = false;
    m_doPhotons = false;
    m_fatJetContainerName = "";
    m_jetContainerName = "";
    m_photonContainerName = "";
    m_eventInfoDetailStr = "";
    m_trigDetailStr = "";
    m_fatJetDetailStr = "";
    m_jetDetailStr = "";
    m_photonDetailStr = "";
}

EL::StatusCode dijetISR_DAODtoMT::setupJob(EL::Job& job) {
    job.useXAOD();
    xAOD::Init("dijetISR_DAODtoMT").ignore();

    EL::OutputStream outTree("MiniTree");
    job.outputAdd(outTree);

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::initialize() {
    if (!m_doJets && ! m_doPhotons) {
        std::cout << "Must select at least one of m_doJets or m_doPhotons!" << std::endl;
        return EL::StatusCode::FAILURE;
    }

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    TTree *outTree = new TTree("MiniTree", "MiniTree");
    TFile *treeFile = wk()->getOutputFile("MiniTree");
    outTree->SetDirectory(treeFile);
    
    m_tree = new dijetISR_MiniTree(m_event, outTree, treeFile);
    m_tree->AddEvent(m_eventInfoDetailStr);
    m_tree->AddTrigger(m_trigDetailStr);
    m_tree->AddFatJets(m_fatJetDetailStr);
    if (m_doJets) m_tree->AddJets(m_jetDetailStr);
    if (m_doPhotons) m_tree->AddPhotons(m_photonDetailStr);

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::execute() {
    // get event info
    const xAOD::EventInfo *eventInfo = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store), "");

    // event weight
    if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
        float mcEventWeight = eventInfo->mcEventWeight();
        double xs = wk()->metaData()->castDouble(SH::MetaFields::crossSection, 1);
        double eff = wk()->metaData()->castDouble(SH::MetaFields::filterEfficiency, 1);
        double kfactor = wk()->metaData()->castDouble(SH::MetaFields::kfactor, 1);
        eventInfo->auxdecor<float>("weight") = mcEventWeight * xs * eff * kfactor;
    }
    else {
        eventInfo->auxdecor<float>("weight") = 1.;
    }

    // get fat jets
    const xAOD::JetContainer *fatJets = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(fatJets, m_fatJetContainerName, m_event, m_store), "");

    // get jets
    const xAOD::JetContainer *jets = 0;
    if (m_doJets) RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(jets, m_jetContainerName, m_event, m_store), "");

    // get photons
    const xAOD::PhotonContainer *photons = 0;
    if (m_doPhotons) RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(photons, m_photonContainerName, m_event, m_store), "");

    // reduction
    if (m_doJets) {
        // jets: at least 1 fat jet, leading fat jet pt > 400 GeV, at least 1 small jet not the fat jet, leading small R jet not the fat jet pt > 400 GeV
        int nfatjets = fatJets->size();
        if (nfatjets < 1) return EL::StatusCode::SUCCESS;
        const xAOD::Jet *leadfj = fatJets->at(0);
        if (leadfj->pt() / 1000. < 400.) return EL::StatusCode::SUCCESS;
        int njets = 0;
        const xAOD::Jet *leadj = 0;
        for (auto j : *jets) {
            if (j->p4().DeltaR(leadfj->p4()) > 1.) {
                njets++;
                if (!leadj) leadj = j;
            }
        }
        if (njets < 1) return EL::StatusCode::SUCCESS;
        if (leadj->pt() / 1000. < 400.) return EL::StatusCode::SUCCESS;
    }
    if (m_doPhotons) {
        // photons: at least 1 fat jet, at least 1 photon, leading photon pt > 100 GeV
        int nfatjets = fatJets->size();
        if (nfatjets < 1) return EL::StatusCode::SUCCESS;
        int nphotons = photons->size();
        if (nphotons < 1) return EL::StatusCode::SUCCESS;
        const xAOD::Photon *p = photons->at(0);
        if (p->pt() / 1000. < 100.) return EL::StatusCode::SUCCESS;
    }

    // fill tree branches
    m_tree->FillEvent(eventInfo, m_event);
    m_tree->FillTrigger(eventInfo);
    m_tree->FillFatJets(fatJets);
    if (m_doJets) m_tree->FillJets(jets);
    if (m_doPhotons) m_tree->FillPhotons(photons);
    m_tree->Fill();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::histFinalize() {
    RETURN_CHECK("dijetISR_DAODtoMT::histFinalize()", xAH::Algorithm::algFinalize(), "");

    // copy metadata to output minitree
    TFile *fileMD = wk()->getOutputFile("metadata");
    TH1D *histEventCount = (TH1D*) fileMD->Get("MetaData_EventCount");
    TFile *treeFile = wk()->getOutputFile("MiniTree");
    TH1F *thisHistEventCount = (TH1F*) histEventCount->Clone("MetaData");
    thisHistEventCount->SetDirectory(treeFile);

    return EL::StatusCode::SUCCESS;
}
