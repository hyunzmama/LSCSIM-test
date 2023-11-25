#include <cstring>
#include <iostream>

#include "MCObjs/DetectorModule.hh"

ClassImp(DetectorModule);

void DetectorModule::Clear(Option_t *aOption) {
    if (strncmp(aOption, "A", 1) == 0) {
        fModuleID            = -1;
        fCrystalEdep         = 0;
        fQuenchedCrystalEdep = 0;
        TNamed::Clear(aOption);
    } else {
        fCrystalEdep = 0;
        fQuenchedCrystalEdep = 0;
    }
}
