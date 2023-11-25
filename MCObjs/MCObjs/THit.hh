#ifndef THIT_H
#define THIT_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// THit class                                                         //
//                                                                      //
// Description of the track parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
//#include "TString.h"

// class TDirectory;

class THit : public TObject {

  private:
    Double_t Time;
    Int_t PMTno;
    Int_t Wavelength;
    Float_t x, y, z;
    Float_t px, py, pz;
    Float_t polx, poly, polz;
    Int_t Count;
    Int_t ProcessTag;

  public:
    THit();
    THit(const THit &orig);
    virtual ~THit() { Clear(); }
    THit &operator=(const THit &orig);

    void Clear(Option_t *option = "");
    Double_t GetHitTime() const { return Time; }
    Int_t GetHitPMT() const { return PMTno; }
    Int_t GetWaveLength() const { return Wavelength; }
    Float_t GetX() const { return x; }
    Float_t GetY() const { return y; }
    Float_t GetZ() const { return z; }
    Float_t GetPX() const { return px; }
    Float_t GetPY() const { return py; }
    Float_t GetPZ() const { return pz; }
    Float_t GetPolX() const { return polx; }
    Float_t GetPolY() const { return poly; }
    Float_t GetPolZ() const { return polz; }
    Int_t GetHitCount() const { return Count; }
    Double_t GetProcessTag() const { return ProcessTag; }

    void SetHitTime(Double_t time) { Time = time; }
    void SetHitPMT(int no) { PMTno = no; }
    void SetWaveLength(int wl) { Wavelength = wl; }
    void SetX(Float_t xx) { x = xx; }
    void SetY(Float_t yy) { y = yy; }
    void SetZ(Float_t zz) { z = zz; }
    void SetPX(Float_t xx) { px = xx; }
    void SetPY(Float_t yy) { py = yy; }
    void SetPZ(Float_t zz) { pz = zz; }
    void SetPolX(Float_t xx) { polx = xx; }
    void SetPolY(Float_t yy) { poly = yy; }
    void SetPolZ(Float_t zz) { polz = zz; }
    void SetHitCount(int cnt) { Count = cnt; }
    void SetProcessTag(int tag) { ProcessTag = tag; }

    ClassDef(THit, 10) // A track segment
};

#endif
