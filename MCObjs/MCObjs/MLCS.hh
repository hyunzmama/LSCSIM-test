#ifndef MLCS_HH
#define MLCS_HH

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MLCS                                                               //
//                                                                      //
// Description of the event parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class MLCS : public TObject {

  private:
    Int_t Nhits;
    Int_t NhitPmts;

  public:
    MLCS();
    MLCS(const MLCS &orig);
    virtual ~MLCS() { Clear(); }
    MLCS &operator=(const MLCS &orig);

    void Clear(Option_t *option = "");
    Int_t GetNhits() const { return Nhits; }
    Int_t GetNhitPmts() const { return NhitPmts; }

    void SetNhits(Int_t nhit) { Nhits = nhit; }
    void SetNhitPmts(Int_t npmt) { NhitPmts = npmt; }

    ClassDef(MLCS, 2) // Track structure
};

#endif
