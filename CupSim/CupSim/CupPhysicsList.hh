#ifndef CupPhysicsList_h
#define CupPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// EJ
class G4VPhysicsConstructor;
class CupPhysicsListMessenger;

class CupPhysicsList : public G4VModularPhysicsList {
  public:
    CupPhysicsList();
    ~CupPhysicsList();

  public:
    virtual void SetCuts();

    // EJ: for Messenger
    void SetCutForGamma(G4double);
    void SetCutForElectron(G4double);
    void SetCutForPositron(G4double);
    void SetDetectorCut(G4double val);
    void List();

    virtual void AddPhysicsList(const G4String &name);

    static inline void SetEnableCrystalRegion(G4bool a) { enableCrystalRegion = a; }
    static inline G4bool GetEnableCrystalRegion() { return enableCrystalRegion; }

  protected:
    // Construct particle and physics
    virtual void ConstructParticle();
    virtual void ConstructProcess();

    // these methods Construct physics processes and register them
    virtual void ConstructGeneral();
    virtual void ConstructEM();
    virtual void ConstructHad();
    virtual void ConstructOp();

    virtual void AddParameterisation();

  private:
    static G4bool omitHadronicProc;
    static G4bool omitNeutHP;
    static G4bool enableCrystalRegion;

    static G4int VerboseLevel;
    static G4int OpVerbLevel;

    static G4double cutForGamma;
    static G4double cutForElectron;
    static G4double cutForPositron;
    static G4double cutForProton;
    static G4double cutForAlpha;
    static G4double cutForGenericIon;

    G4VPhysicsConstructor *emPhysicsList;
    G4String emName;
    G4VPhysicsConstructor *opPhysicsList;
    G4String opName;

    // these methods Construct particles
    void ConstructMyBosons();
    void ConstructMyLeptons();
    void ConstructMyHadrons();
    void ConstructMyShortLiveds();

    // EJ: for Messenger
    CupPhysicsListMessenger *pMessenger;

    static G4ProductionCuts *DetectorCuts;
};

#endif
