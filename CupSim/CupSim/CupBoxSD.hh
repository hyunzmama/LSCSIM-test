
#ifndef CupBOXSD_h
#define CupBOXSD_h 1

#include "G4Timer.hh"
#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

using namespace CLHEP;

class CupBoxSD : public G4VSensitiveDetector {
  public:
    // member functions
    CupBoxSD(G4String name);
    virtual ~CupBoxSD();

    virtual void Initialize(G4HCofThisEvent *HCE);
    virtual void EndOfEvent(G4HCofThisEvent *HCE);
    virtual void clear();
    virtual void DrawAll();
    virtual void PrintAll();

    void SetZ0(G4double newZ0) { z0 = newZ0; }
    void SetRadLength(G4double newRadLength) { radLength = newRadLength; }
    void SetECut(G4double e) { eCut = e; }
    void SetGCut(G4double e) { gCut = e; }
    G4double GetZ0() { return z0; }
    G4double GetRadLength() { return radLength; }
    G4double GetECut() { return eCut; }
    G4double GetGCut() { return gCut; }

  protected:
    G4double eCut;
    G4double gCut;
    G4double radLength;
    G4double z0;
    enum { nbin = 40 };
    G4double tot_edep;
    G4double h_edep[nbin];
    G4int h_ng[nbin], h_ne[nbin];
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
};

#endif
