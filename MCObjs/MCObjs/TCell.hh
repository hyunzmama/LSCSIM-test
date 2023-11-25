#ifndef TCELL_H
#define TCELL_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCell class                                                          //
//                                                                      //
// Description of the track parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TCell : public TObject {

  private:
    Double_t Edep;
    Double_t EdepQuenched;
    Int_t Index;

  public:
    TCell();
    TCell(const TCell &orig);
    virtual ~TCell() { Clear(); }
    TCell &operator=(const TCell &orig);

    void Clear(Option_t *option = "");
    Double_t GetEdep() const { return Edep; }
    Double_t GetEdepQuenched() const { return EdepQuenched; }
    Int_t GetCellID() const { return Index; }

    void SetEdep(Double_t eng) { Edep = eng; }
    void SetEdepQuenched(Double_t eng) { EdepQuenched = eng; }
    void SetCellID(Int_t n) { Index = n; }

    ClassDef(TCell, 10) // A track segment
};

#endif
