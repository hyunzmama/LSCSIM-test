
#ifdef G4VIS_USE

#include "CupSim/CupVisMessenger.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4ViewParameters.hh"
#include "G4ios.hh"
#include "globals.hh"

#include <sstream>

CupVisMessenger::CupVisMessenger(G4VisManager *pVMan_) : pVMan(pVMan_) {
    // the cupvis directory
    Dir = new G4UIdirectory("/cupvis/");
    Dir->SetGuidance("Visualization commands more suited to GenericLAND.");

    // the cupvis reset command (same as camera/reset)
    CameraResetCmd = new G4UIcommand("/cupvis/reset", this);
    CameraResetCmd->SetGuidance("Reset to nominal viewing position, up vector, etc.");

    // the upvector command
    UpVectorCmd = new G4UIcommand("/cupvis/upvector", this);
    UpVectorCmd->SetGuidance("Set \"up\" direction");
    UpVectorCmd->SetParameter(new G4UIparameter("x", 'd', true));
    UpVectorCmd->SetParameter(new G4UIparameter("y", 'd', true));
    UpVectorCmd->SetParameter(new G4UIparameter("z", 'd', true));
}

CupVisMessenger::~CupVisMessenger() {
    delete Dir;
    delete CameraResetCmd;
    delete UpVectorCmd;
}

void CupVisMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {

    G4String commandname = command->GetCommandName();
    std::istringstream is(newValues);

    G4VViewer *currentViewer = pVMan->GetCurrentViewer();
    if (!currentViewer) {
        G4cerr << "CupSim/CupVisMessenger::SetNewValue: no current viewer." << G4endl;
        return;
    }
    G4ViewParameters vp = currentViewer->GetViewParameters();

    if (commandname == "reset") {
        vp.SetCurrentTargetPoint(G4Point3D(0.0, 0.0, 0.0));
        vp.SetZoomFactor(1.);
        vp.SetDolly(0.);
        vp.SetViewpointDirection(G4Vector3D(0., 1., 0.));
        vp.SetUpVector(G4Vector3D(0., 0., 1.));
        G4cout << "Target point reset to (0.0,0.0,0.0)\n";
        G4cout << "Zoom factor reset to 1.\n";
        G4cout << "Dolly distance reset to 0.\n";
        G4cout << "Viewpoint direction reset to +y.\n";
        G4cout << "Up vector set to +z.";
        G4cout << G4endl;
    } else if (commandname == "upvector") {
        G4double x, y, z;
        is >> x >> y >> z;
        if (is.fail()) {
            G4cerr << "CupSim/CupVisMessenger::SetNewValue: "
                   << "Could not understand arguments, up vector left as " << vp.GetUpVector()
                   << G4endl;
            return;
        } else {
            vp.SetUpVector(G4Vector3D(x, y, z));
        }
    } else {
        G4cerr << "CupSim/CupVisMessenger::SetNewValue: I do not recognize this command: "
               << commandname << G4endl;
        return;
    }

    currentViewer->SetViewParameters(vp);
}

G4String CupVisMessenger::GetCurrentValue(G4UIcommand *) {
    return G4String("invalid CupVisMessenger \"get\" command");
}
#endif
