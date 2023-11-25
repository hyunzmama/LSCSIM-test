
#ifndef __CupDebugMessenger_hh__
#define __CupDebugMessenger_hh__ 1

#include "G4UImessenger.hh"

class G4UIcommand;
class CupDetectorConstruction;

class CupDebugMessenger : public G4UImessenger {
  public:
    CupDebugMessenger(CupDetectorConstruction *myDetector);
    ~CupDebugMessenger();

    void SetNewValue(G4UIcommand *command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand *command);

  private:
    CupDetectorConstruction *myDetector;
    G4UIdirectory *DebugDir;
    G4UIcommand *DumpMaterialsCmd;
    G4UIcommand *DumpGeomCmd;
    G4UIcommand *matcmd;
    G4UIcommand *dovercmd;
    G4UIcommand *dreadcmd;
    G4UIcommand *ddumpcmd;
    G4UIcommand *delemcmd;
    G4UIcommand *seedcmd;
    G4UIcommand *neutcmd;
    G4UIcommand *runIDcmd;
#ifdef G4DEBUG
    G4UIcommand *illucmd;
#endif
};

#endif
