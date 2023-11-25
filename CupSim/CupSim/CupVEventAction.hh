
#ifndef CupVEventAction_h
#define CupVEventAction_h 1

#include "G4UImessenger.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

#include "fstream"
//#include "CupHitPhotonCollection.hh"
#include "CupHitPMTCollection.hh"

#include "G4DigiManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UImanager.hh"
#include "G4UnitsTable.hh"
#include "G4VHitsCollection.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

class G4Event;         // EJ
class CupRecorderBase; // EJ

class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class CupVEventAction : public G4UserEventAction, public G4UImessenger {
  public:
    // constructor, destructor
    //  CupVEventAction();
    CupVEventAction(CupRecorderBase *r = 0); // EJ
    ~CupVEventAction();

    // overrides for G4UserEventAction methods
    virtual void BeginOfEventAction(const G4Event *);
    virtual void EndOfEventAction(const G4Event *);

    // overrides for G4UImessenger methods
    virtual void SetNewValue(G4UIcommand *command, G4String newValue);
    G4String GetCurrentValue(G4UIcommand *command);

    //  static CupHitPhotonCollection*  GetTheHitPhotons() { return &theHitPhotons; }
    static CupHitPMTCollection *GetTheHitPMTCollection() { return &theHitPMTCollection; }
    static G4bool GetDoParameterizedScintillation() { return fgDoParameterizedScintillation; }

  private: // EJ
    // Save the CupRecorderBase object to be called by the UserEventAction.
    CupRecorderBase *recorder; // EJ

  protected:
    //  static CupHitPhotonCollection theHitPhotons;
    static CupHitPMTCollection theHitPMTCollection;

    static G4bool flagFullOutputMode;
    G4String drawFlag;

    // EJ
    /*
    public:
      virtual void OpenFile(const G4String filename,G4bool outputMode) = 0;
      virtual void CloseFile() = 0;
      virtual void FillData(const G4Event*) = 0;
      virtual void Clear() = 0;
    */

  protected:
    G4UIcmdWithAString *fDrawCmd;
    G4UIcommand *fFileCmd;
    G4UIcmdWithAString *fModeCmd;

    static G4bool fgDoParameterizedScintillation;
};

#endif
