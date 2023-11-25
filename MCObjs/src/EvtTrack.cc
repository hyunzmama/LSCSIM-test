#include "MCObjs/EvtTrack.hh"

ClassImp(EvtTrack);

TClonesArray *EvtTrack::fgTrack = nullptr;

//______________________________________________________________________________
EvtTrack::EvtTrack() : TObject() {
    if (fgTrack == nullptr) fgTrack = new TClonesArray("TTrack", 1000000);
    fTrack = fgTrack;

    fNtrack = 0;
}

//______________________________________________________________________________
EvtTrack::~EvtTrack() {
    Clear();
    delete fgTrack;
    fgTrack = nullptr;
}
//______________________________________________________________________________
void EvtTrack::Clear(Option_t * /*option*/) { fTrack->Delete(); }
