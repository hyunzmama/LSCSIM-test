
#ifndef __CupHitPMT_hh__
#define __CupHitPMT_hh__

#include "CupHitPhoton.hh"
#include <cstddef>
#include <vector>

#include "G4ThreeVector.hh"
using namespace CLHEP;

#include <vector>

class CupHitPMT {
  public:
    CupHitPMT(int ID);
    ~CupHitPMT();

    void Clear();
    void DetectPhoton(CupHitPhoton *);
    void SortTimeAscending();

    int GetID() const { return fID; }
    int GetEntries() const { return fPhotons.size(); }
    CupHitPhoton *GetPhoton(int i) const { return fPhotons[i]; }

    void Print(std::ostream &, bool fullDetailsMode = false);

    static const size_t kApproxMaxIndividualHitPhotonsPerPMT;
    static const double kMergeTime;

  private:
    int fID;
    std::vector<CupHitPhoton *> fPhotons;
};

/** comparison function for sorting CupHitPMT pointers
 */
inline bool Compare_HitPMTPtr_TimeAscending(const CupHitPMT *a, const CupHitPMT *b) {
    // put empties at the end
    if (!a || a->GetEntries() <= 0) return false;
    if (!b || b->GetEntries() <= 0) return true;
    return a->GetPhoton(0)->GetTime() < b->GetPhoton(0)->GetTime();
}

#endif // __CupHitPMT_hh__
