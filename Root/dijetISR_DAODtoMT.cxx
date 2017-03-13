#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

#include <SampleHandler/MetaFields.h>

// Infrastructure include(s)
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// EDM include(s)
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCore/AuxContainerBase.h"

// ROOT include(s)
#include "TH1.h"

// Package include(s)
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
    if (m_doJets == m_doPhotons) {
        std::cout << "Must select only one of m_doJets or m_doPhotons!" << std::endl;
        return EL::StatusCode::FAILURE;
    }

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::addTree(std::string name) {
  
    Info("addTree()", "Adding tree '%s'.", name.c_str());

    TTree *outTree = new TTree(name.c_str(), name.c_str());
    TFile *treeFile = wk()->getOutputFile("MiniTree");
    outTree->SetDirectory(treeFile);

    dijetISR_MiniTree *tree = new dijetISR_MiniTree(m_event, outTree, treeFile);
    tree->AddEvent(m_eventInfoDetailStr);
    tree->AddTrigger(m_trigDetailStr);
    tree->AddFatJets(m_fatJetDetailStr);
    if (m_doJets)    tree->AddJets(m_jetDetailStr);
    if (m_doPhotons) tree->AddPhotons(m_photonDetailStr);
    
    // Store reference to systematics tree
    m_trees[name] = tree;

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::execute() {

    // Run nominal process (done for all inputs, regardless of systematics configuration)
    executeSingle("Nominal", SystType::Nominal);

    // Loop large-radius jet sysetmatics
    std::vector<std::string> *fatJetSysNames = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::initializeTrees()", HelperFunctions::retrieve(fatJetSysNames, m_fatJetContainerName + "_Algo", 0, m_store), "");
    for (std::string &name : *fatJetSysNames) {
        if (name.empty()) { continue; }
	executeSingle(name, SystType::LargeRadius);
    }

    if (m_doJets) {
        // Loop small-radius jet systematics
        std::vector<std::string> *jetSysNames = 0;
        RETURN_CHECK("dijetISR_DAODtoMT::initializeTrees()", HelperFunctions::retrieve(jetSysNames, m_jetContainerName + "_Algo", 0, m_store), "");
        for (std::string &name : *jetSysNames) {
	    if (name.empty()) { continue; }
	    executeSingle(name, SystType::SmallRadius);
        }
    }

    if (m_doPhotons) {
        // Loop photon systematics
        std::vector<std::string> *photonSysNames = 0;
        RETURN_CHECK("dijetISR_DAODtoMT::initializeTrees()", HelperFunctions::retrieve(photonSysNames, m_photonContainerName + "_Algo", 0, m_store), "");
        for (std::string &name : *photonSysNames) {
	    if (name.empty()) { continue; }
	    executeSingle(name, SystType::Photon);
        }
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::executeSingle(const std::string& systName, const SystType& systType) {

    // Make sure the output tree exists for the current systematic
  if (m_trees.count(s_treeName(systName, systType)) == 0) { addTree(s_treeName(systName, systType)); }

     // Retrieve containers
    // -------------------------------------------------------------------------

     // Get event info
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    const xAOD::EventInfo *eventInfo = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store), "");

    // Compute event weight
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

     // Get large-radius jets
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    const xAOD::JetContainer *fatJets = 0;
    RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(fatJets, m_fatJetContainerName + (systType == SystType::LargeRadius ? systName : ""), m_event, m_store), "");

    xAOD::JetContainer*     goodFatJets    = nullptr;
    xAOD::AuxContainerBase* goodFatJetsAux = nullptr;

     // Get small-radius jets
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    const xAOD::JetContainer *jets = 0;
    if (m_doJets) { RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(jets, m_jetContainerName + (systType == SystType::SmallRadius ? systName : ""), m_event, m_store), ""); }

     // Get photons
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    const xAOD::PhotonContainer *photons = 0;
    if (m_doPhotons) { RETURN_CHECK("dijetISR_DAODtoMT::execute()", HelperFunctions::retrieve(photons, m_photonContainerName + (systType == SystType::Photon ? systName : ""), m_event, m_store), ""); }


     // Perform reductions
    // -------------------------------------------------------------------------

     // ISR jet
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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


     // ISR photon
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (m_doPhotons) {

      // Perform simple "overlap removal" of large-radius jets wrt. photons
      // -- Create the new container and its auxiliary store.
      goodFatJets    = new xAOD::JetContainer();
      goodFatJetsAux = new xAOD::AuxContainerBase();
      goodFatJets->setStore( goodFatJetsAux ); //< Connect the two

      // -- Define photon-fatjet overlap criterium
      auto overlap = [](const xAOD::IParticle* p1, const xAOD::IParticle* p2) -> bool {
        return p1->p4().DeltaR(p2->p4()) < 1.0;
      };

      // -- Fill deep copy container only if OR criterium is satisfied
      for (const xAOD::Jet* jet : *fatJets) {
        bool keep = true;
        for (const xAOD::Photon* photon : *photons) {
          if (overlap(photon, jet)) { keep = false; break; }
        }
        if (!keep) { continue; }

        // Copy this jet to the output container:
	xAOD::Jet* goodJet = new xAOD::Jet();
        goodFatJets->push_back( goodJet ); // jet acquires the goodFatJets auxstore
        *goodJet = *jet; // copies auxdata from one auxstore to the other
      }

      // Skim: Requre at least one good fat jet.
      if (goodFatJets->size() == 0) { return EL::StatusCode::SUCCESS; }

      // Skim: Require at exactly one good photon.
      if (photons->size() != 1) { return EL::StatusCode::SUCCESS; }

    }


     // Fill output
    // -------------------------------------------------------------------------

    dijetISR_MiniTree *tree = m_trees[s_treeName(systName, systType)];

    if (!tree) {
      Info("execute()" "Tree is nullptr for systematic '%s'.", systName.c_str());
        return EL::StatusCode::FAILURE;
    }
    tree->FillEvent  (eventInfo, m_event);
    tree->FillTrigger(eventInfo);
    tree->FillFatJets(fatJets);
    if (m_doJets)    { tree->FillJets(jets); }
    if (m_doPhotons) { tree->FillPhotons(photons); }
    tree->Fill();


     // Clean up
    // -------------------------------------------------------------------------

    if (m_doPhotons) {
      delete goodFatJets;    goodFatJets    = nullptr;
      delete goodFatJetsAux; goodFatJetsAux = nullptr;
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode dijetISR_DAODtoMT::histFinalize() {

    RETURN_CHECK("dijetISR_DAODtoMT::histFinalize()", xAH::Algorithm::algFinalize(), "");

    // copy metadata to output minitree
    TFile *fileMD = wk()->getOutputFile("metadata");
    TH1D  *histEventCount = (TH1D*) fileMD->Get("MetaData_EventCount");
    TFile *treeFile = wk()->getOutputFile("MiniTree");
    TH1F  *thisHistEventCount = (TH1F*) histEventCount->Clone("MetaData");
    thisHistEventCount->SetDirectory(treeFile);

    return EL::StatusCode::SUCCESS;
}
