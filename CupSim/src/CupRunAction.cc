
#include "CupSim/CupRunAction.hh"
#include "CupSim/CupRecorderBase.hh"

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

CupRunAction::CupRunAction(CupRecorderBase *r) : recorder(r) { runIDcounter = 0; }

CupRunAction::~CupRunAction() {}

void CupRunAction::BeginOfRunAction(const G4Run *aRun) {
    ((G4Run *)(aRun))->SetRunID(runIDcounter++);

    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

    G4UImanager *UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/tracking/storeTrajectory 1");

    G4VVisManager *pVVisManager = G4VVisManager::GetConcreteInstance();

    if (pVVisManager) {
        UI->ApplyCommand("/vis~/clear/view");
        UI->ApplyCommand("/vis~/draw/current");
    }
    // Do any necessary record-keeping.
    if (recorder != 0) recorder->RecordBeginOfRun(aRun);
}

void CupRunAction::EndOfRunAction(const G4Run *aRun) {
    G4VVisManager *pVVisManager = G4VVisManager::GetConcreteInstance();

    if (pVVisManager) {
        G4UImanager::GetUIpointer()->ApplyCommand("/vis/show/view");
    }
    // Do any necessary record-keeping.
    if (recorder != 0) recorder->RecordEndOfRun(aRun);
}
