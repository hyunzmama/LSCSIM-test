#ifndef PHOTON_H
#define PHOTON_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Photon                                                               //
//                                                                      //
// Description of the event parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "TObject.h"
//#include "TMath.h"
#include "MCObjs/THit.hh"

class TDirectory;

class Photon : public TObject {

  private:
    Int_t fNhitPmts; // Number of hit pmts
    Int_t fNhits;    // Number of photon hits
    Int_t fNop_cerenkov;
    Int_t fNop_scint;
    Int_t fNop_reem;
    TClonesArray *fHit; //->array with all hits

    static TClonesArray *fgHit;

  public:
    Photon();
    virtual ~Photon();
    void Clear(Option_t *option = "");

    void SetNhitPmts(Int_t n) { fNhitPmts = n; }
    void SetNhits(Int_t n) { fNhits = n; }
    void SetNcerenkov(Int_t n) { fNop_cerenkov = n; }
    void SetNscint(Int_t n) { fNop_scint = n; }
    void SetNreem(Int_t n) { fNop_reem = n; }

    Int_t GetNhitPmts() const { return fNhitPmts; }
    Int_t GetNhits() const { return fNhits; }
    Int_t GetNcerenkov() const { return fNop_cerenkov; }
    Int_t GetNscint() const { return fNop_scint; }
    Int_t GetNreem() const { return fNop_reem; }
    TClonesArray *GetHit() const { return fHit; }

    ClassDef(Photon, 2) // Track structure
};

#endif
