
#ifdef G4VIS_USE
#ifndef CupVISMESSENGER_HH
#define CupVISMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4VisManager.hh"

class CupVisMessenger : public G4UImessenger {
  public:
    CupVisMessenger(G4VisManager *pVMan_);
    ~CupVisMessenger();

    void SetNewValue(G4UIcommand *command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand *command);

  protected:
    G4VisManager *pVMan;
    G4UIdirectory *Dir;
    G4UIcommand *CameraResetCmd;
    G4UIcommand *UpVectorCmd;
};

#endif
#endif
