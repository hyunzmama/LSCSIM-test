#include "TDirectory.h"

#include "MCObjs/Photon.hh"

ClassImp(Photon);

TClonesArray *Photon::fgHit = nullptr;

//______________________________________________________________________________
Photon::Photon() : TObject() {

    if (fgHit == nullptr) fgHit = new TClonesArray("THit", 200000);
    fHit = fgHit;

    fNhitPmts = 0;
}

//______________________________________________________________________________
Photon::~Photon() {
    Clear();
    delete fgHit;
    fgHit = nullptr;
}
//______________________________________________________________________________
void Photon::Clear(Option_t * /*option*/) {
    fHit->Delete();
}

