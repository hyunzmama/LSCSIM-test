
#include "CupSim/CupHitPMTCollection.hh"
#include <G4ios.hh>
#include <algorithm>

CupHitPMTCollection::CupHitPMTCollection() {}

CupHitPMTCollection::~CupHitPMTCollection() { Clear(); }

/** clear out AND DELETE HitPMTs (and HitPhotons) that were detected,
    resetting this HitPMTCollection to be empty */
void CupHitPMTCollection::Clear() {
    for (size_t i = 0; i < fPMT.size(); i++) {
        fPMT[i]->Clear();
        delete fPMT[i];
    }
    fPMT.clear();
    fHitmap.clear();
}

/** find or make appropriate HitPMT, and DetectPhoton in that HitPMT */
void CupHitPMTCollection::DetectPhoton(CupHitPhoton *new_photon) {
    CupHitPMT *hitpmtptr = GetPMT_ByID(new_photon->GetPMTID());

    if (hitpmtptr != NULL) {
        // found a HitPMT with this ID
        hitpmtptr->DetectPhoton(new_photon);
    } else {
        // make a HitPMT with this ID
        fPMT.push_back(new CupHitPMT((short)new_photon->GetPMTID()));
        fPMT[fPMT.size() - 1]->DetectPhoton(new_photon);
        fHitmap.insert(
            std::make_pair((short)new_photon->GetPMTID(), (CupHitPMT *)fPMT[fPMT.size() - 1]));
    }
}

void CupHitPMTCollection::SortTimeAscending() {
    for (size_t i = 0; i < fPMT.size(); i++)
        fPMT[i]->SortTimeAscending();
    std::sort(fPMT.begin(), fPMT.end(), Compare_HitPMTPtr_TimeAscending);
}

/** return the number of HitPMTs in internal collection */
int CupHitPMTCollection::GetEntries() const { return fPMT.size(); }

/** return the i-th HitPMT in internal collection */
CupHitPMT *CupHitPMTCollection::GetPMT(int i) const { return fPMT[i]; }

/** return pointer to HitPMT with given id if in collection,
    or NULL if no such HitPMT is in collection */
CupHitPMT *CupHitPMTCollection::GetPMT_ByID(int id) const {
    std::map<short, CupHitPMT *>::const_iterator p = fHitmap.find((short)id);
    if (p != fHitmap.end()) {
        // found a HitPMT with this ID
        return p->second;
    } else {
        // no HitPMT
        return NULL;
    }
}

/// print out HitPMTs
void CupHitPMTCollection::Print(std::ostream &os) const {
    for (size_t i = 0; i < fPMT.size(); i++) {
        fPMT[i]->Print(os);
    }
}
