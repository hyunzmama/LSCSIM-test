#include "MCObjs/MLCS.hh"

ClassImp(MLCS);

//______________________________________________________________________________
MLCS::MLCS() : TObject(), Nhits(0), NhitPmts(0) {}

//______________________________________________________________________________
MLCS::MLCS(const MLCS &cell) : TObject(cell), Nhits(cell.Nhits), NhitPmts(cell.NhitPmts) {}
// Copy a track object

//______________________________________________________________________________
MLCS &MLCS::operator=(const MLCS &cell) {
    // Copy a track

    TObject::operator=(cell);
    Nhits            = cell.GetNhits();
    NhitPmts         = cell.GetNhitPmts();

    return *this;
}

//______________________________________________________________________________
void MLCS::Clear(Option_t * /*option*/) { TObject::Clear(); }
