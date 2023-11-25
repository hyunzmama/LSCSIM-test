// This file is part of the GenericLAND software library.
// $Id: CupDeferTrackProc.hh,v 1.1.1.1 2016/10/31 08:41:44 ejjeon Exp $
//
// Process to limit step length to stay within event time
// and defer long tracks (AND tracks which start after event time) using
// defered particle "generator".
//
// Written: G. Horton-Smith, 29-Oct-2001
//
#ifndef CupDeferTrackProc_h
#define CupDeferTrackProc_h 1

#include "G4VProcess.hh"
#include "G4ios.hh"
#include "globals.hh"

////////////////////////////////////////////////////////////////

class CupPrimaryGeneratorAction;
class G4HadronCaptureProcess;

using namespace CLHEP;

class CupDeferTrackProc : public G4VProcess {
  public: // with description
    CupDeferTrackProc(const G4String &processName = "DeferTrackProc");

    ~CupDeferTrackProc();

    virtual G4double PostStepGetPhysicalInteractionLength(const G4Track &track,
                                                          G4double previousStepSize,
                                                          G4ForceCondition *condition);

    virtual G4VParticleChange *PostStepDoIt(const G4Track &, const G4Step &);

  public: // without description
    //  no operation in  AtRestGPIL
    virtual G4double AtRestGetPhysicalInteractionLength(const G4Track &, G4ForceCondition *) {
        return -1.0;
    };

    //  no operation in  AtRestDoIt
    virtual G4VParticleChange *AtRestDoIt(const G4Track &, const G4Step &) { return NULL; };

    //  no operation in  AlongStepGPIL
    virtual G4double AlongStepGetPhysicalInteractionLength(const G4Track &, G4double, G4double,
                                                           G4double &, G4GPILSelection *) {
        return -1.0;
    };

    //  no operation in  AlongStepDoIt
    virtual G4VParticleChange *AlongStepDoIt(const G4Track &, const G4Step &) { return NULL; };

  private:
    // hide assignment operator as private
    CupDeferTrackProc(CupDeferTrackProc &);
    CupDeferTrackProc &operator=(const CupDeferTrackProc &right);

  private:
    CupPrimaryGeneratorAction *_generator;
};

#endif
