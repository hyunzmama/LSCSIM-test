#ifndef TGSD_H
#define TGSD_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGSD                                                               //
//                                                                      //
// Description of the event parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "MCObjs/TCell.hh"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObject.h"

class TDirectory;

class TGSD : public TObject {

  private:
    Double_t TotEdep;
    Double_t TotEdepQuenched;
    Int_t nTotCell;
    TClonesArray *fCell; //->array with all hits

    static TClonesArray *fgCell;

  public:
    TGSD();
    virtual ~TGSD();
    void Clear(Option_t *option = "");

    void SetNTotCell(Int_t n) { nTotCell = n; }
    void SetTotEdep(Double_t eng) { TotEdep = eng; }
    void SetTotEdepQuenched(Double_t eng) { TotEdepQuenched = eng; }

    Int_t GetNTotCell() const { return nTotCell; }
    Double_t GetTotEdep() const { return TotEdep; }
    Double_t GetTotEdepQuenched() const { return TotEdepQuenched; }
    TClonesArray *GetCell() const { return fCell; }

    ClassDef(TGSD, 2) // Track structure
};

#endif
