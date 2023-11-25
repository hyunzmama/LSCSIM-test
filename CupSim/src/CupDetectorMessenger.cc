
#include "CupSim/CupDetectorMessenger.hh"
#include "CupSim/CupDetectorConstruction.hh"

#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "globals.hh"

#include "fstream"  // for file streams
#include <stdlib.h> // for strtol

CupDetectorMessenger::CupDetectorMessenger(CupDetectorConstruction *mydetector)
    : myDetector(mydetector) {
    // the CupDetector directory
    DetectorDir = new G4UIdirectory("/detector/");
    DetectorDir->SetGuidance("Control the detector geometry options.");

    // the select command
    DetectorSelectCmd = new G4UIcommand("/detector/select", this);
    DetectorSelectCmd->SetGuidance("Select which detector you want to build");
    DetectorSelectCmd->SetGuidance(
        "Use with no parameters to get list of available detector styles.");
    DetectorSelectCmd->AvailableForStates(G4State_PreInit);
    DetectorSelectCmd->SetParameter(new G4UIparameter("which", 's', true));

    // the caldevice command
    CalDeviceCmd = new G4UIcommand("/detector/calDevice", this);
    CalDeviceCmd->SetGuidance("Select which calibration device you want to build");
    CalDeviceCmd->AvailableForStates(G4State_PreInit);
    CalDeviceCmd->SetParameter(new G4UIparameter("which", 's', true));

    // the calposition command
    CalPositionCmd = new G4UIcmdWith3VectorAndUnit("/detector/calPosition", this);
    CalPositionCmd->SetGuidance("Set the source position");
    CalPositionCmd->AvailableForStates(G4State_PreInit);
    CalPositionCmd->SetParameterName("x", "y", "z", false);
    CalPositionCmd->SetDefaultUnit("mm");
}

CupDetectorMessenger::~CupDetectorMessenger() {
    delete PmtStyleCmd;
    delete DetectorSelectCmd;
    delete CalDeviceCmd;
    delete CalPositionCmd;
    delete DetectorDir;
}

void CupDetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
    // DetectorSelectCmd
    if (command == DetectorSelectCmd) {
        if (newValues.length() == 0) {
            G4cout << "Available detector geometries: ";
            for (int i = 0; i < myDetector->GetNumDetectorTypes(); i++)
                G4cout << " " << myDetector->GetDetectorTypeName(i);
            G4cout << G4endl;
        } else {
            for (int i = 0; i < myDetector->GetNumDetectorTypes(); i++) {
                if (newValues.compare(myDetector->GetDetectorTypeName(i)) == 0) {
                    // EJ: start
                    G4cout << "detector/select " << myDetector->GetDetectorTypeName(i) << G4endl;
                    // EJ: end
                    myDetector->SetWhichDetector(i);
                    return;
                }
            }
            G4cerr << "Unknown detector style " << newValues << G4endl;
        }
    }

    // CalDeviceCmd
    else if (command == CalDeviceCmd) {
        myDetector->SetWhichCalibrationDevice(newValues);
    }

    // CalPositionCmd
    else if (command == CalPositionCmd) {
        myDetector->SetCalibrationPosition(CalPositionCmd->GetNew3VectorValue(newValues));
    }

    // invalid command
    else {
        G4cerr << "invalid detector \"set\" command\n" << std::flush;
    }
}

G4String CupDetectorMessenger::GetCurrentValue(G4UIcommand *command) {
    // DetectorSelectCmd
    if (command == DetectorSelectCmd) {
        return myDetector->GetDetectorTypeName(myDetector->GetWhichDetector());
    }

    // CalDeviceCmd
    else if (command == CalDeviceCmd) {
        return myDetector->GetWhichCalibrationDevice();
    }

    // CalPositionCmd
    else if (command == CalPositionCmd) {
        return CalPositionCmd->ConvertToString(myDetector->GetCalibrationPosition(), "mm");
    }

    // invalid command
    else {
        return G4String("invalid CupDetectorMessenger \"get\" command");
    }
}
