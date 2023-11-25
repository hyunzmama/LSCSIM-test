#ifndef VERTEX_H
#define VERTEX_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Event                                                                //
//                                                                      //
// Description of the event and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
//#include "TMath.h"
#include "TString.h"

class TDirectory;

class Vertex : public TObject {

  private:
    Float_t t0;
    Float_t x0, y0, z0;
    Float_t px, py, pz;
    Float_t polx, poly, polz;
    Float_t ke;
    Int_t pdgcode;
    Int_t copyno;
    TString particlename;
    TString volumename;

  public:
    Vertex();
    Vertex(const Vertex &orig);
    virtual ~Vertex() { Clear(); }
    Vertex &operator=(const Vertex &orig);

    void Clear(Option_t *option = "");
    Float_t GetT0() const { return t0; }
    Float_t GetX0() const { return x0; }
    Float_t GetY0() const { return y0; }
    Float_t GetZ0() const { return z0; }
    Float_t GetPX() const { return px; }
    Float_t GetPY() const { return py; }
    Float_t GetPZ() const { return pz; }
    Float_t GetPolX() const { return polx; }
    Float_t GetPolY() const { return poly; }
    Float_t GetPolZ() const { return polz; }
    Float_t GetKE() const { return ke; }
    Int_t GetPDGcode() const { return pdgcode; }
    const char *GetParticleName() const { return particlename; }
    const char *GetVolumeName() const { return volumename; }
    Int_t GetCopyNo() const { return copyno; }

    void SetT0(Float_t T0) { t0 = T0; }
    void SetX0(Float_t xx) { x0 = xx; }
    void SetY0(Float_t yy) { y0 = yy; }
    void SetZ0(Float_t zz) { z0 = zz; }
    void SetPX(Float_t xx) { px = xx; }
    void SetPY(Float_t yy) { py = yy; }
    void SetPZ(Float_t zz) { pz = zz; }
    void SetPolX(Float_t xx) { polx = xx; }
    void SetPolY(Float_t yy) { poly = yy; }
    void SetPolZ(Float_t zz) { polz = zz; }
    void SetKE(Float_t KE) { ke = KE; }
    void SetPDGcode(Int_t code) { pdgcode = code; }
    void SetParticleName(const char *pname) { particlename = pname; }
    void SetVolumeName(const char *vname) { volumename = vname; }
    void SetCopyNo(Int_t copynumber) { copyno = copynumber; }

    ClassDef(Vertex, 10) // A track segment
};

#endif
