
#include "CupSim/CupVEventAction.hh"

#include "G4DigiManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UImanager.hh"
#include "G4UnitsTable.hh"
#include "G4VHitsCollection.hh"
#include "G4VVisManager.hh"
#include "G4Version.hh"
#include "G4ios.hh"

#include "CupSim/CupScintillation.hh" // for doScintilllation and total energy deposition info

#include "CupSim/CupRecorderBase.hh"

CupHitPMTCollection CupVEventAction ::theHitPMTCollection = CupHitPMTCollection();
G4bool CupVEventAction ::flagFullOutputMode               = false;

CupVEventAction::CupVEventAction(CupRecorderBase *r) : recorder(r), drawFlag("all") {
    fDrawCmd = new G4UIcmdWithAString("/event/drawTracks", this);
    fDrawCmd->SetGuidance("Draw the tracks in the event");
    fDrawCmd->SetGuidance("  Choice : none, charged(default), all, allnonopt");
    fDrawCmd->SetParameterName("choice", true);
    fDrawCmd->SetDefaultValue("charged");
    fDrawCmd->SetCandidates("none charged all allnonopt");
    fDrawCmd->AvailableForStates(G4State_Idle);

    fFileCmd = new G4UIcommand("/event/output_file", this);
    fFileCmd->SetGuidance("Set the file name for event output.");
    fFileCmd->SetParameter(new G4UIparameter("filename", 's', true));

    fModeCmd = new G4UIcmdWithAString("/event/output_mode", this);
    fModeCmd->SetGuidance("Select how much hit data you want output:");
    fModeCmd->SetGuidance("  Choice : basic(default), full");
    fModeCmd->SetParameterName("choice", true);
    fModeCmd->SetDefaultValue("basic");
    fModeCmd->SetCandidates("basic full");
    fModeCmd->AvailableForStates(G4State_PreInit, G4State_Idle, G4State_GeomClosed);
}

CupVEventAction::~CupVEventAction() {
    delete fDrawCmd;
    delete fFileCmd;
    delete fModeCmd;
}

G4String CupVEventAction::GetCurrentValue(G4UIcommand *command) {
    if (command->GetCommandName() == "drawTracks")
        return drawFlag;
    else if (command->GetCommandName() == "output_mode")
        return (flagFullOutputMode ? "full" : "basic");

    return G4String("Unhandled command passed to "
                    "CupSim/CupEventAction::GetCurrentValue");
}

// EJ
void CupVEventAction::SetNewValue(G4UIcommand *command, G4String newValue) {}

void CupVEventAction::BeginOfEventAction(const G4Event *evt) {
    // EJ:
    CupScintillation::ResetTotEdep();
    // clearing theHitPMTCollection clears away the HitPhotons and HitPMTs
    theHitPMTCollection.Clear();
}

void CupVEventAction::EndOfEventAction(const G4Event *evt) {
    G4TrajectoryContainer *trajectoryContainer = evt->GetTrajectoryContainer();
    G4int n_trajectories                       = 0;
    if (trajectoryContainer) {
        n_trajectories = trajectoryContainer->size();
    }

    // FillData(evt); // EJ: commented out

    // draw trajectories
    if (G4VVisManager::GetConcreteInstance() && drawFlag != "none") {
        for (G4int i = 0; i < n_trajectories; i++) {
            // CupUDGE: the explicit cast on the next line is not a good thing.
            G4Trajectory *trj = (G4Trajectory *)((*trajectoryContainer)[i]);
            if ((drawFlag == "all") ||
                ((drawFlag == "allnonopt") && (trj->GetParticleName() != "opticalphoton")) ||
                ((drawFlag == "charged") && (trj->GetCharge() != 0.)))
#if G4VERSION_NUMBER <= 999
                trj->DrawTrajectory(50);
#else
                trj->DrawTrajectory();
#endif
        }
    }
    // Do any necessary record-keeping.
    if (recorder != 0) recorder->RecordEndOfEvent(evt); // EJ
}
