
#include "G4Version.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "TROOT.h"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"

#include "CupSim/CupDebugMessenger.hh"
#include "CupSim/CupDetectorConstruction.hh"
#include "CupSim/CupParam.hh"
#include "CupSim/CupPhysicsList.hh" // EJ
#include "CupSim/CupPrimaryGeneratorAction.hh"
#include "CupSim/CupRecorderBase.hh" // EJ
#include "CupSim/CupRootNtuple.hh"   // EJ
#include "CupSim/CupRunAction.hh"    // EJ
#include "CupSim/CupSimGitRevision.hh"
#include "CupSim/CupSteppingAction.hh" // EJ
#include "CupSim/CupTrackingAction.hh" // EJ
#include "CupSim/CupVEventAction.hh"   // EJ

#include "MCObjs/MCObjsGitRevision.hh"

#ifdef G4VIS_USE
#include "CupSim/CupVisMessenger.hh"
#include "G4VisExecutive.hh"
#endif
#include "G4Run.hh"
#include <cstdlib>

#if G4VERSION_NUMBER >= 1000
#include "CupSim/CupActionInitialization.hh"
#endif

#define STRINGFY(X) #X
#define TOSTRING(X) STRINGFY(X)

void PrintGitInfo() {
}

int main(int argc, char **argv) {
    ROOT::EnableThreadSafety();
    std::cout << "Version informaion for cupsim: " << std::endl;
    std::cout << "CupSim executable: "
              << " <Branch: " << TOSTRING(CupSim_GIT_BRANCH)
              << ", Revision hash: " << TOSTRING(CupSim_GIT_COMMIT_HASH) << ">" << std::endl;
    PrintGitInfo();
    CupSimGitRevision::PrintGitInfo();
    MCObjsGitRevision::PrintGitInfo();
    std::cout << std::endl;

    // Run manager
#ifdef G4MULTITHREADED
    G4MTRunManager *theRunManager = new G4MTRunManager;
    theRunManager->SetNumberOfThreads(4);
#else
    G4RunManager *theRunManager = new G4RunManager;
#endif

    // -- database
    CupParam &db(CupParam::GetDB());
    if (getenv("CupDATA") != NULL)
        db.ReadFile((G4String(getenv("CupDATA")) + "/settings.dat").c_str());
    else
        db.ReadFile("data/settings.dat");

    // UserInitialization classes
    CupDetectorConstruction *theCupDetectorConstruction = new CupDetectorConstruction;
    theRunManager->SetUserInitialization(theCupDetectorConstruction);
    theRunManager->SetUserInitialization(new CupPhysicsList);

    // Create the CupRecorderBase object
    CupRecorderBase *myRecords = new CupRootNtuple; // EJ

#if G4VERSION_NUMBER >= 1000
    theRunManager->SetUserInitialization(
        new CupActionInitialization(myRecords, theCupDetectorConstruction));
#else
    // UserAction classes
    theRunManager->SetUserAction(new CupPrimaryGeneratorAction(theCupDetectorConstruction));
    theRunManager->SetUserAction(new CupRunAction(myRecords));
    CupVEventAction *eventAction = new CupVEventAction(myRecords);
    theRunManager->SetUserAction(eventAction);
    theRunManager->SetUserAction(new CupTrackingAction(myRecords));
    theRunManager->SetUserAction(new CupSteppingAction(myRecords));
#endif

    // an additional "messenger" class for user diagnostics
    CupDebugMessenger theDebugMessenger(theCupDetectorConstruction);

    // Visualization, only if you choose to have it!
#ifdef G4VIS_USE
    G4VisManager *theVisManager      = new G4VisExecutive();
    CupVisMessenger *theVisMessenger = new CupVisMessenger(theVisManager);
    theVisManager->Initialize();
#endif

    // user interface
    G4UImanager *theUI = G4UImanager::GetUIpointer();

    // interactive or batch according to command-line args
    if (argc == 1) {
        // G4UIterminal is a (dumb) terminal.
        // ..but it can be made smart by adding a "shell" to it
        G4UIsession *theSession = new G4UIterminal(new G4UItcsh);
        theUI->ApplyCommand("/control/execute ./mac/IBD.mac");
        // theUI -> ApplyCommand("/control/execute ./mac/prerun_co60.mac");
        theSession->SessionStart();
        delete theSession;
    } else // Batch mode, with optional user interaction
    {
        G4String command = "/control/execute ";
        for (int iarg = 1; iarg < argc; iarg++)
        // process list of macro files; "-" means interactive user session
        {
            G4String fileName = argv[iarg];
            if (fileName == "-")
            // interactive session requested
            {
                G4UIsession *theSession = new G4UIterminal(new G4UItcsh);
                theSession->SessionStart();
                delete theSession;
            } else
            // execute given file
            {
                theUI->ApplyCommand(command + fileName);
            }
        }
    }

#ifdef G4VIS_USE
    delete theVisManager;
    delete theVisMessenger;
#endif

    delete theRunManager;
    delete myRecords; // EJ

    return 0;
}
