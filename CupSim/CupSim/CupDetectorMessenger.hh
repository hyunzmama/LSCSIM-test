
#ifndef __CupDetectorMessenger_hh__
#define __CupDetectorMessenger_hh__ 1

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class CupDetectorConstruction;

class CupDetectorMessenger : public G4UImessenger {
  public:
    CupDetectorMessenger(CupDetectorConstruction *myDetector);
    ~CupDetectorMessenger();

    void SetNewValue(G4UIcommand *command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand *command);

  private:
    CupDetectorConstruction *myDetector;
    G4UIdirectory *DetectorDir;
    G4UIcmdWithAString *PmtStyleCmd;
    G4UIcommand *DetectorSelectCmd;
    G4UIcommand *CalDeviceCmd;
    G4UIcmdWith3VectorAndUnit *CalPositionCmd;
};

#endif
