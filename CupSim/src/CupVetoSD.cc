
#include "CupSim/CupVetoSD.hh"
#include "CupSim/CupVetoHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Version.hh"
#include "G4ios.hh"

#include "G4EmSaturation.hh"
#include "G4LossTableManager.hh"

CupVetoSD::CupVetoSD(G4String name, int arg_max_tgs) : G4VSensitiveDetector(name) {
    max_tgs = arg_max_tgs;
    G4String HCname;
    collectionName.insert(HCname = "VetoSDColl");
    HCID = -1;
}

CupVetoSD::~CupVetoSD() { ; }

void CupVetoSD::Initialize(G4HCofThisEvent *HCE) {
    hitsCollection = new CupVetoHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (HCID < 0) {
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(hitsCollection);
    }
    HCE->AddHitsCollection(HCID, hitsCollection);

    // fill TGSD hits with zero energy deposition
    for (G4int i = 0; i < max_tgs; i++) {
        CupVetoHit *aHit = new CupVetoHit(i);
        hitsCollection->insert(aHit);
    }
}

G4bool CupVetoSD::ProcessHits(G4Step *aStep, G4TouchableHistory * /*ROhist*/) {
    G4EmSaturation *emSaturation = G4LossTableManager::Instance()->EmSaturation();

#if G4VERSION_NUMBER <= 1020
    G4double edep_quenched = emSaturation->VisibleEnergyDeposition(aStep);
#else
    G4double edep_quenched =
        emSaturation->VisibleEnergyDepositionAtAStep(aStep); // for geant4.10.4.2
#endif
    G4double edep                      = aStep->GetTotalEnergyDeposit();
    G4ParticleDefinition *particleType = aStep->GetTrack()->GetDefinition();
    G4String particleName              = particleType->GetParticleName();

    if (edep == 0. || particleName == "opticalphoton") return true;

    G4StepPoint *preStepPoint            = aStep->GetPreStepPoint();
    G4TouchableHandle theTouchable       = preStepPoint->GetTouchableHandle();
    G4int copyNo                         = theTouchable->GetCopyNumber();
    G4int motherCopyNo                   = theTouchable->GetCopyNumber(1);
    G4VPhysicalVolume *thePhysical       = theTouchable->GetVolume();
    G4VPhysicalVolume *theMotherPhysical = theTouchable->GetVolume(1);
    G4String motherVolName               = theMotherPhysical->GetName();

    if ((strstr(motherVolName, "Envelope")) != NULL) copyNo = motherCopyNo;

    CupVetoHit *aHit = (*hitsCollection)[copyNo];
    // check if it is first touch
    if (!(aHit->GetLogV())) {
        // fill volume information
        aHit->SetLogV(thePhysical->GetLogicalVolume());
        G4AffineTransform aTrans = theTouchable->GetHistory()->GetTopTransform();
        aTrans.Invert();
        aHit->SetRot(aTrans.NetRotation());
        aHit->SetPos(aTrans.NetTranslation());
    }
    // add energy deposition
    aHit->AddEdep(edep);
    aHit->AddEdepQuenched(edep_quenched);

    return true;
}

void CupVetoSD::EndOfEvent(G4HCofThisEvent * /*HCE*/) { ; }
