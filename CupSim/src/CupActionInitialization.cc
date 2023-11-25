#include "G4Version.hh"

#if G4VERSION_NUMBER >= 1000
#include "CupSim/CupActionInitialization.hh"
#include "CupSim/CupPrimaryGeneratorAction.hh"
#include "CupSim/CupRunAction.hh"
#include "CupSim/CupSteppingAction.hh"
#include "CupSim/CupTrackingAction.hh"
#include "CupSim/CupVEventAction.hh"

void CupActionInitialization::BuildForMaster() const {
    SetUserAction(new CupRunAction(fRecorders));
}

void CupActionInitialization::Build() const {
    auto p = new CupPrimaryGeneratorAction(fDetConstruction);
    SetUserAction(p);
    SetUserAction(new CupRunAction(fRecorders));
    SetUserAction(new CupVEventAction(fRecorders));
    SetUserAction(new CupTrackingAction(fRecorders));
    SetUserAction(new CupSteppingAction(fRecorders, p));
}
#endif
