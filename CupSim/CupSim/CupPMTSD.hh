
#ifndef CupPMTSD_h
#define CupPMTSD_h 1

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

using namespace CLHEP;

class CupPMTSD : public G4VSensitiveDetector {
  protected:
    int max_pmts;
    int pmt_no_offset;
    int my_id_pmt_size;
    // enum { max_waveform_ns= 200 };

  public:
    G4int *hit_sum; /* indexed by pmt number */
    // typedef G4int waveform_t[max_waveform_ns];
    // waveform_t *hit_waveform; /* indexed by pmt number */

    G4int n_pmt_hits; /* # of hits,       calculated at EndOfEvent */
    G4int n_hit_pmts; /* # of PMTs hit,   calculated at EndOfEvent */

  public:
    // member functions
    CupPMTSD(G4String name, int max_pmts = 1920, int pmt_no_offset = 0, int my_id_pmt_size = -1);
    virtual ~CupPMTSD();

    virtual void Initialize(G4HCofThisEvent *HCE);
    virtual void EndOfEvent(G4HCofThisEvent *HCE);
    virtual void clear();
    virtual void DrawAll();
    virtual void PrintAll();

    void SimpleHit(G4int ipmt, G4double time, G4double kineticEnergy, const G4ThreeVector &position,
                   const G4ThreeVector &momentum, const G4ThreeVector &polarization,
                   G4int iHitPhotonCount,
                   G4int processTag); // EJ: 2007-11=06

  protected:
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
};

#endif
