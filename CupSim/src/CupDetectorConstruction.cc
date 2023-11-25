
#include "fstream"

#include "globals.hh"

#include "CupSim/CupDetectorConstruction.hh"
#include "CupSim/CupDetectorMessenger.hh"
#include "CupSim/CupInputDataReader.hh"

#include "G4Element.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4PhysicalVolumeStore.hh"
#include "Randomize.hh" // for G4UniformRand()

#include "G4NistManager.hh"

using namespace CLHEP;
int CupDetectorConstruction::whichDetector = 99999; // EJ

CupDetectorConstruction::CupDetectorConstruction() {
    world_phys      = NULL;
    materials_built = false;
    whichDetector= kDetector_TestBench;
    whichPmtStyle = kPmtStyle_TorusStack;
    myMessenger   = new CupDetectorMessenger(this);
}

CupDetectorConstruction::~CupDetectorConstruction() {}

G4VPhysicalVolume *CupDetectorConstruction::Construct() {
    // make materials if needed
    if (!materials_built) {
        ConstructMaterials();
    }

    // delete the old detector if we are constructing a new one
    if (world_phys) {
        delete world_phys;
        world_phys = NULL;
    }

    // construct the new detector
    switch (whichDetector) {
        case kDetector_TestBench:
            ConstructTestBench();
            break;
        default:
            G4cerr << "ERROR: INVALID VALUE for CupDetectorConstruction.whichDetector" << G4endl
                   << std::flush;
            // fall through and make a GenericLAND
    }

    return world_phys;
}
// end of CupDetectorConstruction::Construct()

// ----------------------------------------------------------------
G4String CupDetectorConstruction::GetDetectorTypeName(int i) {
    switch (i) {
        case kDetector_TestBench:
            return "testbench";
        default:
            return "detector-unknown";
    }
}

// ----------------------------------------------------------------
void CupDetectorConstruction::ConstructMaterials() {
    // === Common Elements ================================================
    G4double a; // atomic mass
    G4double z; // atomic number
    G4String name;
    G4String symbol;

    _elementH  = new G4Element(name = "Hydrogen", symbol = "H", z = 1, a = 1.008 * g / mole);
    _elementC  = new G4Element(name = "Carbon", symbol = "C", z = 6, a = 12.01 * g / mole);
    _elementN  = new G4Element(name = "Nitrogen", symbol = "N", z = 7, a = 14.01 * g / mole);
    _elementO  = new G4Element(name = "Oxygen", symbol = "O", z = 8, a = 16.00 * g / mole);
    _elementAl = new G4Element(name = "Aluminum", symbol = "Al", z = 13, a = 26.98 * g / mole);
    _elementSi = new G4Element(name = "Silicon", symbol = "Si", z = 14, a = 28.09 * g / mole);
    _elementK  = new G4Element(name = "Potassium", symbol = "K", z = 19, a = 39.0983 * g / mole);
    _elementCr = new G4Element(name = "Chromium", symbol = "Cr", z = 24, a = 51.996 * g / mole);
    _elementFe = new G4Element(name = "Iron", symbol = "Fe", z = 26, a = 55.845 * g / mole);
    _elementNi = new G4Element(name = "Nickel", symbol = "Ni", z = 28, a = 58.693 * g / mole);
    _elementNa = new G4Element(name = "Sodium", symbol = "Na", z = 11, a = 22.989768 * g / mole);
    _elementI  = new G4Element(name = "Iodine", symbol = "I", z = 53, a = 126.90447 * g / mole);
    _elementCs = new G4Element(name = "Cesium", symbol = "Cs", z = 55, a = 132.90543 * g / mole);
    _elementCa = new G4Element(name = "Calcium", symbol = "Ca", z = 20, a = 40.08 * g / mole);
    _elementF  = new G4Element(name = "Fluorine", symbol = "F", z = 9, a = 19.00 * g / mole);
    _elementCu = new G4Element(name = "Copper", symbol = "Cu", z = 29, a = 63.546 * g / mole);
    _elementPb = new G4Element(name = "Lead", symbol = "Pb", z = 82, a = 207.19 * g / mole);
    _elementNb = new G4Element(name = "Niobium", symbol = "Nb", z = 41, a = 92.90638 * g / mole);
    _elementAu = new G4Element(name = "Gold", symbol = "Au", z = 79, a = 196.97 * g / mole);
    _elementGe = new G4Element(name = "Germanium", symbol = "Ge", z = 32, a = 71.92 * g / mole);

    // === Material ===============================================
    G4double density;
    G4double mol;
    G4int nelements;
    G4int natoms;
    G4MaterialPropertiesTable *MPT;

    // --- Air  N=70% O=30% ---------
    name      = "Air";
    density   = 1.29e-3 * g / cm3;
    nelements = 2;

    _air = new G4Material(name, density, nelements);
    _air->AddElement(_elementN, 70 * perCent);
    _air->AddElement(_elementO, 30 * perCent);

    // --- PMT vacuum is very dilute air -------
    density              = 1e-3 * kGasThreshold; // from PhysicalConstants.h
    G4double temperature = STP_Temperature;      // from PhysicalConstants.h
    G4double pressure    = STP_Pressure * density / (1.29e-3 * g / cm3);
    PMT_Vac = new G4Material(name = "PMT_Vac", density, 1, kStateGas, temperature, pressure);
    PMT_Vac->AddMaterial(_air, 1.);

    // --- Rock  SiO2 ---------------
    name      = "Rock";
    density   = 2.7 * g / cm3;
    nelements = 2;

    _rock = new G4Material(name, density, nelements);
    _rock->AddElement(_elementSi, natoms = 1);
    _rock->AddElement(_elementO, natoms = 2);

    // --- Glass  SiO2 ---------------
    name      = "Glass";
    density   = 2.2 * g / cm3; // changed 1999/12/03 (was 2.7*g/cm3) -- GAS
    nelements = 2;

    _glass = new G4Material(name, density, nelements);
    _glass->AddElement(_elementSi, natoms = 1);
    _glass->AddElement(_elementO, natoms = 2);

    // --- Steel  Fe ----------------
    name      = "Steel";
    density   = 7.87 * g / cm3;
    nelements = 1;

    _steel = new G4Material(name, density, nelements);
    _steel->AddElement(_elementFe, natoms = 1);

    // --- Water  H2O ---------------
    name      = "Water";
    density   = 1.0 * g / cm3;
    nelements = 2;

    _water = new G4Material(name, density, nelements);
    _water->AddElement(_elementH, natoms = 2);
    _water->AddElement(_elementO, natoms = 1);

    // --- Stainless Steel  71% Fe, 19% Cr, 10% Ni ------
    name      = "StainlessSteel";
    density   = 7.87 * g / cm3;
    nelements = 3;

    _stainless = new G4Material(name, density, nelements);
    _stainless->AddElement(_elementFe, 0.71);
    _stainless->AddElement(_elementCr, 0.19);
    _stainless->AddElement(_elementNi, 0.10);

    // --- Lead  Pb ------
    name      = "Lead";
    density   = 11.35 * g / cm3;
    nelements = 1;

    _lead = new G4Material(name, density, nelements);
    _lead->AddElement(_elementPb, natoms = 1);

    // --- Aluminum  Al ------
    name      = "Aluminum";
    density   = 2.7 * g / cm3;
    nelements = 1;

    _aluminum = new G4Material(name, density, nelements);
    _aluminum->AddElement(_elementAl, natoms = 1);

    // --- Copper Cu ------
    name      = "Copper";
    density   = 8.96 * g / cm3;
    nelements = 1;

    _copper = new G4Material(name, density, nelements);
    _copper->AddElement(_elementCu, natoms = 1);

    name      = "Copper2";
    density   = 8.96 * g / cm3;
    nelements = 1;

    _copper2 = new G4Material(name, density, nelements);
    _copper2->AddElement(_elementCu, natoms = 1);

    // --- Niobium Nb ------
    name      = "Niobium";
    density   = 8.57 * g / cm3;
    nelements = 1;

    _niobium = new G4Material(name, density, nelements);
    _niobium->AddElement(_elementNb, natoms = 1);

    // --- Gold ------
    name      = "Gold";
    density   = 19.32 * g / cm3;
    nelements = 1;

    _gold = new G4Material(name, density, nelements);
    _gold->AddElement(_elementAu, natoms = 1);

    // --- Vm2000 ------
    name      = "Vm2000";
    density   = 0.9 * g / cm3;
    nelements = 2;

    _vm2000 = new G4Material(name, density, nelements);
    _vm2000->AddElement(_elementC, natoms = 2);
    _vm2000->AddElement(_elementH, natoms = 4);

    // --- N2 gas ------
    name      = "N2_Gas";
    density   = 1.165 * g / cm3;
    nelements = 1;

    _N2_Gas = new G4Material(name, density, nelements);
    _N2_Gas->AddElement(_elementN, natoms = 1);

    // --- Calcium ------
    name      = "Calcium";
    density   = 1.165 * g / cm3;
    nelements = 1;

    _calcium = new G4Material(name, density, nelements);
    _calcium->AddElement(_elementCa, natoms = 1);

    // --- Mineral Oil  (CH2)n ------
    name      = "MineralOil";
    density   = 0.77 * g / cm3;
    nelements = 2;

    _mineralOil = new G4Material(name, density, nelements);
    _mineralOil->AddElement(_elementC, natoms = 1);
    _mineralOil->AddElement(_elementH, natoms = 2);

    // Use the chemical formula as a useful label
    _mineralOil->SetChemicalFormula("OIL");

    // --- Vacuum ------
    a       = 4. * g / mole;
//    density = 0.1786 * mg / cm3;
    density = 0. * mg / cm3;
    _vacuum = new G4Material("HeliumGas", z = 2., a, density, kStateGas, 4.3 * kelvin, 1.e-8 * bar);

    // --- GeWafer ------
    name      = "GeWafer";
    density   = 5.323 * g / cm3;
    nelements = 1;

    _gewafer = new G4Material(name, density, nelements);
    _gewafer->AddElement(_elementGe, natoms = 1);

    // For the moment, no molecular weight defined... Use Dodecane!

    //
    // .......................... Dodecane .............................
    //
    density   = 0.749 * g / cm3;
    nelements = 2;
    Dodecane  = new G4Material("Dodecane", density, nelements);

    // Use the chemical formula as a label for the function in the scintillator
    Dodecane->SetChemicalFormula("OIL");

    Dodecane->AddElement(_elementC, 12);
    Dodecane->AddElement(_elementH, 26);

    // Calculate the molecular weight
    mol = _elementC->GetA() * 12 + _elementH->GetA() * 26;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PC
    Dodecane->SetMaterialPropertiesTable(MPT);

    //
    // ............................. PC ................................
    //
    // Pseudo-cumene (C9 H12) also called 1,2,4-Trimethybenzene

    density      = 0.8758 * g / cm3; // at T=20 deg C
    nelements    = 2;
    Pseudocumene = new G4Material(name = "pseudocumene", density, nelements);
    Pseudocumene->AddElement(_elementC, 9);
    Pseudocumene->AddElement(_elementH, 12);

    // Use the chemical formula as a label
    Pseudocumene->SetChemicalFormula("AROMATIC");

    // Calculate the molecular weight
    mol = _elementC->GetA() * 9 + _elementH->GetA() * 12;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the pseudocumene
    Pseudocumene->SetMaterialPropertiesTable(MPT);

    //
    // ............................. LAB ................................
    //
    // LAB (CnH2n+1-C6H5, n=9~14) //added by EJJeon (2008-02-26)
    G4int num_C;
    G4int num_H;
    char Name[15];
    density   = 0.86 * g / cm3;
    nelements = 2;
    for (int i = 0; i < 6; i++) {
        num_C = i + 15;
        num_H = 2 * (i + 9) + 6;
        sprintf(Name, "LAB_n=%i", i + 9);
        LAB[i] = new G4Material(Name, density, nelements);
        LAB[i]->AddElement(_elementC, num_C);
        LAB[i]->AddElement(_elementH, num_H);

        // Use the chemical formula as a label
        LAB[i]->SetChemicalFormula("AROMATIC");

        // Calculate the molecular weight
        mol = _elementC->GetA() * num_C + _elementH->GetA() * num_H;
        // Allocate memory for a new Material Property Table
        MPT = new G4MaterialPropertiesTable();
        // Fill with the molecular weight
        MPT->AddConstProperty("MOL", mol / g);
        // Attach this MPT to the pseudocumene
        LAB[i]->SetMaterialPropertiesTable(MPT);
    }

    //
    // ............................. PXE ..............................
    //

    density   = 0.99 * g / cm3;
    nelements = 2;
    PXE       = new G4Material("PXE", density, nelements);

    // Use the chemical formula as a label
    PXE->SetChemicalFormula("AROMATIC");

    PXE->AddElement(_elementC, 16);
    PXE->AddElement(_elementH, 18);

    // Calculate the molecular weight
    mol = _elementC->GetA() * 16 + _elementH->GetA() * 18;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PXE
    PXE->SetMaterialPropertiesTable(MPT);

    //
    // ............................. PPO ...............................
    //
    // PPO (C15 H11 N 0) -- also called DPO, 2,5-diphenyloxazole

    density = 1.06 * g / cm3; // ??? at T=?
    PPO     = new G4Material(name = "PPO", density, nelements = 4);

    // Use the chemical formula as a label
    PPO->SetChemicalFormula("FLUOR");

    PPO->AddElement(_elementC, 15);
    PPO->AddElement(_elementH, 11);
    PPO->AddElement(_elementN, 1);
    PPO->AddElement(_elementO, 1);

    // Calculate the molecular weight
    mol = _elementC->GetA() * 15 + _elementH->GetA() * 11 + _elementN->GetA() * 1 +
          _elementO->GetA() * 1;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PC
    PPO->SetMaterialPropertiesTable(MPT);

    //
    // ............................. BPO ...............................
    //
    // BPO (C21 H15 N O) -- like PPO, with one more phenyl ring

    density = 1.06 * g / cm3; // unknown (set to the PPO value)

    BPO = new G4Material(name = "BPO", density, nelements = 4);

    // Use the chemical formula as a label
    BPO->SetChemicalFormula("FLUOR");

    BPO->AddElement(_elementC, 21);
    BPO->AddElement(_elementH, 15);
    BPO->AddElement(_elementN, 1);
    BPO->AddElement(_elementO, 1);

    // Calculate the molecular weight
    mol = _elementC->GetA() * 21 + _elementH->GetA() * 15 + _elementN->GetA() * 1 +
          _elementO->GetA() * 1;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PC
    BPO->SetMaterialPropertiesTable(MPT);

    //
    // .............................. Bis-MSB .....................................
    //

    density   = 1.3 * g / cm3; // Unknown
    nelements = 2;
    BisMSB    = new G4Material("Bis-MSB", density, nelements);

    // Use the chemical formula as a label
    BisMSB->SetChemicalFormula("WLS");

    BisMSB->AddElement(_elementC, 24);
    BisMSB->AddElement(_elementH, 22);
    //

    // Calculate the molecular weight
    mol = _elementC->GetA() * 24 + _elementH->GetA() * 22;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the Bis-MSB
    BisMSB->SetMaterialPropertiesTable(MPT);

    // GenericLAND scintillator
    density               = 0.78 * g / cm3;
    Scintillator          = new G4Material(name = "scintillator", density, nelements = 3);
    G4double PPO_fraction = 1.5 * g / (1e3 * cm3 * density); // 1.5 g/l
    Scintillator->AddMaterial(_mineralOil, 0.80 / (1.0 + PPO_fraction));
    Scintillator->AddMaterial(Pseudocumene, 0.20 / (1.0 + PPO_fraction));
    Scintillator->AddMaterial(PPO, PPO_fraction / (1.0 + PPO_fraction));
    Scintillator->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);

    // EJ: Gd-loaded Lab target material
    G4NistManager *man      = G4NistManager::Instance();
    G4Material *_materialGd = man->FindOrBuildMaterial("G4_Gd");
    density                 = 0.78 * g / cm3;
    nelements               = 4;
    GdLoadedScint           = new G4Material(name = "GdLoadedScint", density, nelements);

    GdLoadedScint->AddMaterial(_mineralOil, 0.80 / (1.0 + PPO_fraction));
    GdLoadedScint->AddMaterial(Pseudocumene, 0.199 / (1.0 + PPO_fraction));
    GdLoadedScint->AddMaterial(_materialGd, 0.001 / (1.0 + PPO_fraction)); // EJ: 0.1% loaded
    GdLoadedScint->AddMaterial(PPO, PPO_fraction / (1.0 + PPO_fraction));

    //
    // ............................. CaMoO4 ...............................
    //
    G4int iz; // atomic number(protons)
    G4int n;  // number of nucleons
    G4int nisotope;
    G4double abundance;

    G4Isotope *Ca40       = new G4Isotope(name = "Calcium", iz = 20, n = 40, a = 40.078 * g / mole);
    G4Element *_elementCa = new G4Element(name = "enriched Calsium", symbol = "Ca", nisotope = 1);
    _elementCa->AddIsotope(Ca40, abundance = 100. * perCent);

    // G4Isotope* Mo98 = new G4Isotope(name="Molybdenum98", iz=42, n=98, a=97.9054073*g/mole);
    G4Isotope *Mo100 =
        new G4Isotope(name = "Molybdenum100", iz = 42, n = 100, a = 99.907477 * g / mole);
    G4Element *_elementMo =
        new G4Element(name = "enriched Molybdenum", symbol = "Mo", nisotope = 1);
    _elementMo->AddIsotope(Mo100, abundance = 100. * perCent);

    density   = 4.34 * g / cm3;
    nelements = 3;
    CaMoO4    = new G4Material(name = "CaMoO4", density, nelements);
    CaMoO4->AddElement(_elementCa, 1);
    CaMoO4->AddElement(_elementMo, 1);
    CaMoO4->AddElement(_elementO, 4);
    CaMoO4->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);

    //
    //............................... PbMoO4 .............................
    // edited by Mona
    /*
       G4Isotope* Pb207 = new G4Isotope(name="Lead", iz=82, n=207, a=207.19*g/mole);
       G4Element* _elementPb = new G4Element(name="enriched Lead", symbol="Pb", nisotope=1);
       _elementPb->AddIsotope(Pb207, abundance= 100.*perCent)

       density = 6.95*g/cm3;
       nelements = 3;
       PbMoO4 = new G4Material(name="PbMoO4",density,nelements);
       PbMoO4->AddElement(_elementPb,1);
       PbMoO4->AddElement(_elementMo,1);
       PbMoO4->AddElement(_elementO,4);
       PbMoO4->GetIonisation()->SetBirksConstant(0.117*mm/MeV);
       */
    //
    // ............................. CsI crystal ...........................
    //
    name      = "CsI";
    density   = 4.51 * g / cm3;
    nelements = 2;
    CsI       = new G4Material(name, density, nelements);
    CsI->AddElement(_elementCs, natoms = 1);
    CsI->AddElement(_elementI, natoms = 1);
    CsI->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);
    // EJ: Birks Constant for the LXe scintillator(0.126*mm/MeV)
    // EJ: Birks Constant for the Water scintillator(0.126*mm/MeV)
    // EJ: Birks Constant for the LAB-based liquid scintillator(0.117*mm/MeV)
    // EJ: Birks Constant for the Gd-loaded LS_LAB scintilltor(0.124*mm/MeV)

    //
    // ............................. NaI crystal ...........................
    //
    name      = "NaI";
    density   = 3.67 * g / cm3;
    nelements = 2;
    NaI       = new G4Material(name, density, nelements);
    NaI->AddElement(_elementNa, natoms = 1);
    NaI->AddElement(_elementI, natoms = 1);
    NaI->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);

    //
    // ............................. Quartz SiO2 ...........................
    //
    name      = "Quartz";
    density   = 2.64 * g / cm3;
    nelements = 2;
    _quartz   = new G4Material(name, density, nelements);
    _quartz->AddElement(_elementSi, natoms = 1);
    _quartz->AddElement(_elementO, natoms = 2);

    //
    // ............................. Grease SiO2 ...........................
    //
    name      = "Grease";
    density   = 1.06 * g / cm3;
    nelements = 2;
    _grease   = new G4Material(name, density, nelements);
    _grease->AddElement(_elementO, 2);
    _grease->AddElement(_elementSi, 1);

    //
    // ............................. Teflon PTFE ...........................
    //
    name      = "Teflon";
    density   = 2.2 * g / cm3;
    nelements = 2;
    _teflon   = new G4Material(name, density, nelements);
    _teflon->AddElement(_elementC, natoms = 2);
    _teflon->AddElement(_elementF, natoms = 4);

    //
    // ............................. Ethyl Alcohol ....................
    //
    _ethylalcohol = man->FindOrBuildMaterial("G4_ETHYL_ALCOHOL");

    // --- Nylon    H        O  -----
    //             -N-(CH2)5-C-
    name      = "Nylon";
    density   = 0.805 * g / cm3;
    nelements = 4;

    _nylon = new G4Material(name, density, nelements);
    _nylon->AddElement(_elementH, natoms = 11);
    _nylon->AddElement(_elementC, natoms = 6);
    _nylon->AddElement(_elementO, natoms = 1);
    _nylon->AddElement(_elementN, natoms = 1);

    //               H H
    // --- Acrylic  -C-C- --------------------
    //               H COOCH3
    name      = "Acrylic";
    density   = 1.14 * g / cm3;
    nelements = 3;

    _acrylic = new G4Material(name, density, nelements);
    _acrylic->AddElement(_elementH, natoms = 6);
    _acrylic->AddElement(_elementC, natoms = 4);
    _acrylic->AddElement(_elementO, natoms = 2);

    name      = "blackAcryl";
    density   = 1.14 * g / cm3;
    nelements = 3;

    _blackAcryl = new G4Material(name, density, nelements);
    _blackAcryl->AddElement(_elementH, natoms = 6);
    _blackAcryl->AddElement(_elementC, natoms = 4);
    _blackAcryl->AddElement(_elementO, natoms = 2);

    // --- Polyethylene
    name      = "Polyethylene";
    density   = 0.91 * g / cm3;
    nelements = 2;

    _polyethylene = new G4Material(name, density, nelements);
    _polyethylene->AddElement(_elementH, natoms = 2);
    _polyethylene->AddElement(_elementC, natoms = 1);

    // --- Tyvek  ==  High Density Polyethylene:  (...-CH2-CH2-...)*n
    name      = "Tyvek";
    density   = 0.96 * g / cm3;
    nelements = 2;

    _tyvek = new G4Material(name, density, nelements);
    _tyvek->AddElement(_elementH, natoms = 2);
    _tyvek->AddElement(_elementC, natoms = 1);

    // photocathode material, approximated as elemental cesium
    density          = 5. * g / cm3; // true??
    Photocathode_mat = new G4Material(name = "photocathode", density, nelements = 1);
    Photocathode_mat->AddElement(_elementK, 1);

    // --- kevlar == (-NH-C6H4-NH-CO-C6H4-CO-)*n
    name      = "Kevlar";
    density   = 1.44 * g / cm3; // ??
    nelements = 4;

    _kevlar = new G4Material(name, density, nelements);
    _kevlar->AddElement(_elementH, natoms = 10);
    _kevlar->AddElement(_elementC, natoms = 14);
    _kevlar->AddElement(_elementO, natoms = 2);
    _kevlar->AddElement(_elementN, natoms = 2);

    // == Add material properties (RINDEX, ABSLENGTH, etc) ================
    // first open file, using CupDATA variable if set (this used to be one line)
    std::ifstream ifs;
    if (getenv("CupDATA") != NULL)
        ifs.open((G4String(getenv("CupDATA")) + "/materials.dat").c_str());
    else
        ifs.open("data/materials.dat");
    if (ifs.fail()) {
        G4cerr << "Error, material properties file could not be opened.\n";
        if (getenv("CupDATA") == NULL)
            G4cerr << "CupDATA environment variable is not set, so I was looking"
                      " for data/materials.dat from the current directory."
                   << G4endl;
        else
            G4cerr << "I was looking for materials.dat in the CupDATA directory, "
                   << getenv("CupDATA") << G4endl;
        // G4Exception("Error, material properties file could not be opened.\n");
        G4Exception(" ", " ", JustWarning,
                    "Error, material properties file could not be opened.\n");
    }
    // now read materials, keeping error count
    int errorCount_ReadMaterials = CupInputDataReader::ReadMaterials(ifs);
    // close file
    ifs.close();

    if (errorCount_ReadMaterials) {
        G4cerr << "Error count after reading material properties file is "
               << errorCount_ReadMaterials << G4endl;
        // G4Exception("Error reading material properties file.\n");
        G4Exception(" ", " ", JustWarning, "Error reading material properties file.\n");
    }

    // == Make sure WLSSPECTRUM is set ==============
    G4MaterialPropertiesTable *mpt_scint = Scintillator->GetMaterialPropertiesTable();
    if (mpt_scint == nullptr) {
        // G4Exception("Error, scintillator has no material properties table!");
        G4Exception(" ", " ", JustWarning, "Error, scintillator has no material properties table!");
    } else {
        G4MaterialPropertyVector *mpv_scint_reemission = mpt_scint->GetProperty("WLSSPECTRUM");
        if (mpv_scint_reemission == nullptr)
            // G4Exception("Error, scintillator has no WLSSPECTRUM vector!");
            G4Exception(" ", " ", JustWarning, "Error, scintillator has no WLSSPECTRUM vector!");
    }

    // == Create optical surfaces
    Photocathode_opsurf = new G4OpticalSurface("Photocathode_opsurf");
    Photocathode_opsurf->SetType(dielectric_metal); // ignored if RINDEX defined
    Photocathode_opsurf->SetMaterialPropertiesTable(
        G4Material::GetMaterial("photocathode")->GetMaterialPropertiesTable());

    GeWafer_opsurf = new G4OpticalSurface("GeWafer_opsurf");
    GeWafer_opsurf->SetType(dielectric_metal); // ignored if RINDEX defined
    GeWafer_opsurf->SetMaterialPropertiesTable(_gewafer->GetMaterialPropertiesTable());

    Stainless_opsurf = new G4OpticalSurface("Stainless_opsurf");
    Stainless_opsurf->SetFinish(ground);
    Stainless_opsurf->SetModel(glisur);
    Stainless_opsurf->SetType(dielectric_metal);
    Stainless_opsurf->SetPolish(0.1); // a guess -- FIXME?
    Stainless_opsurf->SetMaterialPropertiesTable(_stainless->GetMaterialPropertiesTable());

    Polyethylene_opsurf = new G4OpticalSurface("Polyethylene_opsurf");
    Polyethylene_opsurf->SetFinish(ground);              // a guess -- FIXME?
    Polyethylene_opsurf->SetModel(glisur);               // a guess -- FIXME?
    Polyethylene_opsurf->SetType(dielectric_dielectric); // a guess -- FIXME?
    Polyethylene_opsurf->SetPolish(0.7);                 // a guess -- FIXME?
    Polyethylene_opsurf->SetMaterialPropertiesTable(_polyethylene->GetMaterialPropertiesTable());

    Tyvek_opsurf = new G4OpticalSurface("Tyvek_opsurf");
    Tyvek_opsurf->SetFinish(ground);
    Tyvek_opsurf->SetModel(glisur);
    Tyvek_opsurf->SetType(dielectric_metal);
    Tyvek_opsurf->SetPolish(0.01); // a guess -- FIXME
    Tyvek_opsurf->SetMaterialPropertiesTable(_tyvek->GetMaterialPropertiesTable());

    BlackSheet_opsurf = new G4OpticalSurface("BlackSheet_opsurf");
    BlackSheet_opsurf->SetFinish(ground);
    BlackSheet_opsurf->SetModel(glisur);
    BlackSheet_opsurf->SetType(dielectric_metal);
    BlackSheet_opsurf->SetPolish(0.1); // a guess -- FIXME
    BlackSheet_opsurf->SetMaterialPropertiesTable(_blackAcryl->GetMaterialPropertiesTable());

    Teflon_opsurf = new G4OpticalSurface("Teflon_opsurf");
    Teflon_opsurf->SetFinish(ground);
    Teflon_opsurf->SetModel(glisur);
    Teflon_opsurf->SetType(dielectric_metal);
    Teflon_opsurf->SetPolish(0.01); // a guess -- FIXME
    Teflon_opsurf->SetMaterialPropertiesTable(_teflon->GetMaterialPropertiesTable());

    Vikuiti_opsurf = new G4OpticalSurface("Vikuiti_opsurf");
    Vikuiti_opsurf->SetFinish(ground);
    Vikuiti_opsurf->SetModel(glisur);
    Vikuiti_opsurf->SetType(dielectric_metal);
    Vikuiti_opsurf->SetPolish(0.01); // a guess -- FIXME
    Vikuiti_opsurf->SetMaterialPropertiesTable(_vm2000->GetMaterialPropertiesTable());

    // success!
    materials_built = true;
}

// ----------------------------------------------------------------

// NOTE:  code for CupDetectorConstruction::ConstructAMoRE10(),
//        CupDetectorConstruction::ConstructTestBench(), and
//        CupDetectorConstruction::ConstructSET25wPMT() are located in
//        separate files (e.g., Cup_ConstructAMoRE10.cc)

// ----------------------------------------------------------------

/** GetPhysicalVolumeByName allows a G4VPhysicalVolume to be found in
  the G4PhysicalVolumeStore by its name.  This remedies a deficiency
  in G4PhysicalVolumeStore. */
G4VPhysicalVolume *CupDetectorConstruction::GetPhysicalVolumeByName(const G4String &name) {
    // access the store of physical volumes
    G4PhysicalVolumeStore *pvs = G4PhysicalVolumeStore::GetInstance();
    G4VPhysicalVolume *pv;
    G4int npv = pvs->size();
    G4int ipv;
    for (ipv = 0; ipv < npv; ipv++) {
        pv = (*pvs)[ipv];
        if (!pv) break;
        if (pv->GetName() == name) return pv;
    }
    return NULL;
}
