#ifndef __CupActionInitialization_h__
#define __CupActionInitialization_h__ 1

#include "G4Version.hh"

#if G4VERSION_NUMBER >= 1000
#include "G4MTRunManager.hh"
#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class CupDetectorConstruction;
class CupRecorderBase;

class CupActionInitialization : public G4VUserActionInitialization {
  public:
    CupActionInitialization(CupRecorderBase *aRec, CupDetectorConstruction *aDet)
        : fDetConstruction(aDet), fRecorders(aRec){};
    virtual ~CupActionInitialization(){};

    virtual void BuildForMaster() const;
    virtual void Build() const;

  private:
    CupDetectorConstruction *fDetConstruction;

    CupRecorderBase *fRecorders;
};
#endif

#endif
