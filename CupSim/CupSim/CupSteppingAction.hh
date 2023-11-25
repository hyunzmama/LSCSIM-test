// This file is part of the GenericLAND software library.
// $Id: CupSteppingAction.hh,v 1.1.1.1 2016/10/31 08:41:44 ejjeon Exp $
//
#ifndef __CupSteppingAction_H__
#define __CupSteppingAction_H__ 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class CupPrimaryGeneratorAction;
class CupRecorderBase; // EJ

class CupSteppingAction : public G4UserSteppingAction {
  public:
    CupSteppingAction(CupRecorderBase *r, CupPrimaryGeneratorAction *p); // EJ
    CupSteppingAction(CupRecorderBase *r);                               // EJ
    virtual ~CupSteppingAction(){};                                      // EJ

    // void UserSteppingAction(const G4Step* aStep);
    virtual void UserSteppingAction(const G4Step *aStep);

  protected:
    CupPrimaryGeneratorAction *myGenerator;
    CupRecorderBase *recorder; // EJ
};

#endif
