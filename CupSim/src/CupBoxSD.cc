
#include "CupSim/CupBoxSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4ios.hh"

#include <string.h> // for memset

CupBoxSD::CupBoxSD(G4String name) : G4VSensitiveDetector(name) {
    eCut      = 1.5 * MeV;
    gCut      = 1.5 * MeV;
    radLength = 17.6 * mm;
    z0        = 0.0;
}

CupBoxSD::~CupBoxSD() {}

void CupBoxSD::Initialize(G4HCofThisEvent *) {
    memset(h_edep, 0, sizeof(h_edep));
    memset(h_ng, 0, sizeof(h_ng));
    memset(h_ne, 0, sizeof(h_ne));
    tot_edep = 0.0;
}

G4bool CupBoxSD::ProcessHits(G4Step *aStep, G4TouchableHistory *) {
    G4Track *pTrack             = aStep->GetTrack();
    G4StepPoint *pPreStepPoint  = aStep->GetPreStepPoint();
    G4StepPoint *pPostStepPoint = aStep->GetPostStepPoint();

    G4double edep = aStep->GetTotalEnergyDeposit();
    G4double e0   = pPreStepPoint->GetTotalEnergy();

    G4double z1 = pPreStepPoint->GetPosition().z();
    G4int iz1   = G4int(2.0 * fabs(z1 - z0) / radLength);
    G4double z2 = pPostStepPoint->GetPosition().z();
    G4int iz2   = G4int(2.0 * fabs(z2 - z0) / radLength);
    if (iz1 > iz2) {
        G4int tmp = iz1;
        iz1       = iz2;
        iz2       = tmp;
    }
    G4int numseg = iz2 - iz1 + 1;
    if (iz2 >= nbin) iz2 = nbin - 1;

    G4int i;
    if (edep > 0.0)
        for (i = iz1; i <= iz2; i++) // note "<=" implies numseg segments
            h_edep[i] += edep / numseg;

    G4ParticleDefinition *pdef = pTrack->GetDefinition();
    if (pdef->GetPDGCharge() == 0) {
        if (e0 > gCut)
            for (i = iz1; i < iz2; i++) // note "<" for only boundary-crossings
                h_ng[i]++;
    } else {
        if (e0 > eCut)
            for (i = iz1; i < iz2; i++) // note "<" for only boundary-crossings
                h_ne[i]++;
    }

    tot_edep += edep;

    return true;
}

void CupBoxSD::EndOfEvent(G4HCofThisEvent *) {

    if (verboseLevel > 0) PrintAll();

    if (verboseLevel > -1) {
        G4int sum_ng = 0, sum_ne = 0;
        for (int i = 0; i < nbin; i++) {
            sum_ng += h_ng[i];
            sum_ne += h_ne[i];
        }
        G4cout << GetName() << ":\t" << sum_ng << " neutral-x0,\t" << sum_ne << " charged-x0,\t"
               << tot_edep / MeV << " MeV\n"
               << G4endl;
        G4cout.flush();
    }
}

void CupBoxSD::clear() {}

void CupBoxSD::DrawAll() {}

// following routine is borrowed from test_radondaq by G. Horton-Smith
// it dumps histograms in "non-repeating-zeroes" format
template <class T>
static void dump_histo(T *hist, int hlength, int xmin) {
    int i;
    int nrzflag;

    nrzflag = -1;
    for (i = 0; i < hlength; i++) {
        if (hist[i] != 0 || nrzflag == -1) {
            if (nrzflag != -1 && nrzflag != i - 1) {
                G4cout << (i - 1 + xmin) << "\t0\n";
            }
            G4cout << (i + xmin) << "\t" << hist[i] << G4endl;
            if (hist[i] == 0)
                nrzflag = i;
            else
                nrzflag = -1;
        }
    }
    if (nrzflag > -1) {
        G4cout << (hlength - 1 + xmin) << "\t0\n";
    }
    G4cout << G4endl;
}

void CupBoxSD::PrintAll() {
    // print histograms in compressed form
    G4cout << "\n# " << GetName() << G4endl;
    G4cout << "# uncharged vs. 2*rad.length" << G4endl;
    dump_histo(h_ng, nbin, 0);
    G4cout << "# charged vs. 2*rad.length" << G4endl;
    dump_histo(h_ne, nbin, 0);
    G4cout << "# edep vs. 2*rad.length" << G4endl;
    dump_histo(h_edep, nbin, 0);
    G4cout << G4endl << std::flush;
}
