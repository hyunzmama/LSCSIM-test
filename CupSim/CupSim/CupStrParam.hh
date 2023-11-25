
#ifndef __CupStrParam_hh__
#define __CupStrParam_hh__

#include "G4Version.hh"
#include "globals.hh"
#include "iostream"
#include "map"

class CupStrParam : public std::map<G4String, G4String> {
  private:
    CupStrParam(); // singleton
#if G4VERSION_NUMBER >= 1000
    G4ThreadLocal static CupStrParam *theCupStrParam;
    static CupStrParam *theMasterCupStrParam;
#else
    static CupStrParam *theCupStrParam;
    static CupStrParam *theMasterCupStrParam;
#endif

  public:
    enum EOverride { kKeepExistingValue, kOverrideExistingValue };
    static inline CupStrParam &GetDB();
    static inline CupStrParam *GetDBPtr();
    void ReadFile(const char *filename, EOverride oflag = kKeepExistingValue);
    void ReadFile(std::istream &is, EOverride oflag = kKeepExistingValue);
    void WriteFile(std::ostream &os);
    inline G4String GetWithDefault(const G4String &name, G4String defaultValue);
};

// inline functions
CupStrParam *CupStrParam::GetDBPtr() {
    // Get (and possibly create) the pointer to the singleton
    //----------------
    if (theCupStrParam == nullptr) {
        if (theMasterCupStrParam == nullptr) {
            theMasterCupStrParam = new CupStrParam();
            theCupStrParam       = theMasterCupStrParam;
        } else {
            theCupStrParam = new CupStrParam(*theMasterCupStrParam);
        }
    }
    return theCupStrParam;
}

CupStrParam &CupStrParam::GetDB() { return *GetDBPtr(); }

G4String CupStrParam::GetWithDefault(const G4String &name, G4String defaultValue) {
    if (count(name))
        return (*this)[name];
    else
        return (*this)[name] = defaultValue;
}

#endif
