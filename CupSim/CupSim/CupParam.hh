
#ifndef __CupParam_hh__
#define __CupParam_hh__

#include "G4Version.hh"
#include "globals.hh"
#include "iostream"
#include "map"

class CupParam : public std::map<G4String, G4double> {
  private:
    CupParam(); // singleton
#if G4VERSION_NUMBER >= 1000
    G4ThreadLocal static CupParam *theCupParam;
    static CupParam *theMasterCupParam;
#else
    static CupParam *theCupParam;
    static CupParam *theMasterCupParam;
#endif

  public:
    enum EOverride { kKeepExistingValue, kOverrideExistingValue };
    static inline CupParam &GetDB();
    static inline CupParam *GetDBPtr();
    void ReadFile(const char *filename, EOverride oflag = kKeepExistingValue);
    void ReadFile(std::istream &is, EOverride oflag = kKeepExistingValue);
    void WriteFile(std::ostream &os);
    inline G4double GetWithDefault(G4String name, G4double defaultValue);
};

// inline functions
CupParam *CupParam::GetDBPtr() {
    // Get (and possibly create) the pointer to the singleton
    //----------------
    if (theCupParam == nullptr) {
        if (theMasterCupParam == nullptr) {
            theMasterCupParam = new CupParam();
            theCupParam       = theMasterCupParam;
        } else {
            theCupParam = new CupParam(*theMasterCupParam);
        }
    }
    return theCupParam;
}

CupParam &CupParam::GetDB() { return *GetDBPtr(); }

G4double CupParam::GetWithDefault(G4String name, G4double defaultValue) {
    if (count(name))
        return (*this)[name];
    else
        return (*this)[name] = defaultValue;
}

#endif
