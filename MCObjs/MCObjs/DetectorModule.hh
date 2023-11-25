// DetectorModule Class
// A class for recording an essential information for crystal.
// Author: BaseHardware

#ifndef __CRYSTALDETECTORMODULE__H_
#define __CRYSTALDETECTORMODULE__H_

#include <vector>

#include "Rtypes.h"

#include "TClonesArray.h"
#include "TNamed.h"

#define MACRO_IS_IN_RANGE_OF(A, B, C) (((A) <= (B)) && ((B) < (C)))

class DetectorModule : public TNamed {
  public:
    DetectorModule() : TNamed("EMPTY", "An empty crystal detector module object."), fModuleID(-1) {}
    DetectorModule(TString aModuleName, TString aModuleTitle, Int_t aModuleID)
        : TNamed(aModuleName, aModuleTitle), fModuleID(aModuleID) {}
    virtual ~DetectorModule() {}

    inline void SetModuleID(Int_t aMID) { fModuleID = aMID; }
    inline Int_t GetModuleID() const { return fModuleID; }

    inline void SetCrystalEdep(Double_t aEdep) { fCrystalEdep = aEdep; }
    inline void SetQuenchedCrystalEdep(Double_t aEdep) { fQuenchedCrystalEdep = aEdep; }

    inline Double_t GetCrystalEdep() const { return fCrystalEdep; }
    inline Double_t GetQuenchedCrystalEdep() const { return fQuenchedCrystalEdep; }

    void Clear(Option_t *aOption = "");

  private:
    Int_t fModuleID;
    Double_t fCrystalEdep;
    Double_t fQuenchedCrystalEdep;

  public:
    Bool_t IsSortable() const { return false; }

    ClassDef(DetectorModule, 1);
};
#endif
