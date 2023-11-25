#include "MCObjs/TGSD.hh"

ClassImp(TGSD);

TClonesArray *TGSD::fgCell = nullptr;

//______________________________________________________________________________
TGSD::TGSD() : TObject() {
    // Create an Track object.
    // When the constructor is invoked for the first time, the class static
    // variable fgTracks is 0 and the TClonesArray fgTracks is created.

    if (fgCell == nullptr) fgCell = new TClonesArray("TCell", 1000);
    fCell = fgCell;

    nTotCell = 0;
}

//______________________________________________________________________________
TGSD::~TGSD() {
    Clear();
    delete fgCell;
    fgCell = nullptr;
}
void TGSD::Clear(Option_t * /*option*/) {
    //   fCell->Clear("C"); //will also call Track::Clear
    // fCell->Clear(); //will also call Track::Clear
    fCell->Delete();
};

