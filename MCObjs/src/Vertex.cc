#include "RVersion.h"
#include "TProcessID.h"

#include "MCObjs/Vertex.hh"

ClassImp(Vertex);

//______________________________________________________________________________
Vertex::Vertex()
    : TObject(), t0(-1), x0(-1), y0(-1), z0(-1), px(-1), py(-1), pz(-1), polx(-1), poly(-1),
      polz(-1), ke(-1), pdgcode(-1), copyno(-1), particlename(), volumename() {}

//______________________________________________________________________________
Vertex::Vertex(const Vertex &orig)
    : TObject(orig), t0(orig.t0), x0(orig.x0), y0(orig.y0), z0(orig.z0), px(orig.px), py(orig.py),
      pz(orig.pz), polx(orig.polx), poly(orig.poly), polz(orig.polz), ke(orig.ke),
      pdgcode(orig.pdgcode), copyno(orig.copyno), particlename(orig.particlename),
      volumename(orig.volumename) {}
// Copy a track object

//______________________________________________________________________________
Vertex &Vertex::operator=(const Vertex &orig) {
    // Copy a track

    TObject::operator=(orig);
    t0               = orig.t0;
    x0               = orig.x0;
    y0               = orig.y0;
    z0               = orig.z0;
    px               = orig.px;
    py               = orig.py;
    pz               = orig.pz;
    polx             = orig.polx;
    poly             = orig.poly;
    polz             = orig.polz;
    ke               = orig.ke;
    pdgcode          = orig.pdgcode;
    particlename     = orig.particlename;
    volumename       = orig.volumename;
    copyno           = orig.copyno;

    return *this;
}

//______________________________________________________________________________
void Vertex::Clear(Option_t * /*option*/) {
    // Note that we intend on using TClonesArray::ConstructedAt, so we do not
    // need to delete any of the arrays.

    TObject::Clear();
}
