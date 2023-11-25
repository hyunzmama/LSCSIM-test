#ifndef SCINT_H
#define SCINT_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Scint                                                               //
//                                                                      //
// Description of the event parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class Scint : public TObject {

  private:
    Double_t totScintEdep;
    Double_t totScintEdepQuenched;
    Int_t totScintPhotons;
    Double_t centroid_x;
    Double_t centroid_y;
    Double_t centroid_z;

  public:
    Scint();
    Scint(const Scint &orig);
    virtual ~Scint() { Clear(); }
    Scint &operator=(const Scint &orig);

    void Clear(Option_t *option = "");
    Double_t GetTotScintEdep() const { return totScintEdep; }
    Double_t GetTotScintEdepQuenched() const { return totScintEdepQuenched; }
    Int_t GetTotScintPhotons() const { return totScintPhotons; }
    Double_t GetCentX() const { return centroid_x; }
    Double_t GetCentY() const { return centroid_y; }
    Double_t GetCentZ() const { return centroid_z; }

    void SetTotScintPhotons(Int_t n) { totScintPhotons = n; }
    void SetTotScintEdep(Double_t edep) { totScintEdep = edep; }
    void SetTotScintEdepQuenched(Double_t e) { totScintEdepQuenched = e; }
    void SetCentroidX(Double_t x) { centroid_x = x; }
    void SetCentroidY(Double_t y) { centroid_x = y; }
    void SetCentroidZ(Double_t z) { centroid_x = z; }

    ClassDef(Scint, 2) // Track structure
};

#endif
