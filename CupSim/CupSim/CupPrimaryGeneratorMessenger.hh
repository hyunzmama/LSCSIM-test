
#ifndef __CupPrimaryGeneratorMessenger_hh__
#define __CupPrimaryGeneratorMessenger_hh__ 1

#include "G4UImessenger.hh"

class CupPrimaryGeneratorAction;
class G4UIcommand;

class CupPrimaryGeneratorMessenger : public G4UImessenger {
  public:
    CupPrimaryGeneratorMessenger(CupPrimaryGeneratorAction *myGun);
    ~CupPrimaryGeneratorMessenger();

    void SetNewValue(G4UIcommand *command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand *command);

  private:
    CupPrimaryGeneratorAction *myAction;

    G4UIdirectory *GenDir;
    G4UIcommand *ListCmd;
    G4UIcommand *RateCmd;
    G4UIcommand *GunCmd;
    G4UIcommand *VtxSetCmd;
    G4UIcommand *PosSetCmd;
    G4UIcommand *EventWindowCmd;
    G4UIcommand *ChainClipCmd;
    G4UIcommand *PileupCmd;
};

#endif
