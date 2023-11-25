#include "RVersion.h"
#include "TProcessID.h"

#include "MCObjs/Scint.hh"

ClassImp(Scint);

//______________________________________________________________________________
Scint::Scint()
    : TObject(), totScintEdep(0), totScintEdepQuenched(0), totScintPhotons(0), centroid_x(0),
      centroid_y(0), centroid_z(0) {}

//______________________________________________________________________________
Scint::Scint(const Scint &scin)
    : TObject(scin), totScintEdep(scin.totScintEdep),
      totScintEdepQuenched(scin.totScintEdepQuenched), totScintPhotons(scin.totScintPhotons),
      centroid_x(scin.centroid_x), centroid_y(scin.centroid_y), centroid_z(scin.centroid_z) {}
// Copy a track object

//______________________________________________________________________________
Scint &Scint::operator=(const Scint &scin) {
    // Copy a track

    TObject::operator    =(scin);
    totScintEdep         = scin.GetTotScintEdep();
    totScintEdepQuenched = scin.GetTotScintEdepQuenched();
    totScintPhotons      = scin.GetTotScintPhotons();
    centroid_x           = scin.GetCentX();
    centroid_y           = scin.GetCentY();
    centroid_z           = scin.GetCentZ();

    return *this;
}

//______________________________________________________________________________
void Scint::Clear(Option_t * /*option*/) { TObject::Clear(); }

