#include "MCObjs/PMTSD.hh"

ClassImp(PMTSD);

//______________________________________________________________________________
PMTSD::PMTSD() : TObject(), Nhits(0), NhitPmts(0) {}

//______________________________________________________________________________
PMTSD::PMTSD(const PMTSD &cell) : TObject(cell), Nhits(cell.Nhits), NhitPmts(cell.NhitPmts) {}
// Copy a track object

//______________________________________________________________________________
PMTSD &PMTSD::operator=(const PMTSD &cell) {
    // Copy a track

    TObject::operator=(cell);
    Nhits            = cell.GetNhits();
    NhitPmts         = cell.GetNhitPmts();

    return *this;
}

//______________________________________________________________________________
void PMTSD::Clear(Option_t * /*option*/) { TObject::Clear(); }

