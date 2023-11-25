/**
 * @file DetectorModule_Amore.cc
 * @brief Source code file for implementation of class DetectorModule_Amore
 * @author BaseHardware (basehw)
 * @date 2019-04-06
 * */
#include <iostream>

#include "MCObjs/DetectorModule_Amore.hh"

ClassImp(DetectorModule_Amore);

/// Constructor with name, title, moduleID, and total number of gold films
DetectorModule_Amore::DetectorModule_Amore(TString aName, TString aTitle, Int_t aModuleID,
                                           Int_t aGoldFilmNum)
    : DetectorModule(aName, aTitle, aModuleID) {
    fQuenchedGeWaferEdep = fGeWaferEdep = 0.;

    fGoldFilmNum = aGoldFilmNum;
    ResizeEdepArray();
}

/**
 * @brief Clear an object of this class
 * @param aOption, Only "A" will be processed, which means that clear an array of gold film
 * @return No return value
 * */
void DetectorModule_Amore::Clear(Option_t *aOption) {
    fQuenchedGeWaferEdep = fGeWaferEdep = 0.;
    if (strncmp(aOption, "A", 1) == 0) {
        fGoldFilmNum = 0;
        fGoldFilmEdep.clear();
    } else {
        for (auto &nowVal : fGoldFilmEdep)
            nowVal = 0.;
    }
    DetectorModule::Clear(aOption);
}

/**
 * @brief Resize an array of energt deposit on golf films.
 * @return No return value
 * */
void DetectorModule_Amore::ResizeEdepArray() {
    if (fGoldFilmNum < 0) fGoldFilmNum = 0;
    fGoldFilmEdep.resize(fGoldFilmNum, 0.);
}

/**
 * @brief index operator for accessing goldfilm edep array
 * @param aIdx index of elements in the array
 * @return Returns reference to the elements
 * */
Double_t &DetectorModule_Amore::operator[](Int_t aIdx) {
    if (fGoldFilmNum > 0 && MACRO_IS_IN_RANGE_OF(0, aIdx, fGoldFilmNum))
        return fGoldFilmEdep[aIdx];
    else {
        Error(__func__,
              "Wrong index for accessing gold film of \"%s: %s\". Reference to a dummy variable "
              "will be returned.",
              fName.Data(), fTitle.Data());
        return fDummy;
    }
}
