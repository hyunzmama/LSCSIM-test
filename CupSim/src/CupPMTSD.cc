
#include "CupSim/CupPMTSD.hh"
#include "CupSim/CupDetectorConstruction.hh"
#include "CupSim/CupScintillation.hh" // for doScintilllation and total energy deposition info
#include "CupSim/CupVEventAction.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VSolid.hh" // for access to solid store
#include "G4ios.hh"

#include "Randomize.hh"

#include <string.h> // for memset

CupPMTSD::CupPMTSD(G4String name, int arg_max_pmts, int arg_pmt_no_offset, int arg_my_id_pmt_size)
    : G4VSensitiveDetector(name) {
    max_pmts       = arg_max_pmts;
    pmt_no_offset  = arg_pmt_no_offset;
    my_id_pmt_size = arg_my_id_pmt_size;

    hit_sum = new G4int[max_pmts];
}

CupPMTSD::~CupPMTSD() {
    if (hit_sum) delete[] hit_sum;
}

void CupPMTSD::Initialize(G4HCofThisEvent *) {
    memset(hit_sum, 0, sizeof(hit_sum[0]) * max_pmts);
    n_pmt_hits = n_hit_pmts = 0;
}

G4bool CupPMTSD::ProcessHits(G4Step *, G4TouchableHistory *) {
    // NOTE: ProcessHits should never be called anymore, because
    // hits are decided by CupPMTOpticalModel, which calls CupPMTSD::SimpleHit()
    // in order to avoid having to create a bogus G4Step object.
    ////////////////////////////////////////////////////////////////

    return false;
}

// Here is the real "hit" routine, used by CupPMTOpticalModel and by ProcessHits
// It is more efficient in some ways.
void CupPMTSD::SimpleHit(G4int ipmt, G4double time, G4double kineticEnergy,
                         const G4ThreeVector &hit_position, const G4ThreeVector &hit_momentum,
                         const G4ThreeVector &hit_polarization, G4int iHitPhotonCount,
                         G4int processTag) // EJ
//			 G4int iHitPhotonCount )
{
    G4int pmt_index = ipmt - pmt_no_offset;
    if (pmt_index < 0 || pmt_index >= max_pmts) {
        G4cerr << "Error: CupPMTSD::SimpleHit [" << GetName() << "] passed ipmt=" << ipmt
               << ", but max_pmts=" << max_pmts << " and offset=" << pmt_no_offset << " !"
               << G4endl;
        return;
    }

    hit_sum[pmt_index] += iHitPhotonCount;

    // create new CupHitPhoton, the way of recording photo hits on PMTs
    CupHitPhoton *hit_photon = new CupHitPhoton();
    hit_photon->SetPMTID((int)ipmt);
    hit_photon->SetTime((double)time);
    hit_photon->SetKineticEnergy((double)kineticEnergy);
    hit_photon->SetPosition((double)hit_position.x(), (double)hit_position.y(),
                            (double)hit_position.z());
    hit_photon->SetMomentum((double)hit_momentum.x(), (double)hit_momentum.y(),
                            (double)hit_momentum.z());
    hit_photon->SetPolarization((double)hit_polarization.x(), (double)hit_polarization.y(),
                                (double)hit_polarization.z());
    hit_photon->SetCount(iHitPhotonCount);
    hit_photon->SetProcessTag(processTag); // EJ: 2007-11-06

    CupVEventAction::GetTheHitPMTCollection()->DetectPhoton(hit_photon);
}

void CupPMTSD::EndOfEvent(G4HCofThisEvent *) {
    int ipmt;

    n_pmt_hits = 0;
    n_hit_pmts = 0;
    for (ipmt = 0; ipmt < max_pmts; ipmt++) {
        if (hit_sum[ipmt]) {
            n_pmt_hits += hit_sum[ipmt];
            n_hit_pmts++;
        }
    }
}

void CupPMTSD::clear() {}

void CupPMTSD::DrawAll() {}

void CupPMTSD::PrintAll() {}
