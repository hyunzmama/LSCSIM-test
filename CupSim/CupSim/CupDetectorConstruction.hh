
#ifndef CupDetectorConstruction_HH
#define CupDetectorConstruction_HH 1

#include "map"

#include "globals.hh"

#include "G4ThreeVector.hh"
#include "G4VUserDetectorConstruction.hh"

#include "CupSim/Cup_PMT_LogicalVolume.hh" // for ePmtStyle

class G4Element;
class G4Material;
class G4VPhysicalVolume;
class G4OpticalSurface;

using namespace CLHEP;

class CupDetectorConstruction : public G4VUserDetectorConstruction {
  public:
    typedef enum {
        kDetector_TestBench,
        kNumGenericDetectors
    } eDetector;

    CupDetectorConstruction();          // constructor
    virtual ~CupDetectorConstruction(); // destructor

    virtual G4VPhysicalVolume *Construct(); // make the volumes, return ptr to world

    static G4VPhysicalVolume *GetPhysicalVolumeByName(const G4String &name);

    G4VPhysicalVolume *GetWorld() const { return world_phys; }

    virtual int GetNumDetectorTypes() { return kNumGenericDetectors; }
    virtual G4String GetDetectorTypeName(int i);

    int GetWhichDetector(void) { return whichDetector; }
    ePmtStyle GetWhichPmtStyle(void) { return whichPmtStyle; }
    G4String GetWhichCalibrationDevice(void) { return calDeviceName; }
    G4ThreeVector GetCalibrationPosition(void) { return calPosition; }

    static G4int GetDetectorType() { return whichDetector; } // EJ

    virtual void SetWhichDetector(int w) { whichDetector = w; }
    void SetWhichPmtStyle(ePmtStyle w) { whichPmtStyle = w; }
    virtual void SetWhichCalibrationDevice(G4String newDevice) { calDeviceName = newDevice; }
    virtual void SetCalibrationPosition(G4ThreeVector newPos) { calPosition = newPos; }

  protected:
    void ConstructMaterials();          // make all needed materials
    void ConstructTestBench();  // make the TestBench

    // the following pointers are kept for convenience; they don't have to be!
    // [but if we didn't have them, would have to rewrite ConstructGenericLAND to
    //  lookup materials by name using G4Material::GetMaterial(G4String name)]
    G4Element *_elementH;
    G4Element *_elementC;
    G4Element *_elementN;
    G4Element *_elementO;
    G4Element *_elementAl;
    G4Element *_elementK;
    G4Element *_elementCa;
    G4Element *_elementSi;
    G4Element *_elementCr;
    G4Element *_elementFe;
    G4Element *_elementNi;
    G4Element *_elementNa;
    G4Element *_elementI;
    G4Element *_elementCs;
    G4Element *_elementF;
    G4Element *_elementCu;
    G4Element *_elementPb;
    G4Element *_elementNb;
    G4Element *_elementAu;
    G4Element *_elementGe;
    G4Material *_air;
    G4Material *_rock;
    G4Material *_glass;
    G4Material *_steel;
    G4Material *_water;
    G4Material *_stainless;
    G4Material *_lead;
    G4Material *_aluminum;
    G4Material *_copper;
    G4Material *_copper2;
    G4Material *_niobium;
    G4Material *_gold;
    G4Material *_vm2000;
    G4Material *_N2_Gas;
    G4Material *_calcium;
    G4Material *_mineralOil;
    G4Material *_nylon;
    G4Material *_acrylic;
    G4Material *_blackAcryl;
    G4Material *_polyethylene;
    G4Material *_tyvek;
    G4Material *_teflon;
    G4Material *_quartz;
    G4Material *_grease;
    G4Material *_kevlar;
    G4Material *_vacuum;
    G4Material *_ethylalcohol;
    G4Material *_gewafer;
    G4Material *PMT_Vac;
    G4Material *Dodecane;
    G4Material *Pseudocumene;
    G4Material *PXE;
    G4Material *PPO;
    G4Material *BPO;
    G4Material *BisMSB;
    G4Material *Scintillator;
    G4Material *Photocathode_mat;
    G4Material *GdLoadedScint;
    G4Material *LAB[6];
    G4Material *Lithium;
    G4Material *CaMoO4;
    G4Material *NaI;
    G4Material *CsI;
    G4OpticalSurface *Photocathode_opsurf;
    G4OpticalSurface *Stainless_opsurf;
    G4OpticalSurface *Polyethylene_opsurf;
    G4OpticalSurface *Tyvek_opsurf;
    G4OpticalSurface *BlackSheet_opsurf;
    G4OpticalSurface *Teflon_opsurf;
    G4OpticalSurface *Vikuiti_opsurf;
    G4OpticalSurface *GeWafer_opsurf;

    G4VPhysicalVolume *world_phys;

    // int whichDetector;
    static int whichDetector; // EJ
    ePmtStyle whichPmtStyle;

    G4String calDeviceName;
    G4ThreeVector calPosition;

    G4bool materials_built;

    class CupDetectorMessenger *myMessenger;
};

#endif
