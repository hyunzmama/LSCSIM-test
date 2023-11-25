
#include "CupSim/CupDeferTrackProc.hh"

#include "G4EnergyLossTables.hh"
#include "G4Step.hh"
#include "G4VParticleChange.hh"
class G4UImessenger; // for G4ProcessTable.hh
#include "CupSim/CupPrimaryGeneratorAction.hh"
#include "G4ProcessTable.hh"
//#include "geomdefs.hh" // for kCarTolerance
#include "G4GeometryTolerance.hh"

////////////////////////////////////////////////////////////////

CupDeferTrackProc::CupDeferTrackProc(const G4String &aName) : G4VProcess(aName) {
    if (verboseLevel > 0) {
        G4cout << GetProcessName() << " is created " << G4endl;
    }

    _generator = CupPrimaryGeneratorAction::GetTheCupPrimaryGeneratorAction();
    if (_generator == 0) {
        G4Exception(" ", " ", JustWarning,
                    "CupSim/CupDeferTrackProc:: no CupPrimaryGeneratorAction instance.");
    }
}

CupDeferTrackProc::~CupDeferTrackProc() {}

////////////////////////////////////////////////////////////////

CupDeferTrackProc::CupDeferTrackProc(CupDeferTrackProc &right) : G4VProcess(right) {}

////////////////////////////////////////////////////////////////

G4double CupDeferTrackProc::PostStepGetPhysicalInteractionLength(const G4Track &aTrack,
                                                                 G4double /* previousStepSize */,
                                                                 G4ForceCondition *condition) {
    // condition is set to "Not Forced"
    *condition = NotForced;

    // apply maximum time limit
    G4double dTime = (_generator->GetEventWindow() - aTrack.GetGlobalTime());
    if (dTime <= 0.0) {
        double kCarTolerance;
        kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
        return kCarTolerance;
    }
    G4double beta = (aTrack.GetDynamicParticle()->GetTotalMomentum()) / (aTrack.GetTotalEnergy());
    return beta * c_light * dTime;
}

////////////////////////////////////////////////////////////////

G4VParticleChange *CupDeferTrackProc::PostStepDoIt(const G4Track &aTrack, const G4Step & /* aStep */
) {
    _generator->DeferTrackToLaterEvent(&aTrack);
    aParticleChange.Initialize(aTrack);
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return &aParticleChange;
}

////////////////////////////////////////////////////////////////
