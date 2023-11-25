
#include "CupSim/CupDebugMessenger.hh"
#include "CupSim/CupDetectorConstruction.hh"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4Timer.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "globals.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4HadronCaptureProcess.hh"
#include "G4HadronElasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4ProcessTable.hh"

#include "G4RunManager.hh"

#include "CupSim/CupParam.hh"

#include <fstream>  // for file streams
#include <iomanip>  // for setw(), etc..
#include <sstream>  // for string streams
#include <stdlib.h> // for strtol

using namespace std;

CupDebugMessenger::CupDebugMessenger(CupDetectorConstruction *mydetector) : myDetector(mydetector) {
    // the cupdebug directory
    DebugDir = new G4UIdirectory("/cupdebug/");
    DebugDir->SetGuidance("User-added debugging, tests, and diagnostics.");

    // the dumpmat command
    DumpMaterialsCmd = new G4UIcommand("/cupdebug/dumpmat", this);
    DumpMaterialsCmd->SetGuidance("Dump entire materials table or one material's properties table");

    G4UIparameter *aParam;
    aParam = new G4UIparameter("material", 's', true); // omittable
    DumpMaterialsCmd->SetParameter(aParam);

    // the dumpgeom command
    DumpGeomCmd = new G4UIcommand("/cupdebug/dumpgeom", this);
    DumpGeomCmd->SetGuidance("Dump the geometry information for the entire detector");
    aParam = new G4UIparameter("physicalVolume", 's', true); // omittable
    DumpGeomCmd->SetParameter(aParam);

    // the setmaterial commmand
    matcmd = new G4UIcommand("/cupdebug/setmaterial", this);
    matcmd->SetGuidance("Change the material in a region");
    matcmd->SetParameter(new G4UIparameter("logicalVolume", 's', false));
    matcmd->SetParameter(new G4UIparameter("material", 's', false));

    // the database override commmand
    dovercmd = new G4UIcommand("/cupdebug/cupparam", this);
    dovercmd->SetGuidance("Inspect or modify the options and values database");
    dovercmd->SetParameter(new G4UIparameter("identifier", 's', false));
    dovercmd->SetParameter(new G4UIparameter("value", 'd', true));

    // the database read commmand
    dreadcmd = new G4UIcommand("/cupdebug/cupparam_read", this);
    dreadcmd->SetGuidance("Read name/value pairs from a file into option/value db");
    dreadcmd->SetParameter(new G4UIparameter("filename", 's', false));

    // the database dump commmand
    ddumpcmd = new G4UIcommand("/cupdebug/cupparam_dump", this);
    ddumpcmd->SetGuidance("Dump the option/value database (to terminal or a file)");
    ddumpcmd->SetParameter(new G4UIparameter("filename", 's', true));

    // the dumpelem command
    delemcmd = new G4UIcommand("/cupdebug/dumpelem", this);
    delemcmd->SetGuidance("Dump entire element table or one element's properties");
    delemcmd->SetParameter(new G4UIparameter("name", 's', true));

    // the setseed command
    seedcmd = new G4UIcommand("/cupdebug/setseed", this);
    seedcmd->SetGuidance("Change random number generator state using setSeed(seed, luxury).");
    seedcmd->SetGuidance("The \"luxury\" parameter may be omitted; the default random number\n"
                         "generator doesn't use it anyway.\n"
                         "There is no getseed command, for Very Good Reasons, so use Geant4's\n"
                         "/run/storeRandomNumberStatus and restoreRandomNumberStatus commands\n"
                         "for saving/restoring the generator status in general.");
    seedcmd->SetParameter(new G4UIparameter("seed", 'd', false));
    seedcmd->SetParameter(new G4UIparameter("luxury_level", 'd', true));

    // the dumpNeutronCrossSections command
    neutcmd = new G4UIcommand("/cupdebug/dumpNeutronCrossSections", this);
    neutcmd->SetGuidance("Dump cross-sections (in a form suitable for Gnuplot)");
    neutcmd->SetParameter(new G4UIparameter("element", 's', false));
    neutcmd->SetParameter(new G4UIparameter("E1", 'd', true));
    neutcmd->SetParameter(new G4UIparameter("E2", 'd', true));
    neutcmd->SetParameter(new G4UIparameter("temperature", 'd', true));

    // the SetRunIDCounter command
    runIDcmd = new G4UIcommand("/cupdebug/SetRunIDCounter", this);
    runIDcmd->SetGuidance("Set Geant4 run number for next run");
    runIDcmd->SetParameter(new G4UIparameter("number", 'i', false));

#ifdef G4DEBUG
    // illuminationMap
    illucmd = new G4UIcommand("/cupdebug/dump_illumination_map", this);
    illucmd->SetGuidance("Dump a pretty picture of particle hits, for debugging");
#endif
}

CupDebugMessenger::~CupDebugMessenger() {
    delete DebugDir;
    delete DumpMaterialsCmd;
    delete DumpGeomCmd;
    delete matcmd;
    delete dovercmd;
    delete dreadcmd;
    delete ddumpcmd;
    delete delemcmd;
    delete seedcmd;
    delete neutcmd;
    delete runIDcmd;
#ifdef G4DEBUG
    delete illucmd;
#endif
}

static void DumpGeom(G4VPhysicalVolume *pv, const char *s) {
    G4cout << "*******************************\n";
    G4cout << "Physical volume dump for " << s << G4endl;
    G4cout << " Name: " << pv->GetName() << G4endl;

    G4LogicalVolume *lv = pv->GetLogicalVolume();
    G4cout << " Logical volume name: " << lv->GetName() << G4endl;
    G4cout << " Solid name: " << lv->GetSolid()->GetName() << G4endl;

    G4Material *m = lv->GetMaterial();
    G4cout << " Material: " << m->GetName() << G4endl;
    G4cout << (*m) << G4endl;

    ///  G4PhysicalVolume::GetMother() was removed in Geant4 version 06.
    // G4VPhysicalVolume* mother= pv->GetMother();
    // if ( mother )
    //   G4cout << "Mother volume is " << mother->GetName() << G4endl;
    // else
    //   G4cout << "Has no mother!" << G4endl;

    G4int ndaught = lv->GetNoDaughters();
    if (ndaught == 0) {
        G4cout << "Has no daughters." << G4endl;
    } else {
        G4cout << "Has " << ndaught << " daughters:\n";
        for (G4int i = 0; i < ndaught; i++)
            G4cout << "\t" << lv->GetDaughter(i)->GetName();
        G4cout << G4endl;
    }
    G4cout.flush();
}

static void SetMaterial(G4String newValues) {
    // parse out names
    std::istringstream iss(newValues.c_str());
    G4String lvName;
    G4String matName;
    iss >> lvName >> matName;
    if (iss.fail()) {
        G4cerr << "Could not parse volume and material name from command args\n";
        G4cerr.flush();
        return;
    }

    // access the store of logical volumes
    G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
    G4LogicalVolume *lv       = NULL;
    G4int nlv                 = lvs->size();
    G4int ilv;
    for (ilv = 0; ilv < nlv; ilv++) {
        lv = (*lvs)[ilv];
        if (!lv) break;
        if (lv->GetName() == lvName) break;
    }
    if (lv == NULL || ilv >= nlv) { // not found
        G4cerr << "Error, logical volume named \'" << lvName << "\' not found\n";
        G4cerr.flush();
        return;
    }

    // access the store of materials
    G4Material *mat = G4Material::GetMaterial(matName);
    if (mat == NULL) {
        G4cerr << "Error, material named \'" << matName << "\' not found\n";
        G4cerr.flush();
        return;
    }

    // set the material
    lv->SetMaterial(mat);
    G4cout << "Set material of " << lv->GetName() << " to " << mat->GetName() << G4endl;
    G4cout.flush();
}

static void DumpNeutronCrossSections(G4Element *elem, double e1, double e2, double temp) {
    G4ProcessTable *ptable = G4ProcessTable::GetProcessTable();

    G4HadronicProcess *theHadronElasticProcess =
        (G4HadronicProcess *)(ptable->FindProcess("LElastic", "neutron"));
    G4HadronicProcess *theNeutronInelasticProcess =
        (G4HadronicProcess *)(ptable->FindProcess("NeutronInelastic", "neutron"));
    G4HadronicProcess *theHadronFissionProcess =
        (G4HadronicProcess *)(ptable->FindProcess("LFission", "neutron"));
    G4HadronicProcess *theCaptureProcess =
        (G4HadronicProcess *)(ptable->FindProcess("LCapture", "neutron"));
    if (theHadronElasticProcess == 0 || theNeutronInelasticProcess == 0 ||
        theHadronFissionProcess == 0 || theCaptureProcess == 0) {
        G4cerr << "Could not find all neutron processes\n";
        return;
    }

    G4DynamicParticle *testNeutron =
        new G4DynamicParticle(G4Neutron::Neutron(), G4ThreeVector(1.0, 0.0, 0.0));
    G4cout << "# neutron cross sections for " << elem->GetName() << " at T=" << temp / kelvin
           << " degK" << G4endl << "# E_kin\tElastic\tInelastic\tFission\tCapture" << G4endl;
    for (int i = 0; i <= 100; i++) {
        double Ek = e1 * pow(e2 / e1, 0.01 * i);
        testNeutron->SetKineticEnergy(Ek);
        G4cout << Ek
               << '\t'
               //<< theHadronElasticProcess	 -> GetMicroscopicCrossSection(testNeutron, elem,
               // temp) << '\t'
               //<< theNeutronInelasticProcess -> GetMicroscopicCrossSection(testNeutron, elem,
               // temp) << '\t'
               //<< theHadronFissionProcess	 -> GetMicroscopicCrossSection(testNeutron, elem,
               // temp) << '\t'
               //<< theCaptureProcess		 -> GetMicroscopicCrossSection(testNeutron, elem,
               // temp) << G4endl;
               << theHadronElasticProcess->GetMicroscopicCrossSection(testNeutron, elem, 0) << '\t'
               << theNeutronInelasticProcess->GetMicroscopicCrossSection(testNeutron, elem, 0)
               << '\t' << theHadronFissionProcess->GetMicroscopicCrossSection(testNeutron, elem, 0)
               << '\t' << theCaptureProcess->GetMicroscopicCrossSection(testNeutron, elem, 0)
               << G4endl;
        /* NOTE: THE LINES ABOVE ARE COMPATIBLE WITH GEANT4 VERSION 3.2 and 4.4
           They are not compatible with version 3.1.  If you are using such an
           old version of Geant4, that's your problem.  Upgrade your Geant4,
           or modifiy your own version of Cupsim if you want to, but don't
           commit the changes. */
    }
}

void CupDebugMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
    // DumpMaterialsCmd
    if (command == DumpMaterialsCmd) {
        if (newValues == "") {
            G4cout << *(G4Material::GetMaterialTable()) << G4endl;
        } else {
            G4Material *m = G4Material::GetMaterial(newValues);
            if (m == NULL) {
                G4cerr << "Unknown material " << newValues << G4endl << std::flush;
                return;
            }
            G4cout << (*m) << G4endl;
            G4MaterialPropertiesTable *mpt = m->GetMaterialPropertiesTable();
            if (mpt == NULL) {
                G4cout << "This material has no material properties table." << G4endl;
            } else {
                G4cout.flush();
                mpt->DumpTable();
                G4cout.flush();
            }
        }
    }

    // DumpGeomCmd
    else if (command == DumpGeomCmd) {
        if (newValues == "") {
            DumpGeom(myDetector->GetWorld(), "myDetector->GetWorld()");
        } else {
            // access the store of physical volumes
            G4PhysicalVolumeStore *pvs = G4PhysicalVolumeStore::GetInstance();
            G4VPhysicalVolume *pv      = NULL;
            G4int npv                  = pvs->size();
            G4int ipv;
            for (ipv = 0; ipv < npv; ipv++) {
                pv = (*pvs)[ipv];
                if (!pv) break;
                if (pv->GetName() == newValues) break;
            }
            if (pv == NULL || ipv >= npv) { // not found
                G4cerr << "Error, name \'" << newValues << "\' not found\n";
                G4cerr.flush();
            } else {
                DumpGeom(pv, newValues);
            }
        }
    }

    else if (command->GetCommandName() == "setmaterial") {
        SetMaterial(newValues);
    }

    else if (command->GetCommandName() == "cupparam") {
        CupParam &db(CupParam::GetDB());
        std::istringstream iss(newValues.c_str());
        G4String parameterName;
        G4double new_value;
        iss >> parameterName;
        if (iss.fail()) {
            G4cerr << "Could not parse parameter name from command args\n";
            G4cerr.flush();
            return;
        }
        iss >> new_value;
        // set new value, if value was provided
        if (!(iss.fail())) db[parameterName] = new_value;
        // print out current value
        switch (db.count(parameterName)) {
            case 0:
                G4cout << parameterName << " undefined" << G4endl;
                break;
            default:
                G4cout << parameterName << " is multiply defined! " << db.count(parameterName)
                       << G4endl;
            case 1:
                G4cout << parameterName << "\t" << db[parameterName] << G4endl;
                break;
        }
    }

    else if (command->GetCommandName() == "cupparam_read") {
        CupParam &db(CupParam::GetDB());
        db.ReadFile(newValues.c_str());
    }

    else if (command->GetCommandName() == "cupparam_dump") {
        CupParam &db(CupParam::GetDB());
        if (newValues.length() > 0) {
            std::ofstream ofstr(newValues.c_str());
            if (!ofstr.good()) {
                G4cerr << "Could not open output file " << newValues << G4endl;
                return;
            }
            db.WriteFile(ofstr);
            ofstr.close();
        } else {
            db.WriteFile(G4cout);
            G4cout.flush();
        }
    } else if (command->GetCommandName() == "dumpelem") {
        if (newValues == "") {
            G4cout << *(G4Element::GetElementTable()) << G4endl;
        } else {
            G4cout << *(G4Element::GetElement(newValues)) << G4endl;
        }
    } else if (command->GetCommandName() == "setseed") {
        std::istringstream iss(newValues.c_str());
        long seed        = 1;
        int luxury_level = 0;
        iss >> seed >> luxury_level;
        // HepRandom::getTheEngine()->setSeed(seed, luxury_level);
        G4Random::getTheEngine()->setSeed(seed, luxury_level);
    } else if (command->GetCommandName() == "dumpNeutronCrossSections") {
        std::istringstream iss(newValues.c_str());
        G4String elemName;
        double e1 = 0.01 * eV, e2 = 10.0 * MeV, temp = 300.0 * kelvin;
        iss >> elemName >> e1 >> e2 >> temp;
        G4Element *elem = G4Element::GetElement(elemName);
        if (elem == 0) {
            G4cerr << "Error, no element named " << elemName << G4endl;
            return;
        }
        DumpNeutronCrossSections(elem, e1, e2, temp);
    } else if (command->GetCommandName() == "SetRunIDCounter") {
        int i = atoi(newValues);
        G4RunManager::GetRunManager()->SetRunIDCounter(i);
        G4cout << "Set RunIDCounter to " << i << endl;
    }

#ifdef G4DEBUG
    else if (command->GetCommandName() == "dump_illumination_map") {
        extern int CupSteppingAction_dump_IlluminationMap(void);
        CupSteppingAction_dump_IlluminationMap();
    }
#endif

    // invalid command
    else {
        G4cerr << "invalid Cup \"set\" command\n" << std::flush;
    }
}

G4String CupDebugMessenger::GetCurrentValue(G4UIcommand * /*command*/) {
    return G4String("invalid CupDebugMessenger \"get\" command");
}
