//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: CupPhysicsListMessenger.hh,v 1.1.1.1 2016/10/31 08:41:44 ejjeon Exp $
// GEANT4 tag $Name:  $
//
//
/////////////////////////////////////////////////////////////////////////
//
// CupPhysicsListMessenger
//
// Created: 31.01.2006 V.Ivanchenko
//
// Modified:
// 04.06.2006 Adoptation of hadr01 (V.Ivanchenko)
//
////////////////////////////////////////////////////////////////////////
//

#ifndef CupPhysicsListMessenger_h
#define CupPhysicsListMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class CupPhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcommand;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CupPhysicsListMessenger : public G4UImessenger {
  public:
    CupPhysicsListMessenger(CupPhysicsList *);
    virtual ~CupPhysicsListMessenger();

    void SetNewValue(G4UIcommand *, G4String);

  private:
    CupPhysicsList *pPhysicsList;

    G4UIcmdWithADoubleAndUnit *gammaCutCmd;
    G4UIcmdWithADoubleAndUnit *electCutCmd;
    G4UIcmdWithADoubleAndUnit *posCutCmd;
    G4UIcmdWithADoubleAndUnit *allCutCmd;
    G4UIcmdWithADoubleAndUnit *mCutCmd;
    G4UIcmdWithAString *pListCmd;
    G4UIcmdWithoutParameter *listCmd;
    G4UIcommand *CrystalRegionCmd;

    G4UIdirectory *physDir;
    G4UIcmdWithAnInteger *verboseCmd;
    G4UIcmdWithADouble *yieldfactorCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
