//
// CupRootNtupleMessenger.hh
//
#ifndef __CupRootNtupleMessenger_hh__
#define __CupRootNtupleMessenger_hh__ 1

#include "G4UImessenger.hh"

class G4UIcommand;
class CupRootNtuple;

class CupRootNtupleMessenger : public G4UImessenger {
  public:
    CupRootNtupleMessenger(CupRootNtuple *myNtuple);
    ~CupRootNtupleMessenger();

    void SetNewValue(G4UIcommand *command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand *command);

  private:
    CupRootNtuple *myNtuple;
    G4UIdirectory *RootNtupleDir;
    G4UIcommand *NtuplePrimary;
    G4UIcommand *NtupleTrack;
    G4UIcommand *NtupleStep;
    G4UIcommand *NtuplePhoton;
    G4UIcommand *NtupleScint;
    G4UIcommand *NtupleMuon;
};

#endif
