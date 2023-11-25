// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: CupRunAction.hh,v 1.1.1.1 2016/10/31 08:41:44 ejjeon Exp $
// GEANT4 tag $Name:  $
//
//

#ifndef CupRunAction_h
#define CupRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class CupRecorderBase;

class CupRunAction : public G4UserRunAction {
  public:
    // If the constructor is called with a RecordBase arguement,
    // then we'll perform some record keeping in the user action classes.
    CupRunAction(CupRecorderBase *r = 0);
    virtual ~CupRunAction();

  public:
    virtual void BeginOfRunAction(const G4Run *aRun);
    virtual void EndOfRunAction(const G4Run *aRun);

  private:
    G4int runIDcounter;
    // Save the CupRecorderBase object to be called at the beginning
    // and end of the run.
    CupRecorderBase *recorder;
};

#endif
