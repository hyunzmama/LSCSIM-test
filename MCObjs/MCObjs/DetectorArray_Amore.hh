// DetectorArray_Amore Class
// A class contains array of DetectorModule_Amore
// Author: BaseHardware
#ifndef __AMOREDETECTORARRAY__H_
#define __AMOREDETECTORARRAY__H_

#include <string>
#include <vector>

#include "Rtypes.h"
#include "TNamed.h"

#include "TClonesArray.h"

#include "MCObjs/DetectorModule_Amore.hh"

class DetectorArray_Amore : public TNamed {
  public:
    DetectorArray_Amore()
        : TNamed("EMPTY", "An empty array for crystal detector modules of AMoRE simulation"),
          fModuleArray(nullptr), fModuleNum(-1){};
    DetectorArray_Amore(TString aName, TString aTitle,
                        const std::vector<std::string> &aNameList4Modules);
    DetectorArray_Amore(TString aName, TString aTitle,
                        const std::vector<std::string> &aNameList4Modules,
                        const std::vector<std::string> &aTitleList4Modules);

    virtual ~DetectorArray_Amore() {
        Clear("A");
        delete fModuleArray;
    };

    DetectorModule_Amore &operator[](Int_t aIdx);
    inline DetectorModule_Amore &GetDetectorModule(Int_t aIdx);
    inline DetectorModule_Amore *GetDetectorModulePtr(Int_t aIdx);

    Bool_t ReInitializeWith(const std::vector<std::string> &aNameList,
                            const std::vector<std::string> &aTitleList = {});

    inline Int_t GetTotalNumOfDetectorModules() const { return fModuleNum; }

    void Clear(Option_t *aOption = "");

  protected:
    Bool_t InitializeModuleArray(const std::vector<std::string> &aNameList4Modules,
                                 const std::vector<std::string> &aTitleList4Modules = {});

    TClonesArray *GetModuleArray() const { return fModuleArray; }

  private:
    TClonesArray *fModuleArray;
    Int_t fModuleNum;

  public:
    DetectorModule_Amore fDummy; //! Dummy variable
    ClassDef(DetectorArray_Amore, 1);
};

DetectorModule_Amore &DetectorArray_Amore::GetDetectorModule(Int_t aIdx) { return (*this)[aIdx]; }

DetectorModule_Amore *DetectorArray_Amore::GetDetectorModulePtr(Int_t aIdx) {
    return &((*this)[aIdx]);
}

#endif
