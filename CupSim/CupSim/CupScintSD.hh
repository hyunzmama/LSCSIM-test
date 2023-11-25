#ifndef CupScintSD_h
#define CupScintSD_h 1

#include "CupScintHit.hh"
#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class CupScintSD : public G4VSensitiveDetector {
  public:
    CupScintSD(G4String name, int max_tgs = 1000);
    virtual ~CupScintSD();

    virtual void Initialize(G4HCofThisEvent *HCE);
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
    virtual void EndOfEvent(G4HCofThisEvent *HCE);

  protected:
    int max_tgs;

    CupScintHitsCollection *hitsCollection;
    G4int HCID;
};

#endif
