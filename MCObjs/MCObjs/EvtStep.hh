#ifndef EVTSTEP_H
#define EVTSTEP_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EvtStep                                                              //
//                                                                      //
// Description of the step parameters                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "MCObjs/TStep.hh"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObject.h"

class TDirectory;

class EvtStep : public TObject {

  private:
    Int_t fNstep;        // Number of tracks
    TClonesArray *fStep; //->array with all tracks

    static TClonesArray *fgStep;

  public:
    EvtStep();
    virtual ~EvtStep();
    void Build();
    void Clear(Option_t *option = "");

    void SetNstep(Int_t n) { fNstep = n; }

    Int_t GetNstep() const { return fNstep; }
    TClonesArray *GetStep() const { return fStep; }

    ClassDef(EvtStep, 2) // Step structure
};

#endif
