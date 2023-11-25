#include "RVersion.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "MCObjs/EvtStep.hh"

ClassImp(EvtStep);

TClonesArray *EvtStep::fgStep = nullptr;

//______________________________________________________________________________
EvtStep::EvtStep() : TObject() {
    // Create an Step object.
    // When the constructor is invoked for the first time, the class static
    // variable fgSteps is 0 and the TClonesArray fgSteps is created.

    if (fgStep == nullptr) fgStep = new TClonesArray("TStep", 1000000);
    fStep = fgStep;

    fNstep = 0;
}

//______________________________________________________________________________
EvtStep::~EvtStep() {
    Clear();
    delete fgStep;
    fgStep = nullptr;
}

//______________________________________________________________________________
void EvtStep::Build() {
    // Save current Object count
    Int_t ObjectNumber = TProcessID::GetObjectCount();
    Clear();

    TProcessID::SetObjectCount(ObjectNumber);
}

//______________________________________________________________________________
void EvtStep::Clear(Option_t * /*option*/) { fStep->Delete(); }

