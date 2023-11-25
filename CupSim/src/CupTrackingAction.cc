#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

#include "CupSim/CupTrackingAction.hh"
//#include "CupSim/CupDetectorConstruction.hh"
//#include "CupSim/CupUserTrackInformation.hh"
#include "CupSim/CupRecorderBase.hh"
#include "G4Trajectory.hh"

CupTrackingAction::CupTrackingAction(CupRecorderBase *r) : recorder(r) {}

void CupTrackingAction::PreUserTrackingAction(const G4Track *aTrack) {
    if (aTrack->GetDefinition()->GetParticleName() == "opticalphoton")
        return;
    else if (recorder)
        recorder->RecordTrack(aTrack);
}

void CupTrackingAction::PostUserTrackingAction(const G4Track *aTrack) {}
