
#include "CupSim/CupPhysicsListMessenger.hh"

#include "CupSim/CupPhysicsList.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CupPhysicsListMessenger::CupPhysicsListMessenger(CupPhysicsList *pPhys) : pPhysicsList(pPhys) {
    physDir = new G4UIdirectory("/Cup/phys/");
    physDir->SetGuidance("PhysicsList control");

    gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/Cup/phys/CutGamma", this);
    gammaCutCmd->SetGuidance("Set gamma cut.");
    gammaCutCmd->SetParameterName("Gcut", false);
    gammaCutCmd->SetUnitCategory("Length");
    gammaCutCmd->SetRange("Gcut>0.0");
    gammaCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    electCutCmd = new G4UIcmdWithADoubleAndUnit("/Cup/phys/CutEl", this);
    electCutCmd->SetGuidance("Set electron cut.");
    electCutCmd->SetParameterName("Ecut", false);
    electCutCmd->SetUnitCategory("Length");
    electCutCmd->SetRange("Ecut>0.0");
    electCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    posCutCmd = new G4UIcmdWithADoubleAndUnit("/Cup/phys/CutPos", this);
    posCutCmd->SetGuidance("Set positron cut.");
    posCutCmd->SetParameterName("Pcut", false);
    posCutCmd->SetUnitCategory("Length");
    posCutCmd->SetRange("Pcut>0.0");
    posCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    allCutCmd = new G4UIcmdWithADoubleAndUnit("/Cup/phys/CutsAll", this);
    allCutCmd->SetGuidance("Set cut for all.");
    allCutCmd->SetParameterName("cut", false);
    allCutCmd->SetUnitCategory("Length");
    allCutCmd->SetRange("cut>0.0");
    allCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    mCutCmd = new G4UIcmdWithADoubleAndUnit("/Cup/phys/DetectorCuts", this);
    mCutCmd->SetGuidance("Set cuts for the Detector");
    mCutCmd->SetParameterName("Ecut", false);
    mCutCmd->SetUnitCategory("Length");
    mCutCmd->SetRange("Ecut>0.0");
    mCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    pListCmd = new G4UIcmdWithAString("/Cup/phys/Physics", this);
    pListCmd->SetGuidance("Add modula physics list.");
    pListCmd->SetParameterName("PList", false);
    pListCmd->AvailableForStates(G4State_PreInit);

    listCmd = new G4UIcmdWithoutParameter("/Cup/phys/ListPhysics", this);
    listCmd->SetGuidance("Available Physics Lists");
    listCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    verboseCmd = new G4UIcmdWithAnInteger("/Cup/phys/verbose", this);
    verboseCmd->SetGuidance("set verbose for physics processes");
    verboseCmd->SetParameterName("verbose", true);
    verboseCmd->SetDefaultValue(1);
    verboseCmd->SetRange("verbose>=0");
    verboseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    yieldfactorCmd = new G4UIcmdWithADouble("/Cup/phys/alphaYieldFactor", this);
    yieldfactorCmd->SetGuidance("Scintillation Yield factor for alpha");
    yieldfactorCmd->SetParameterName("aYieldFactor", false);
    yieldfactorCmd->SetRange("aYieldFactor>0.0");
    yieldfactorCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    CrystalRegionCmd = new G4UIcommand("/Cup/phys/EnableCrystalRegion", this);
    CrystalRegionCmd->SetGuidance("Enable Crystal Region");
    CrystalRegionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    CrystalRegionCmd->SetParameter(new G4UIparameter("EnableCrystalRegion", 'b', true));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CupPhysicsListMessenger::~CupPhysicsListMessenger() {
    delete gammaCutCmd;
    delete electCutCmd;
    delete posCutCmd;
    delete allCutCmd;
    delete mCutCmd;
    delete pListCmd;
    delete listCmd;
    delete CrystalRegionCmd;

    delete verboseCmd;
    delete physDir;
    delete yieldfactorCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CupPhysicsListMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
    if (command == gammaCutCmd)
        pPhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));
    else if (command == electCutCmd)
        pPhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));
    else if (command == posCutCmd)
        pPhysicsList->SetCutForPositron(posCutCmd->GetNewDoubleValue(newValue));
    else if (command == allCutCmd) {
        G4double cut = allCutCmd->GetNewDoubleValue(newValue);
        pPhysicsList->SetCutForGamma(cut);
        pPhysicsList->SetCutForElectron(cut);
        pPhysicsList->SetCutForPositron(cut);
    } else if (command == pListCmd) {
        G4String name = newValue;
        if (name == "PHYSLIST") {
            char *path = getenv(name);
            if (path)
                name = G4String(path);
            else {
                G4cout << "### CupPhysicsListMessenger WARNING: "
                       << " environment variable PHYSLIST is not defined" << G4endl;
                return;
            }
        }
        pPhysicsList->AddPhysicsList(name);
    } else if (command == mCutCmd) {
        pPhysicsList->SetDetectorCut(mCutCmd->GetNewDoubleValue(newValue));
    } else if (command == listCmd)
        pPhysicsList->List();
    else if (command == CrystalRegionCmd)
        pPhysicsList->SetEnableCrystalRegion(StoB(newValue));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
