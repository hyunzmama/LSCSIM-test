#ifndef EVTTRACK_H
#define EVTTRACK_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EvtTrack                                                             //
//                                                                      //
// Description of the event parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "MCObjs/TTrack.hh"
#include "TClonesArray.h"
#include "TObject.h"

class TDirectory;

class EvtTrack : public TObject {

  private:
    Int_t fNtrack;        // Number of tracks
    TClonesArray *fTrack; //->array with all tracks

    static TClonesArray *fgTrack;

  public:
    EvtTrack();
    virtual ~EvtTrack();
    void Clear(Option_t *option = "");

    void SetNtrack(Int_t n) { fNtrack = n; }

    Int_t GetNtrack() const { return fNtrack; }
    TClonesArray *GetTrack() const { return fTrack; }

    ClassDef(EvtTrack, 2) // Track structure
};

#endif
