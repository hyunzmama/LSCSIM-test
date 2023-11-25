#include <iostream>

#include "MCObjs/DetectorArray_Amore.hh"

ClassImp(DetectorArray_Amore);

using namespace std;

DetectorArray_Amore::DetectorArray_Amore(TString aName, TString aTitle,
                                         const vector<string> &aNameList4Modules)
    : TNamed(aName, aTitle), fModuleArray(nullptr), fModuleNum(-1) {
    if (aNameList4Modules.size() == 0) {
        Error(__func__,
              "Name list of crystal detector modules is empty. Array will not be initialized.");
        return;
    } else {
        InitializeModuleArray(aNameList4Modules);
    }
}

DetectorArray_Amore::DetectorArray_Amore(TString aName, TString aTitle,
                                         const vector<string> &aNameList4Modules,
                                         const vector<string> &aTitleList4Modules)
    : TNamed(aName, aTitle), fModuleArray(nullptr), fModuleNum(-1) {
    if (aNameList4Modules.size() == 0) {
        Error(__func__,
              "Name list of crystal detector modules is empty. Array will not be initialized.");
        return;
    } else if (aNameList4Modules.size() != aTitleList4Modules.size()) {
        Info(
            __func__,
            "The size of list for name and title is not matching. The title list will be ignored.");
        InitializeModuleArray(aNameList4Modules);
    } else {
        InitializeModuleArray(aNameList4Modules, aTitleList4Modules);
    }
}

Bool_t DetectorArray_Amore::InitializeModuleArray(const vector<string> &aNameList,
                                                  const vector<string> &aTitleList) {
    vector<string> tempTitleList(aNameList.size());
    const vector<string> *selectedTitleListPtr;
    if (aTitleList.size() == 0) {
        for (size_t i = 0; i < aNameList.size(); i++)
            tempTitleList[i] = "A detector module for crystal: " + aNameList[i];
        selectedTitleListPtr = &tempTitleList;
    } else
        selectedTitleListPtr = &aTitleList;

    const vector<string> &selectedTitleList = *selectedTitleListPtr;

    fModuleNum   = aNameList.size();
    fModuleArray = new TClonesArray("DetectorModule_Amore", fModuleNum);

    for (size_t i = 0; i < aNameList.size(); i++)
        new ((*fModuleArray)[i]) DetectorModule_Amore(aNameList[i], selectedTitleList[i], i, 4);

    return fModuleNum == fModuleArray->GetEntries();
}

DetectorModule_Amore &DetectorArray_Amore::operator[](Int_t aIdx) {
    if (fModuleNum != 0 && MACRO_IS_IN_RANGE_OF(0, aIdx, fModuleNum)) {
        TObject *nowObjectPtr = (*fModuleArray)[aIdx];
        return static_cast<DetectorModule_Amore &>(*nowObjectPtr);
    } else {
        Error(__func__, "The module is empty or the given index is out of range of module array. "
                        "Reference to a dummy variable will be returned.");
        return fDummy;
    }
}

Bool_t DetectorArray_Amore::ReInitializeWith(const vector<string> &aNameList,
                                             const vector<string> &aTitleList) {
    if (fModuleNum != 0) {
        Info(__func__, "The array of module is not empty. Please call this function when array is "
                       "vacant. (after calling Clear(\"A\"))");
        return false;
    } else {
        if (aTitleList.size() > 0 && aNameList.size() != aTitleList.size()) {
            Info(__func__, "The size of list for name and title is not matching. The title list "
                           "will be ignored.");
            return InitializeModuleArray(aNameList);
        } else
            return InitializeModuleArray(aNameList, aTitleList);
    }
}

void DetectorArray_Amore::Clear(Option_t *aOption) {
    if (fModuleArray == nullptr) {
        TNamed::Clear(aOption);
        fModuleNum = -1;
        return;
    }
    if (strncmp(aOption, "A", 1) == 0) {
        fModuleArray->Delete();
        fModuleNum = 0;
        TNamed::Clear(aOption);
    } else {
        for (auto nowObject : *fModuleArray) {
            DetectorModule_Amore *nowModule = static_cast<DetectorModule_Amore *>(nowObject);
            nowModule->Clear(aOption);
        }
    }
}
