#include "MCObjs/Primary.hh"

ClassImp(Primary);

TClonesArray *Primary::fgVertex = nullptr;

//______________________________________________________________________________
Primary::Primary() : TObject() {
    // Create an Primary object.
    // When the constructor is invoked for the first time, the class static
    // variable fgTracks is 0 and the TClonesArray fgTracks is created.

    if (fgVertex == nullptr) fgVertex = new TClonesArray("Vertex", 100);
    fVertex = fgVertex;

    fNvertex = 0;
}

//______________________________________________________________________________
Primary::~Primary() {
    Clear();
    delete fgVertex;
    fgVertex = nullptr;
}
//______________________________________________________________________________
void Primary::Clear(Option_t * /*option*/) {
    //   fVertex->Clear("C"); //will also call Track::Clear
    // fVertex->Clear();
    fVertex->Delete();
    fNvertex   = 0;
    ketot      = 0;
    centroid_x = 0;
    centroid_y = 0;
    centroid_z = 0;
}
