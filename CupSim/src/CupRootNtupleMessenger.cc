
#include "CupSim/CupRootNtupleMessenger.hh"
#include "CupSim/CupRootNtuple.hh"

#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "globals.hh"

#include <cstdlib> // for strtol
#include <fstream> // for file streams
#include <sstream>

CupRootNtupleMessenger::CupRootNtupleMessenger(CupRootNtuple *myntuple) : myNtuple(myntuple) {
    // the CupRootNtuple directory
    RootNtupleDir = new G4UIdirectory("/ntuple/");
    RootNtupleDir->SetGuidance("Control the detector geometry options.");

    // select Primary
    NtuplePrimary = new G4UIcommand("/ntuple/primary", this);
    NtuplePrimary->SetGuidance("Select on/off for primary to be included in the ntuples");

    NtuplePrimary->AvailableForStates(G4State_PreInit);
    NtuplePrimary->SetParameter(new G4UIparameter("track", 'd', true));

    // select Track
    NtupleTrack = new G4UIcommand("/ntuple/track", this);
    NtupleTrack->SetGuidance("Select on/off for track to be included in the ntuples");

    NtupleTrack->AvailableForStates(G4State_PreInit);
    NtupleTrack->SetParameter(new G4UIparameter("track", 'd', true));

    // select Step
    NtupleStep = new G4UIcommand("/ntuple/step", this);
    NtupleStep->SetGuidance("Select on/off for track to be included in the ntuples");

    NtupleStep->AvailableForStates(G4State_PreInit);
    NtupleStep->SetParameter(new G4UIparameter("step", 'd', true));

    // select Photon
    NtuplePhoton = new G4UIcommand("/ntuple/photon", this);
    NtuplePhoton->SetGuidance("Select on/off for track to be included in the ntuples");

    NtuplePhoton->AvailableForStates(G4State_PreInit);
    NtuplePhoton->SetParameter(new G4UIparameter("photon", 'd', true));

    // select Scint
    NtupleScint = new G4UIcommand("/ntuple/scint", this);
    NtupleScint->SetGuidance("Select on/off for track to be included in the ntuples");

    NtupleScint->AvailableForStates(G4State_PreInit);
    NtupleScint->SetParameter(new G4UIparameter("scint", 's', true));

    NtupleMuon = new G4UIcommand("/ntuple/muon", this);
    NtupleMuon->SetGuidance(
        "Select on/off for muon scintillator information to be included in the ntuples");
    NtupleMuon->AvailableForStates(G4State_PreInit);
    NtupleMuon->SetParameter(new G4UIparameter("muon", 's', true));
}

CupRootNtupleMessenger::~CupRootNtupleMessenger() {
    delete NtuplePrimary;
    delete NtupleTrack;
    delete NtupleStep;
    delete NtuplePhoton;
    delete NtupleScint;
    delete NtupleMuon;

    delete RootNtupleDir;
}

void CupRootNtupleMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
    if (command == NtuplePrimary) {
        std::istringstream is((const char *)newValues);
        int index = -1;
        is >> index;
        if (is.fail()) {
            G4cerr << "/ntuple/primary: failed to read" << G4endl;
            return;
        }
        if (index < 0) {
            G4cerr << "/ntuple/primary: invalid value: arguments are \"" << newValues << "\""
                   << G4endl;
            return;
        }
        myNtuple->SetNtuplePrimary(index);
    } else if (command == NtupleTrack) {
        std::istringstream is((const char *)newValues);
        int index = -1;
        is >> index;
        if (is.fail()) {
            G4cerr << "/ntuple/track: failed to read" << G4endl;
        }
        if (index < 0) {
            G4cerr << "/ntuple/track: invalid value: arguments are \"" << newValues << "\""
                   << G4endl;
            return;
        }
        myNtuple->SetNtupleTrack(index);
    } else if (command == NtupleStep) {
        std::istringstream is((const char *)newValues);
        int index = -1;
        is >> index;
        if (is.fail()) {
            G4cerr << "/ntuple/track: failed to read" << G4endl;
        }
        if (index < 0) {
            G4cerr << "/ntuple/track: invalid value: arguments are \"" << newValues << "\""
                   << G4endl;
            return;
        }
        myNtuple->SetNtupleStep(index);
    } else if (command == NtuplePhoton) {
        std::istringstream is((const char *)newValues);
        int index = -1;
        is >> index;
        if (is.fail()) {
            G4cerr << "/ntuple/photon: failed to read" << G4endl;
        }
        if (index < 0) {
            G4cerr << "/ntuple/photon: invalid value: arguments are \"" << newValues << "\""
                   << G4endl;
            return;
        }
        myNtuple->SetNtuplePhoton(index);
    } else if (command == NtupleScint) {
        std::istringstream is((const char *)newValues);
        int index = -1;
        is >> index;
        if (is.fail()) {
            G4cerr << "/ntuple/scint: failed to read" << G4endl;
        }
        if (index < 0) {
            G4cerr << "/ntuple/scint: invalid value: arguments are \"" << newValues << "\""
                   << G4endl;
            return;
        }
        myNtuple->SetNtupleScint(index);
    }

    else if (command == NtupleMuon) {
        std::istringstream is((const char *)newValues);
        int index = -1;
        is >> index;
        if (is.fail()) {
            G4cerr << "/ntuple/muon: failed to read" << G4endl;
        }
        if (index < 0) {
            G4cerr << "/ntuple/muon: invalid value: arguments are \"" << newValues << "\""
                   << G4endl;
            return;
        }
        myNtuple->SetNtupleMuon(index);
    }
    // invalid command
    else {
        G4cerr << "invalid detector \"set\" command\n" << std::flush;
    }
}

G4String CupRootNtupleMessenger::GetCurrentValue(G4UIcommand *command) {
    // CalDeviceCmd
    if (command == NtupleTrack) {
        return myNtuple->GetNtupleTrackStatus();
    } else if (command == NtupleStep) {
        return myNtuple->GetNtupleStepStatus();
    } else if (command == NtuplePhoton) {
        return myNtuple->GetNtuplePhotonStatus();
    } else if (command == NtupleScint) {
        return myNtuple->GetNtupleScintStatus();
    } else if (command == NtupleMuon) {
        return myNtuple->GetNtupleMuonStatus();
    }
    // invalid command
    else {
        return G4String("invalid CupRootNtupleMessenger \"get\" command");
    }
}
