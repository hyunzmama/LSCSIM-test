
#include "G4EmProcessSubType.hh"
#include "G4ParticleTypes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4Version.hh"
#include "G4ios.hh"
#include "globals.hh"

#include "CupSim/CupScintillation.hh"

/////////////////////////
// Class Implementation
/////////////////////////

//////////////
// Operators
//////////////

// EJ: start
// top level of scintillation command
G4UIdirectory *CupScintillation::CupScintDir = 0;
// universal on/off flag
G4bool CupScintillation::doScintillation = true;
// energy deposition
#if G4VERSION_NUMBER >= 1000
G4ThreadLocal G4double CupScintillation::totEdep          = 0.0;
G4ThreadLocal G4double CupScintillation::totEdep_quenched = 0.0;
G4ThreadLocal G4int CupScintillation::nScintPhotons       = 0;
#else
G4double CupScintillation::totEdep          = 0.0;
G4double CupScintillation::totEdep_quenched = 0.0;
G4int CupScintillation::nScintPhotons       = 0;
#endif

// EJ: end

/////////////////
// Constructors
/////////////////

CupScintillation::CupScintillation(const G4String &processName, G4ProcessType type)
    : G4VRestDiscreteProcess(processName, type) {
    SetProcessSubType(fScintillation);

    fTrackSecondariesFirst = false;
    fFiniteRiseTime        = false;

    YieldFactor     = 1.0;
    ExcitationRatio = 1.0;

    scintillationByParticleType = false;

    theFastIntegralTable = NULL;
    theSlowIntegralTable = NULL;

    if (verboseLevel > 0) {
        G4cout << GetProcessName() << " is created " << G4endl;
    }

    BuildThePhysicsTable();

    emSaturation = NULL;

    // EJ: start
    // create UI commands if necessary
    if (CupScintDir == NULL) {
        // the scintillation control commands
        CupScintDir = new G4UIdirectory("/cupscint/");
        CupScintDir->SetGuidance("scintillation process control.");
        G4UIcommand *cmd;
        cmd = new G4UIcommand("/cupscint/on", this);
        cmd->SetGuidance("Turn on scintillation");
        cmd = new G4UIcommand("/cupscint/off", this);
        cmd->SetGuidance("Turn off scintillation");
        cmd = new G4UIcommand("/cupscint/verbose", this);
        cmd->SetGuidance("Set verbose level");
        cmd->SetParameter(new G4UIparameter("level", 'i', false));
    }
    // EJ: end
}

////////////////
// Destructors
////////////////

CupScintillation::~CupScintillation() {
    if (theFastIntegralTable != NULL) {
        theFastIntegralTable->clearAndDestroy();
        delete theFastIntegralTable;
    }
    if (theSlowIntegralTable != NULL) {
        theSlowIntegralTable->clearAndDestroy();
        delete theSlowIntegralTable;
    }
}

////////////
// Methods
////////////

// EJ: start
void CupScintillation::SetNewValue(G4UIcommand *command, G4String newValues) {
    G4String commandName = command->GetCommandName();
    if (commandName == "on") {
        doScintillation = true;
    } else if (commandName == "off") {
        doScintillation = false;
    } else if (commandName == "verbose") {
        verboseLevel = strtol((const char *)newValues, NULL, 0);
    } else {
        G4cerr << "No CupScintillation command named " << commandName << G4endl;
    }
    return;
}
// EJ: end

// AtRestDoIt
// ----------
//
G4VParticleChange *CupScintillation::AtRestDoIt(const G4Track &aTrack, const G4Step &aStep)

// This routine simply calls the equivalent PostStepDoIt since all the
// necessary information resides in aStep.GetTotalEnergyDeposit()

{
    return CupScintillation::PostStepDoIt(aTrack, aStep);
}

// PostStepDoIt
// -------------
//
G4VParticleChange *CupScintillation::PostStepDoIt(const G4Track &aTrack, const G4Step &aStep)

// This routine is called for each tracking step of a charged particle
// in a scintillator. A Poisson/Gauss-distributed number of photons is
// generated according to the scintillation yield formula, distributed
// evenly along the track segment and uniformly into 4pi.

{
    aParticleChange.Initialize(aTrack);

    const G4DynamicParticle *aParticle = aTrack.GetDynamicParticle();
    const G4Material *aMaterial        = aTrack.GetMaterial();

    G4StepPoint *pPreStepPoint  = aStep.GetPreStepPoint();
    G4StepPoint *pPostStepPoint = aStep.GetPostStepPoint();

    G4ThreeVector x0 = pPreStepPoint->GetPosition();
    G4ThreeVector p0 = aStep.GetDeltaPosition().unit();
    G4double t0      = pPreStepPoint->GetGlobalTime();

    G4double TotalEnergyDeposit = aStep.GetTotalEnergyDeposit();

    G4MaterialPropertiesTable *aMaterialPropertiesTable = aMaterial->GetMaterialPropertiesTable();
    if (!aMaterialPropertiesTable) return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);

    G4MaterialPropertyVector *Fast_Intensity =
        aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");
    G4MaterialPropertyVector *Slow_Intensity =
        aMaterialPropertiesTable->GetProperty("SLOWCOMPONENT");

    if (!Fast_Intensity && !Slow_Intensity)
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);

    G4int nscnt = 1;
    if (Fast_Intensity && Slow_Intensity) nscnt = 2;

    G4double ScintillationYield = 0.;

    if (scintillationByParticleType) {
        // The scintillation response is a function of the energy
        // deposited by particle types.

        // Get the definition of the current particle
        G4ParticleDefinition *pDef                   = aParticle->GetDefinition();
        G4MaterialPropertyVector *Scint_Yield_Vector = NULL;

        // Obtain the G4MaterialPropertyVectory containing the
        // scintillation light yield as a function of the deposited
        // energy for the current particle type

        // Protons
        if (pDef == G4Proton::ProtonDefinition())
            Scint_Yield_Vector = aMaterialPropertiesTable->GetProperty("PROTONSCINTILLATIONYIELD");

        // Deuterons
        else if (pDef == G4Deuteron::DeuteronDefinition())
            Scint_Yield_Vector =
                aMaterialPropertiesTable->GetProperty("DEUTERONSCINTILLATIONYIELD");

        // Tritons
        else if (pDef == G4Triton::TritonDefinition())
            Scint_Yield_Vector = aMaterialPropertiesTable->GetProperty("TRITONSCINTILLATIONYIELD");

        // Alphas
        else if (pDef == G4Alpha::AlphaDefinition())
            Scint_Yield_Vector = aMaterialPropertiesTable->GetProperty("ALPHASCINTILLATIONYIELD");

        // Ions (particles derived from G4VIon and G4Ions)
        // and recoil ions below tracking cut from neutrons after hElastic
        else if (pDef->GetParticleType() == "nucleus" || pDef == G4Neutron::NeutronDefinition())
            Scint_Yield_Vector = aMaterialPropertiesTable->GetProperty("IONSCINTILLATIONYIELD");

        // Electrons (must also account for shell-binding energy
        // attributed to gamma from standard PhotoElectricEffect)
        else if (pDef == G4Electron::ElectronDefinition() || pDef == G4Gamma::GammaDefinition())
            Scint_Yield_Vector =
                aMaterialPropertiesTable->GetProperty("ELECTRONSCINTILLATIONYIELD");

        // Default for particles not enumerated/listed above
        else
            Scint_Yield_Vector =
                aMaterialPropertiesTable->GetProperty("ELECTRONSCINTILLATIONYIELD");

        // If the user has not specified yields for (p,d,t,a,carbon)
        // then these unspecified particles will default to the
        // electron's scintillation yield
        if (!Scint_Yield_Vector) {
            Scint_Yield_Vector =
                aMaterialPropertiesTable->GetProperty("ELECTRONSCINTILLATIONYIELD");
        }

        // Throw an exception if no scintillation yield is found
        if (!Scint_Yield_Vector) {
            G4ExceptionDescription ed;
            ed << "\nCupScintillation::PostStepDoIt(): "
               << "Request for scintillation yield for energy deposit and particle type without "
                  "correct entry in MaterialPropertiesTable\n"
               << "ScintillationByParticleType requires at minimum that ELECTRONSCINTILLATIONYIELD "
                  "is set by the user\n"
               << G4endl;
            G4String comments = "Missing MaterialPropertiesTable entry - No correct entry in "
                                "MaterialPropertiesTable";
            G4Exception("CupSim/CupScintillation::PostStepDoIt", "Scint01", FatalException, ed,
                        comments);
            return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }

        if (verboseLevel > 1) {
            G4cout << "\n"
                   << "Particle = " << pDef->GetParticleName() << "\n"
                   << "Energy Dep. = " << TotalEnergyDeposit / MeV << "\n"
                   << "Yield = " << Scint_Yield_Vector->Value(TotalEnergyDeposit) << "\n"
                   << G4endl;
        }

        // Obtain the scintillation yield using the total energy
        // deposited by the particle in this step.

        // Units: [# scintillation photons]
        ScintillationYield = Scint_Yield_Vector->Value(TotalEnergyDeposit);
    } else {
        // The default linear scintillation process
        ScintillationYield = aMaterialPropertiesTable->GetConstProperty("SCINTILLATIONYIELD");

        // Units: [# scintillation photons / MeV]
        ScintillationYield *= YieldFactor;

        // EJ: start
        G4ParticleDefinition *pDef = aParticle->GetDefinition();
        if (verboseLevel > 1) {
            G4cout << "\n"
                   << "Particle = " << pDef->GetParticleName() << "\n"
                   << "Energy Dep. = " << TotalEnergyDeposit / MeV << "\n"
                   << "Yield = " << ScintillationYield << "\n"
                   << G4endl;
        }
        // EJ: end
    }

    G4double ResolutionScale = aMaterialPropertiesTable->GetConstProperty("RESOLUTIONSCALE");

    // Birks law saturation:

    G4double MeanNumberOfPhotons;

    G4double TotalEnergyDepositQuenched = 0.;

    if (scintillationByParticleType)
        MeanNumberOfPhotons = ScintillationYield;
    else if (emSaturation) {
#if G4VERSION_NUMBER <= 1020
        TotalEnergyDepositQuenched = emSaturation->VisibleEnergyDeposition(&aStep);
#else
        TotalEnergyDepositQuenched = emSaturation->VisibleEnergyDepositionAtAStep(&aStep);
#endif
        MeanNumberOfPhotons = ScintillationYield * TotalEnergyDepositQuenched;
    } else {
        MeanNumberOfPhotons = ScintillationYield * TotalEnergyDeposit;
    }

    G4int NumPhotons;

    if (MeanNumberOfPhotons > 10.) {
        G4double sigma = ResolutionScale * std::sqrt(MeanNumberOfPhotons);
        NumPhotons     = G4int(G4RandGauss::shoot(MeanNumberOfPhotons, sigma) + 0.5);
    } else {
        NumPhotons = G4int(G4Poisson(MeanNumberOfPhotons));
    }

    // EJ: start
    totEdep += TotalEnergyDeposit;
    totEdep_quenched += TotalEnergyDepositQuenched;
    nScintPhotons += NumPhotons;

    if (verboseLevel > 2) {
        G4cout << "EJ: CupScintillation: edep= " << TotalEnergyDeposit
               << ", edep_quenched= " << TotalEnergyDepositQuenched << G4endl;
        G4cout << "EJ: totEdep= " << totEdep << ", totEdep_quenched= " << totEdep_quenched
               << ", NumPhotons= " << nScintPhotons
               << ", scintillationByParticleType= " << scintillationByParticleType << G4endl;
        G4cout << "EJ: resolutionscale= " << ResolutionScale << G4endl;
    }
    // EJ: end

    if (NumPhotons <= 0) {
        // return unchanged particle and no secondaries

        aParticleChange.SetNumberOfSecondaries(0);

        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    // EJ: start
    // now we are done if we are not actually making photons here
    if (!doScintillation) {
        aParticleChange.SetNumberOfSecondaries(0);
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
    // EJ: end

    ////////////////////////////////////////////////////////////////

    aParticleChange.SetNumberOfSecondaries(NumPhotons);

    if (fTrackSecondariesFirst) {
        if (aTrack.GetTrackStatus() == fAlive) aParticleChange.ProposeTrackStatus(fSuspend);
    }

    ////////////////////////////////////////////////////////////////

    G4int materialIndex = aMaterial->GetIndex();

    // Retrieve the Scintillation Integral for this material
    // new G4PhysicsOrderedFreeVector allocated to hold CII's

    G4int Num = NumPhotons;

    for (G4int scnt = 1; scnt <= nscnt; scnt++) {

        G4double ScintillationTime                        = 0. * ns;
        G4double ScintillationRiseTime                    = 0. * ns;
        G4PhysicsOrderedFreeVector *ScintillationIntegral = NULL;

        if (scnt == 1) {
            if (nscnt == 1) {
                if (Fast_Intensity) {
                    ScintillationTime =
                        aMaterialPropertiesTable->GetConstProperty("FASTTIMECONSTANT");
                    if (fFiniteRiseTime) {
                        ScintillationRiseTime =
                            aMaterialPropertiesTable->GetConstProperty("FASTSCINTILLATIONRISETIME");
                    }
                    ScintillationIntegral =
                        (G4PhysicsOrderedFreeVector *)((*theFastIntegralTable)(materialIndex));
                }
                if (Slow_Intensity) {
                    ScintillationTime =
                        aMaterialPropertiesTable->GetConstProperty("SLOWTIMECONSTANT");
                    if (fFiniteRiseTime) {
                        ScintillationRiseTime =
                            aMaterialPropertiesTable->GetConstProperty("SLOWSCINTILLATIONRISETIME");
                    }
                    ScintillationIntegral =
                        (G4PhysicsOrderedFreeVector *)((*theSlowIntegralTable)(materialIndex));
                }
            } else {
                G4double YieldRatio = aMaterialPropertiesTable->GetConstProperty("YIELDRATIO");
                if (ExcitationRatio == 1.0) {
                    Num = G4int(std::min(YieldRatio, 1.0) * NumPhotons);
                } else {
                    Num = G4int(std::min(ExcitationRatio, 1.0) * NumPhotons);
                }
                ScintillationTime = aMaterialPropertiesTable->GetConstProperty("FASTTIMECONSTANT");
                if (fFiniteRiseTime) {
                    ScintillationRiseTime =
                        aMaterialPropertiesTable->GetConstProperty("FASTSCINTILLATIONRISETIME");
                }
                ScintillationIntegral =
                    (G4PhysicsOrderedFreeVector *)((*theFastIntegralTable)(materialIndex));
            }
        } else {
            Num               = NumPhotons - Num;
            ScintillationTime = aMaterialPropertiesTable->GetConstProperty("SLOWTIMECONSTANT");
            if (fFiniteRiseTime) {
                ScintillationRiseTime =
                    aMaterialPropertiesTable->GetConstProperty("SLOWSCINTILLATIONRISETIME");
            }
            ScintillationIntegral =
                (G4PhysicsOrderedFreeVector *)((*theSlowIntegralTable)(materialIndex));
        }

        if (!ScintillationIntegral) continue;

        // Max Scintillation Integral

        G4double CIImax = ScintillationIntegral->GetMaxValue();

        for (G4int i = 0; i < Num; i++) {

            // Determine photon energy

            G4double CIIvalue      = G4UniformRand() * CIImax;
            G4double sampledEnergy = ScintillationIntegral->GetEnergy(CIIvalue);

            if (verboseLevel > 1) {
                G4cout << "sampledEnergy = " << sampledEnergy << G4endl;
                G4cout << "CIIvalue =        " << CIIvalue << G4endl;
            }

            // Generate random photon direction

            G4double cost = 1. - 2. * G4UniformRand();
            G4double sint = std::sqrt((1. - cost) * (1. + cost));

            G4double phi  = twopi * G4UniformRand();
            G4double sinp = std::sin(phi);
            G4double cosp = std::cos(phi);

            G4double px = sint * cosp;
            G4double py = sint * sinp;
            G4double pz = cost;

            // Create photon momentum direction vector

            G4ParticleMomentum photonMomentum(px, py, pz);

            // Determine polarization of new photon

            G4double sx = cost * cosp;
            G4double sy = cost * sinp;
            G4double sz = -sint;

            G4ThreeVector photonPolarization(sx, sy, sz);

            G4ThreeVector perp = photonMomentum.cross(photonPolarization);

            phi  = twopi * G4UniformRand();
            sinp = std::sin(phi);
            cosp = std::cos(phi);

            photonPolarization = cosp * photonPolarization + sinp * perp;

            photonPolarization = photonPolarization.unit();

            // Generate a new photon:

            G4DynamicParticle *aScintillationPhoton =
                new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(), photonMomentum);
            aScintillationPhoton->SetPolarization(photonPolarization.x(), photonPolarization.y(),
                                                  photonPolarization.z());

            aScintillationPhoton->SetKineticEnergy(sampledEnergy);

            // Generate new G4Track object:

            G4double rand;

            if (aParticle->GetDefinition()->GetPDGCharge() != 0) {
                rand = G4UniformRand();
            } else {
                rand = 1.0;
            }

            G4double delta = rand * aStep.GetStepLength();
            G4double deltaTime =
                delta / ((pPreStepPoint->GetVelocity() + pPostStepPoint->GetVelocity()) / 2.);

            // emission time distribution
            if (ScintillationRiseTime == 0.0) {
                deltaTime = deltaTime - ScintillationTime * std::log(G4UniformRand());
            } else {
                deltaTime = deltaTime + sample_time(ScintillationRiseTime, ScintillationTime);
            }

            G4double aSecondaryTime = t0 + deltaTime;

            G4ThreeVector aSecondaryPosition = x0 + rand * aStep.GetDeltaPosition();

            G4Track *aSecondaryTrack =
                new G4Track(aScintillationPhoton, aSecondaryTime, aSecondaryPosition);

            aSecondaryTrack->SetTouchableHandle(aStep.GetPreStepPoint()->GetTouchableHandle());
            // aSecondaryTrack->SetTouchableHandle((G4VTouchable*)0);

            aSecondaryTrack->SetParentID(aTrack.GetTrackID());

            aParticleChange.AddSecondary(aSecondaryTrack);
        }
    }

    if (verboseLevel > 0) {
        G4cout << "\n Exiting from CupScintillation::DoIt -- NumberOfSecondaries = "
               << aParticleChange.GetNumberOfSecondaries() << G4endl;
    }

    return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// BuildThePhysicsTable for the scintillation process
// --------------------------------------------------
//

void CupScintillation::BuildThePhysicsTable() {
    if (theFastIntegralTable && theSlowIntegralTable) return;

    const G4MaterialTable *theMaterialTable = G4Material::GetMaterialTable();
    G4int numOfMaterials                    = G4Material::GetNumberOfMaterials();

    // create new physics table

    if (!theFastIntegralTable) theFastIntegralTable = new G4PhysicsTable(numOfMaterials);
    if (!theSlowIntegralTable) theSlowIntegralTable = new G4PhysicsTable(numOfMaterials);

    // loop for materials

    for (G4int i = 0; i < numOfMaterials; i++) {
        G4PhysicsOrderedFreeVector *aPhysicsOrderedFreeVector = new G4PhysicsOrderedFreeVector();
        G4PhysicsOrderedFreeVector *bPhysicsOrderedFreeVector = new G4PhysicsOrderedFreeVector();

        // Retrieve vector of scintillation wavelength intensity for
        // the material from the material's optical properties table.

        G4Material *aMaterial = (*theMaterialTable)[i];

        G4MaterialPropertiesTable *aMaterialPropertiesTable =
            aMaterial->GetMaterialPropertiesTable();

        if (aMaterialPropertiesTable) {

            G4MaterialPropertyVector *theFastLightVector =
                aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");

            if (theFastLightVector) {

                // Retrieve the first intensity point in vector
                // of (photon energy, intensity) pairs

                G4double currentIN = (*theFastLightVector)[0];

                if (currentIN >= 0.0) {

                    // Create first (photon energy, Scintillation
                    // Integral pair

                    G4double currentPM = theFastLightVector->Energy(0);

                    G4double currentCII = 0.0;

                    aPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

                    // Set previous values to current ones prior to loop

                    G4double prevPM  = currentPM;
                    G4double prevCII = currentCII;
                    G4double prevIN  = currentIN;

                    // loop over all (photon energy, intensity)
                    // pairs stored for this material

                    for (size_t ii = 1; ii < theFastLightVector->GetVectorLength(); ++ii) {
                        currentPM = theFastLightVector->Energy(ii);
                        currentIN = (*theFastLightVector)[ii];

                        currentCII = 0.5 * (prevIN + currentIN);

                        currentCII = prevCII + (currentPM - prevPM) * currentCII;

                        aPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

                        prevPM  = currentPM;
                        prevCII = currentCII;
                        prevIN  = currentIN;
                    }
                }
            }

            G4MaterialPropertyVector *theSlowLightVector =
                aMaterialPropertiesTable->GetProperty("SLOWCOMPONENT");

            if (theSlowLightVector) {

                // Retrieve the first intensity point in vector
                // of (photon energy, intensity) pairs

                G4double currentIN = (*theSlowLightVector)[0];

                if (currentIN >= 0.0) {

                    // Create first (photon energy, Scintillation
                    // Integral pair

                    G4double currentPM = theSlowLightVector->Energy(0);

                    G4double currentCII = 0.0;

                    bPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

                    // Set previous values to current ones prior to loop

                    G4double prevPM  = currentPM;
                    G4double prevCII = currentCII;
                    G4double prevIN  = currentIN;

                    // loop over all (photon energy, intensity)
                    // pairs stored for this material

                    for (size_t ii = 1; ii < theSlowLightVector->GetVectorLength(); ++ii) {
                        currentPM = theSlowLightVector->Energy(ii);
                        currentIN = (*theSlowLightVector)[ii];

                        currentCII = 0.5 * (prevIN + currentIN);

                        currentCII = prevCII + (currentPM - prevPM) * currentCII;

                        bPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

                        prevPM  = currentPM;
                        prevCII = currentCII;
                        prevIN  = currentIN;
                    }
                }
            }
        }

        // The scintillation integral(s) for a given material
        // will be inserted in the table(s) according to the
        // position of the material in the material table.

        theFastIntegralTable->insertAt(i, aPhysicsOrderedFreeVector);
        theSlowIntegralTable->insertAt(i, bPhysicsOrderedFreeVector);
    }
}

// Called by the user to set the scintillation yield as a function
// of energy deposited by particle type

void CupScintillation::SetScintillationByParticleType(const G4bool scintType) {
    if (emSaturation) {
        G4Exception("CupSim/CupScintillation::SetScintillationByParticleType", "Scint02",
                    JustWarning,
                    "Redefinition: Birks Saturation is replaced by ScintillationByParticleType!");
        RemoveSaturation();
    }
    scintillationByParticleType = scintType;
}

// GetMeanFreePath
// ---------------
//

G4double CupScintillation::GetMeanFreePath(const G4Track &, G4double, G4ForceCondition *condition) {
    *condition = StronglyForced;

    return DBL_MAX;
}

// GetMeanLifeTime
// ---------------
//

G4double CupScintillation::GetMeanLifeTime(const G4Track &, G4ForceCondition *condition) {
    *condition = Forced;

    return DBL_MAX;
}

G4double CupScintillation::sample_time(G4double tau1, G4double tau2) {
    // tau1: rise time and tau2: decay time

    while (1) {
        // two random numbers
        G4double ran1 = G4UniformRand();
        G4double ran2 = G4UniformRand();
        //
        // exponential distribution as envelope function: very efficient
        //
        G4double d = (tau1 + tau2) / tau2;
        // make sure the envelope function is
        // always larger than the bi-exponential
        G4double t  = -1.0 * tau2 * std::log(1 - ran1);
        G4double gg = d * single_exp(t, tau2);
        if (ran2 <= bi_exp(t, tau1, tau2) / gg) return t;
    }
    return -1.0;
}
