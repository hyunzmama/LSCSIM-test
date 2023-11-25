#include "MCObjs/MuonSD.hh"

ClassImp(MuonSD);

TClonesArray *MuonSD::fgCell = nullptr;

//______________________________________________________________________________
MuonSD::MuonSD() : TObject() {
    // Create an Track object.
    // When the constructor is invoked for the first time, the class static
    // variable fgTracks is 0 and the TClonesArray fgTracks is created.

    if (fgCell == nullptr) fgCell = new TClonesArray("TCell", 300);
    fMuCell = fgCell;

    nTotCell = 0;
}

//______________________________________________________________________________
MuonSD::~MuonSD() {
    Clear();
    delete fgCell;
    fgCell = nullptr;
}
//______________________________________________________________________________
void MuonSD::Clear(Option_t * /*option*/) { fMuCell->Delete(); }

