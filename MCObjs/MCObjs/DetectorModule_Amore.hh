/**
 * @file DetectorModule_Amore.hh
 * @brief A header file for class DetectorModule_Amore
 * @author BaseHardware (basehw)
 * @date 2019-04-06
 * */
#ifndef __AMOREDETECTORMODULE__H_
#define __AMOREDETECTORMODULE__H_

#include <vector>

#include "Rtypes.h"

#include "MCObjs/DetectorModule.hh"

/**
 * @class DetectorModule_Amore
 * @brief A class for recording energy deposit information on detector module for AMoRE simulation
 * @author BaseHardware (basehw)
 * @date 2019-04-06
 * */
class DetectorModule_Amore : public DetectorModule {
  public:
    /// Default constructor
    DetectorModule_Amore() : DetectorModule() {
        fGoldFilmNum = 0;
        fGeWaferEdep = fQuenchedGeWaferEdep = -1;
    };
    DetectorModule_Amore(TString aName, TString aTitle, Int_t aModuleID, Int_t aGoldFilmNum);
    virtual ~DetectorModule_Amore() { Clear("A"); }

    /// Set energy deposit on germanium wafer
    inline void SetGeWaferEdep(Double_t aEdep) { fGeWaferEdep = aEdep; }
    /// Set quenched energy deposit on germanium wafer
    inline void SetQuenchedGeWaferEdep(Double_t aEdep) { fQuenchedGeWaferEdep = aEdep; }
    /// Set total number of gold films
    inline void SetTotalNumberOfGoldFilm(Int_t aNum);
    /// Set energy deposit on a gold film with index
    inline Bool_t SetGoldFilmEdep(Int_t aGIdx, Double_t aEdep);

    /// Get energy deposit on germanium wafer
    inline Double_t GetGeWaferEdep() const { return fGeWaferEdep; }
    /// Get quenched energy deposit on germanium wafer
    inline Double_t GetQuenchedGeWaferEdep() const { return fQuenchedGeWaferEdep; }
    /// Get total number of gold films
    inline Int_t GetTotalNumberOfGoldFilm() const { return fGoldFilmNum; }
    /// Get energy deposit on a gold film with index
    inline Double_t GetGoldFilmEdep(Int_t aGIdx) const;

    virtual Double_t &operator[](Int_t aIdx);

    void Clear(Option_t *aOption = "");

  private:
    /// Total number of gold films. It should be matched with fGoldFilmEdep.size()
    Int_t fGoldFilmNum;
    /// Array for energy deposit
    std::vector<Double_t> fGoldFilmEdep;

    /// Energy deposit on germanium wafer
    Double_t fGeWaferEdep;
    /// Quenched energy deposit on germanium wafer
    Double_t fQuenchedGeWaferEdep;

  protected:
    /// Dummy variable for [] operator
    Double_t fDummy;
    virtual void ResizeEdepArray();

  public:
    /// Overriding of TObject::IsSortable
    /// @return Always returns false
    Bool_t IsSortable() const { return false; }

    ClassDef(DetectorModule_Amore, 1);
};

inline Double_t DetectorModule_Amore::GetGoldFilmEdep(Int_t aGIdx) const {
    if (fGoldFilmNum > 0 && MACRO_IS_IN_RANGE_OF(0, aGIdx, fGoldFilmNum))
        return fGoldFilmEdep[aGIdx];
    else
        return -1;
}

inline Bool_t DetectorModule_Amore::SetGoldFilmEdep(Int_t aGIdx, Double_t aEdep) {
    if (fGoldFilmNum > 0 && MACRO_IS_IN_RANGE_OF(0, aGIdx, fGoldFilmNum)) {
        fGoldFilmEdep[aGIdx] = aEdep;
        return true;
    } else
        return false;
}

inline void DetectorModule_Amore::SetTotalNumberOfGoldFilm(Int_t aNum) {
    fGoldFilmNum = aNum;
    ResizeEdepArray();
}

#endif
