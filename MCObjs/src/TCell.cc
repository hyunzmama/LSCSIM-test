#include "MCObjs/TCell.hh"

ClassImp(TCell);

//______________________________________________________________________________
TCell::TCell() : TObject(), Edep(0), EdepQuenched(0), Index(-1) {}

//______________________________________________________________________________
TCell::TCell(const TCell &cell)
    : TObject(cell), Edep(cell.Edep), EdepQuenched(cell.EdepQuenched), Index(cell.Index) {}
// Copy a track object

//______________________________________________________________________________
TCell &TCell::operator=(const TCell &cell) {
    // Copy a track

    TObject::operator=(cell);
    Edep             = cell.GetEdep();
    EdepQuenched     = cell.GetEdepQuenched();
    Index            = cell.GetCellID();

    return *this;
}

//______________________________________________________________________________
void TCell::Clear(Option_t * /*option*/) { TObject::Clear(); }

