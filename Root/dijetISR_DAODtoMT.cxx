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

    EL::OutputStream outTree("Minitree");
    job.outputAdd(outTree);

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::initialize() {
    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    TTree *outTree = new TTree("tree", "tree");
    TFile *treeFile = wk()->getOutputFile("Minitree");
    outTree->SetDirectory(treeFile);
    
    m_tree = new dijetISR_Minitree(m_event, outTree, treeFile);
    m_tree->AddEvent(m_eventInfoDetailStr);
    m_tree->AddTrigger(m_trigDetailStr);
    m_tree->AddFatJets(m_fatJetDetailStr, "signal");
    m_tree->AddJets(m_jetDetailStr);
    m_tree->AddPhotons(m_photonDetailStr);

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
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(jets, m_jetContainerName, m_event, m_store), "");

    // get photons
    const xAOD::PhotonContainer *photons = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(photons, m_photonContainerName, m_event, m_store), "");

    // fill tree branches
    m_tree->FillEvent(eventInfo, m_event);
    m_tree->FillTrigger(eventInfo);
    m_tree->FillFatJets(fatJets, "signal");
    m_tree->FillJets(jets);
    m_tree->FillPhotons(photons);
    m_tree->Fill();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::histFinalize() {
    RETURN_CHECK("dijetISR_DAODtoMT::histFinalize()", xAH::Algorithm::algFinalize(), "");

    // copy metadata to output minitree
    TFile *fileMD = wk()->getOutputFile("metadata");
    TH1D *histEventCount = (TH1D*) fileMD->Get("MetaData_EventCount");
    TFile *treeFile = wk()->getOutputFile("Minitree");
    TH1F *thisHistEventCount = (TH1F*) histEventCount->Clone("MetaData");
    thisHistEventCount->SetDirectory(treeFile);

    return EL::StatusCode::SUCCESS;
}
