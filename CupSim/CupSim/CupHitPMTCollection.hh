#ifndef __CupHitPMTCollection_hh__
#define __CupHitPMTCollection_hh__

#include "CupHitPMT.hh"
#include <map>
#include <vector>

#include "G4ThreeVector.hh"
using namespace CLHEP;

class CupHitPMTCollection {
  public:
    CupHitPMTCollection();
    virtual ~CupHitPMTCollection();

    void Clear();
    void DetectPhoton(CupHitPhoton *);
    void SortTimeAscending();
    int GetEntries() const;
    CupHitPMT *GetPMT(int i) const;
    CupHitPMT *GetPMT_ByID(int id) const;

    void Print(std::ostream &) const;

  private:
    std::vector<CupHitPMT *> fPMT;
    std::map<short, CupHitPMT *> fHitmap;
};

#endif // __CupHitPMTCollection_hh__
