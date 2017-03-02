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
    if ((!m_doJets && ! m_doPhotons) || (m_doJets && m_doPhotons)) {
        std::cout << "Must select only one of m_doJets or m_doPhotons!" << std::endl;
        return EL::StatusCode::FAILURE;
    }

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::addTree(std::string name) {
    std::string treeName = (name == "Nominal") ? "MiniTree" : "MiniTree_" + name;
    TTree *outTree = new TTree(treeName.c_str(), treeName.c_str());
    TFile *treeFile = wk()->getOutputFile("MiniTree");
    outTree->SetDirectory(treeFile);

    dijetISR_MiniTree *tree = new dijetISR_MiniTree(m_event, outTree, treeFile);
    tree->AddEvent(m_eventInfoDetailStr);
    tree->AddTrigger(m_trigDetailStr);
    tree->AddFatJets(m_fatJetDetailStr);
    if (m_doJets) tree->AddJets(m_jetDetailStr);
    if (m_doPhotons) tree->AddPhotons(m_photonDetailStr);

    m_trees[name] = tree;

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::execute() {
    if (!(m_trees["Nominal"])) addTree("Nominal");
    executeSingle();

    std::vector<std::string> *fatJetSysNames = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::initializeTrees()", HelperFunctions::retrieve(fatJetSysNames, m_fatJetContainerName + "_Algo", 0, m_store), "");
    for (std::string &name : *fatJetSysNames) {
        if (name.empty()) continue;
        if (!(m_trees[name])) addTree(name);
        executeSingle("", name);
    }

    if (m_doJets) {
        std::vector<std::string> *jetSysNames = 0;
        RETURN_CHECK("dijetISR_DAODtoMT::initializeTrees()", HelperFunctions::retrieve(jetSysNames, m_jetContainerName + "_Algo", 0, m_store), "");
        for (std::string &name : *jetSysNames) {
            if (name.empty()) continue;
            if (!(m_trees[name])) addTree(name);
            executeSingle(name, "");
        }
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::executeSingle(std::string resolvedSys, std::string boostedSys) {
    // failsafe
    if (resolvedSys != "" && boostedSys != "") {
        std::cout << "Both resolvedSys and boostedSys are non-empty! WTF?!?" << std::endl;
        return EL::StatusCode::FAILURE;
    }

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
    std::string fatJetContName = m_fatJetContainerName;
    if (boostedSys != "") fatJetContName += boostedSys;
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(fatJets, fatJetContName, m_event, m_store), "");

    // get jets
    const xAOD::JetContainer *jets = 0;
    std::string jetContName = m_jetContainerName;
    if (resolvedSys != "") jetContName += resolvedSys;
    if (m_doJets) RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(jets, jetContName, m_event, m_store), "");

    // get photons
    const xAOD::PhotonContainer *photons = 0;
    if (m_doPhotons) RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(photons, m_photonContainerName, m_event, m_store), "");

    // reduction
    if (m_doJets) {
        bool b1 = false, b2 = false;
        int nfatjets = fatJets->size();
        
        // case 1: leading fat jet is resonance
        // leading fat jet pt > 400 GeV, at least 1 small jet not the fat jet, leading small R jet not the fat jet pt > 400 GeV
        if (nfatjets > 0) {
            const xAOD::Jet *res = fatJets->at(0);
            const xAOD::Jet *isr = 0;
            for (auto j : *jets) {
                if (j->p4().DeltaR(res->p4()) > 1.) {
                    isr = j;
                    break;
                }
            }
            if (isr) {
                if (res->pt() / 1000. > 400. && isr->pt() / 1000. > 400.) b1 = true;
            }
        }

        // case 2: subleading fat jet is resonance
        // NOT case1, subleading fat jet pt > 400 GeV, at least 1 small jet not the fat jet, leading small R jet not the fat jet pt > 400 GeV
        if (!b1 && nfatjets > 1) {
            const xAOD::Jet *res = fatJets->at(1);
            const xAOD::Jet *isr = 0;
            for (auto j : *jets) {
                if (j->p4().DeltaR(res->p4()) > 1.) {
                    isr = j;
                    break;
                }
            }
            if (isr) {
                if (res->pt() / 1000. > 400. && isr->pt() / 1000. > 400.) b2 = true;
            }
        }

        // check cases
        if (!b1 && !b2) return EL::StatusCode::SUCCESS;
    }
    if (m_doPhotons) {
        bool b1 = false, b2 = false;
        int nfatjets = fatJets->size();
        
        // case 1: leading fat jet is resonance
        // leading fat jet pt > 400 GeV, at least 1 photon not the fat jet, leading photon not the fat jet pt > 100 GeV
        if (nfatjets > 0) {
            const xAOD::Jet *res = fatJets->at(0);
            const xAOD::Photon *isr = 0;
            for (auto p : *photons) {
                if (p->p4().DeltaR(res->p4()) > 1.) {
                    isr = p;
                    break;
                }
            }
            if (isr) {
                if (res->pt() / 1000. > 400. && isr->pt() / 1000. > 100.) b1 = true;
            }
        }

        // case 2: subleading fat jet is resonance
        // NOT case1, subleading fat jet pt > 400 GeV, at least 1 photon not the fat jet, leading photon not the fat jet pt > 100 GeV
        if (!b1 && nfatjets > 1) {
            const xAOD::Jet *res = fatJets->at(1);
            const xAOD::Photon *isr = 0;
            for (auto p : *photons) {
                if (p->p4().DeltaR(res->p4()) > 1.) {
                    isr = p;
                    break;
                }
            }
            if (isr) {
                if (res->pt() / 1000. > 400. && isr->pt() / 1000. > 100.) b2 = true;
            }
        }

        // check cases
        if (!b1 && !b2) return EL::StatusCode::SUCCESS;
    }

    // fill tree branches
    dijetISR_MiniTree *tree = m_trees["Nominal"];
    if (resolvedSys != "") tree = m_trees[resolvedSys];
    if (boostedSys != "") tree = m_trees[boostedSys];
    if (!tree) {
        std::cout << "tree is nullptr!  WTF?!?" << std::endl;
        return EL::StatusCode::FAILURE;
    }
    tree->FillEvent(eventInfo, m_event);
    tree->FillTrigger(eventInfo);
    tree->FillFatJets(fatJets);
    if (m_doJets) tree->FillJets(jets);
    if (m_doPhotons) tree->FillPhotons(photons);
    tree->Fill();

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
