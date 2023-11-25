//
#ifndef CupVetoSD_h
#define CupVetoSD_h 1

#include "CupVetoHit.hh"
#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class CupVetoSD : public G4VSensitiveDetector {
    // EJ:
  protected:
    int max_tgs;

  public:
    CupVetoSD(G4String name, int max_tgs = 1000);
    virtual ~CupVetoSD();

    virtual void Initialize(G4HCofThisEvent *HCE);
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
    virtual void EndOfEvent(G4HCofThisEvent *HCE);

  private:
    CupVetoHitsCollection *hitsCollection;
    G4int HCID;
};

#endif
