#ifndef CupOpAttenuation_hh
#define CupOpAttenuation_hh

#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4PhysicsTable.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "templates.hh"

class G4VWLSTimeGeneratorProfile;

class CupOpAttenuation : public G4VDiscreteProcess {
  public:
    CupOpAttenuation(const G4String &processName = "Attenuation", G4ProcessType type = fOptical);
    ~CupOpAttenuation();

  private:
    CupOpAttenuation(const CupOpAttenuation &right);
    CupOpAttenuation &operator=(const CupOpAttenuation &right);
    void BuildThePhysicsTable();

  public:
    // Returns true -> 'is applicable' only for an optical photon.
    G4bool IsApplicable(const G4ParticleDefinition &aParticleType);

    // Returns the absorption length for bulk absorption of optical
    // photons in media with a specified attenuation length.
    G4double GetMeanFreePath(const G4Track &aTrack, G4double, G4ForceCondition *);

    // This is the method implementing bulk absorption of optical
    // photons.
    G4VParticleChange *PostStepDoIt(const G4Track &aTrack, const G4Step &aStep);

    // Returns the address of the WLS integral table.
    G4PhysicsTable *GetIntegralTable() const;

    // Prints the WLS integral table.
    void DumpPhysicsTable() const;

    // Selects the time profile generator
    void UseTimeProfile(const G4String name);

  protected:
    G4VWLSTimeGeneratorProfile *WLSTimeGeneratorProfile;
    G4PhysicsTable *theIntegralTable;
};

inline G4bool CupOpAttenuation::IsApplicable(const G4ParticleDefinition &aParticleType) {
    return (&aParticleType == G4OpticalPhoton::OpticalPhoton());
}

inline G4PhysicsTable *CupOpAttenuation::GetIntegralTable() const { return theIntegralTable; }

inline void CupOpAttenuation::DumpPhysicsTable() const {
    G4int PhysicsTableSize = theIntegralTable->entries();
    G4PhysicsOrderedFreeVector *v;

    for (G4int i = 0; i < PhysicsTableSize; i++) {
        v = (G4PhysicsOrderedFreeVector *)(*theIntegralTable)[i];
        v->DumpValues();
    }
}
#endif
