
#include "globals.hh"

#include "CupSim/CupBoxSD.hh"                // for making sensitive boxes
#include "CupSim/CupDetectorConstruction.hh" // the DetectorConstruction class header
#include "CupSim/CupPMTOpticalModel.hh"      // for same PMT optical model as main sim
#include "CupSim/CupPMTSD.hh"                // for making sensitive photocathodes
#include "CupSim/CupScintSD.hh"              // for making sensitive photocathodes
#include "CupSim/CupTorusStack.hh"           // for making the balloon
#include "CupSim/Cup_PMT_LogicalVolume.hh"   // for making PMT assemblies

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

#include "G4OpticalSurface.hh"

// == Construct Geometry for GenericLAND test bench============================
// This will construct a (20_m)^3 "laboratory" containing the things in the
// Tohoku Physics Dept. C building basement (90-cm test bench and 6.8-m MLCS)
// plus some other things (waveshifter test, (3-m)^3 cube of Fe, alphaquench..)
//
void CupDetectorConstruction::ConstructTestBench() {
    // -- Experimental Hall
    G4double bounding_size    = 20. * meter / 2.0;
    G4Box *boxHall            = new G4Box("hallbox", bounding_size, bounding_size, bounding_size);
    G4LogicalVolume *logiHall = new G4LogicalVolume(boxHall, _air, "logiHall", 0, 0, 0);
    logiHall->SetVisAttributes(G4VisAttributes::Invisible);

    G4VPhysicalVolume *physHall =
        new G4PVPlacement(/*rotation*/ 0, G4ThreeVector(0, 0, 0), logiHall, "physHall",
                          NULL,  // parent
                          false, // no boolean ops
                          0);

    world_phys = physHall;

    ////////////////////////////////////////////
    // ** The 90-cm test bench
    //
    // -- make acrylic box for 90-cm test bench
    {
        G4double box_size = 1200. * millimeter / 2.0;
        G4LogicalVolume *logi_acryl_box =
            new G4LogicalVolume(new G4Box("acryl_box", box_size, box_size, box_size), _acrylic,
                                "acryl_box_log", 0, 0, 0);
        G4VPhysicalVolume *phys_acryl_box = new G4PVPlacement(0, // no rotation
                                                              G4ThreeVector(5.0 * m, 5.0 * m, 0.),
                                                              logi_acryl_box,   // logical vol
                                                              "acryl_box_phys", // name
                                                              logiHall, // mother logical vol
                                                              false,    // no boolean ops
                                                              0);       // copy number

        // -- create water-filled interior of box
        box_size -= 10.0 * mm;
        G4LogicalVolume *logi_water_box = new G4LogicalVolume(
            new G4Box("water_box", box_size, box_size, box_size), _water, "water_box_log", 0, 0, 0);
        G4VPhysicalVolume *phys_water_box = new G4PVPlacement(0, // no rotation
                                                              G4ThreeVector(0., 0., 0.),
                                                              logi_water_box,   // logical vol
                                                              "water_box_phys", // name
                                                              logi_acryl_box, // mother logical vol
                                                              false,          // no boolean ops
                                                              0);             // copy number

        // scintillator-filled balloon with chimney
        CupTorusStack *balloon_solid = new CupTorusStack("balloon_solid");
        G4double balloon_z_edge[4], balloon_r_edge[4], balloon_z0[3];
        G4double r_chimney = 0.1 * m;
        G4double r_balloon = 0.9 * m / 2.0;
        balloon_z_edge[0]  = -r_balloon;
        balloon_r_edge[0]  = 0.0;
        balloon_z0[0]      = 0.0;
        balloon_z_edge[1]  = 0.0;
        balloon_r_edge[1]  = r_balloon;
        balloon_z0[1]      = 0.0;
        balloon_z_edge[2]  = sqrt(r_balloon * r_balloon - r_chimney * r_chimney);
        balloon_r_edge[2]  = r_chimney;
        balloon_z0[2]      = 0.0;
        balloon_z_edge[3]  = box_size;
        balloon_r_edge[3]  = r_chimney;
        balloon_solid->SetAllParameters(3, balloon_z_edge, balloon_r_edge, balloon_z0);
        G4LogicalVolume *logi_balloon =
            new G4LogicalVolume(balloon_solid, Scintillator, "balloon_log");
        G4VPhysicalVolume *phys_balloon = new G4PVPlacement(0, // no rotation
                                                            G4ThreeVector(0., 0., 0.),
                                                            logi_balloon,   // logical vol
                                                            "balloon_phys", // name
                                                            logi_water_box, // mother logical vol
                                                            false, 0);

        // and so on ...
        // PMTs encapsulated in acrylic on sides
        // (see pg. 383 of Ofunato conference proceedings)
        // ...
    }

    ////////////////////////////////////////////////////////////////
    // The Tajima/Suekane 6.8-m "MLCS" experiment (total light yield)
    //
    // -- make a 8-meter-long bounding box to contain the experiment
    {
        G4RotationMatrix *my_rot = new G4RotationMatrix();
        my_rot->rotateY(M_PI / 2.0); // local z-axis is east/west
        G4LogicalVolume *logi_MLCS_box = new G4LogicalVolume(
            new G4Box("MLCS_box", 0.5 * m, 0.5 * m, 4.0 * m), _air, "MLCS_box_log", 0, 0, 0);
        G4VPhysicalVolume *phys_MLCS_box =
            new G4PVPlacement(my_rot, G4ThreeVector(-5. * m, 5. * m, 0.),
                              logi_MLCS_box,   // logical vol
                              "MLCS_box_phys", // name
                              logiHall,        // mother logical vol
                              false,           // no boolean ops
                              0);              // copy number
        logi_MLCS_box->SetVisAttributes(G4VisAttributes::Invisible);

        // make the tube of scintillator
        G4LogicalVolume *logi_MLCS_scinti =
            new G4LogicalVolume(new G4Tubs("MLCS_scinti", 0.0, 70. * mm, // rmin, rmax
                                           3.495 * m,                    // half-length
                                           0.0, twopi),
                                Scintillator, "MLCS_scinti_log", 0, 0, 0);
        G4VPhysicalVolume *phys_MLCS_scinti = new G4PVPlacement(0, // no rotation
                                                                G4ThreeVector(0., 0., 0.),
                                                                logi_MLCS_scinti,   // logical vol
                                                                "MLCS_scinti_phys", // name
                                                                logi_MLCS_box, // mother logical vol
                                                                false,         // no boolean ops
                                                                0);            // copy number

        // Scintillator
        CupScintSD *TGSD;
        G4SDManager *SDman = G4SDManager::GetSDMpointer();
        G4String SDname;
        TGSD = new CupScintSD(SDname = "/cupdet/TGSD", 1);
        SDman->AddNewDetector(TGSD);
        logi_MLCS_scinti->SetSensitiveDetector(TGSD);

        // make surrounding tube
        // Note: here we use stainless steel instead of anodized aluminum
        //       and we use black sheet optical surface instead of special one
        G4LogicalVolume *logi_MLCS_tube =
            new G4LogicalVolume(new G4Tubs("MLCS_tube", 69.0 * mm, 70. * mm, // rmin, rmax
                                           3.495 * m,                        // half-length
                                           0.0, twopi),
                                _stainless, "MLCS_tube_log", 0, 0, 0);
        G4VPhysicalVolume *phys_MLCS_tube =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., 0.),
                              logi_MLCS_tube,   // logical vol
                              "MLCS_tube_phys", // name
                              logi_MLCS_scinti, // mother logical vol
                              false,            // no boolean ops
                              0);               // copy number
        new G4LogicalBorderSurface("MLCS_tube_logsurf", phys_MLCS_scinti, phys_MLCS_tube,
                                   BlackSheet_opsurf);

        // acrylic endcaps
        G4LogicalVolume *logi_MLCS_endcap =
            new G4LogicalVolume(new G4Tubs("MLCS_endcap", 0.0, 69. * mm, // rmin, rmax
                                           40. * mm,                     // half-length
                                           0.0, twopi),
                                _acrylic, "MLCS_endcap_log", 0, 0, 0);
        // fake PMTs in endcaps
        G4LogicalVolume *logi_MLCS_PMTglass =
            new G4LogicalVolume(new G4Tubs("MLCS_PMTglass", 0.0, 63.5 * mm, // rmin, rmax
                                           5. * mm,                         // half-length
                                           0.0, twopi),
                                _glass, "MLCS_PMTglass_log", 0, 0, 0);
        G4VPhysicalVolume *phys_MLCS_PMTglass =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., 0.),
                              logi_MLCS_PMTglass,   // logical vol
                              "MLCS_PMTglass_phys", // name
                              logi_MLCS_endcap,     // mother logical vol
                              false,                // no boolean ops
                              0);                   // copy number
        G4LogicalVolume *logi_MLCS_PMTvac =
            new G4LogicalVolume(new G4Tubs("MLCS_PMTvac", 0.0, 62.5 * mm, // rmin, rmax
                                           4. * mm,                       // half-length
                                           0.0, twopi),
                                PMT_Vac, "MLCS_PMTvac_log", 0, 0, 0);
        G4VPhysicalVolume *phys_MLCS_PMTvac =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., 0.),
                              logi_MLCS_PMTvac,   // logical vol
                              "MLCS_PMTvac_phys", // name
                              logi_MLCS_PMTglass, // mother logical vol
                              false,              // no boolean ops
                              0);                 // copy number
        G4LogicalVolume *logi_MLCS_PMTdynode =
            new G4LogicalVolume(new G4Tubs("MLCS_PMTdynode", 0.0, 62.5 * mm, // rmin, rmax
                                           0.5 * mm,                         // half-length
                                           0.0, twopi),
                                _stainless, "MLCS_PMTdynode_log", 0, 0, 0);
        G4VPhysicalVolume *phys_MLCS_PMTdynode =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., -2. * mm),
                              logi_MLCS_PMTdynode,   // logical vol
                              "MLCS_PMTdynode_phys", // name
                              logi_MLCS_PMTvac,      // mother logical vol
                              false,                 // no boolean ops
                              0);                    // copy number
        // photocathode surfaces...
        CupPMTSD *pmtSD;
        pmtSD = new CupPMTSD("/cupdet/pmt/MLCS", 2);
        G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);
        logi_MLCS_PMTglass->SetSensitiveDetector(pmtSD);
        logi_MLCS_PMTvac->SetSensitiveDetector(pmtSD);
        new G4LogicalBorderSurface("MLCS_photocathode_logsurf",
                                   phys_MLCS_PMTglass, // exiting glass into vac.
                                   phys_MLCS_PMTvac, Photocathode_opsurf);
        // EJ
        G4Region *PmtRegion = new G4Region("MLCS");
        PmtRegion->AddRootLogicalVolume(logi_MLCS_PMTglass);
        CupPMTOpticalModel *pmtOpticalModel =
            new CupPMTOpticalModel("MLCS_optical_model", phys_MLCS_PMTglass);

        G4RotationMatrix *mlcs_endcap1_rot = new G4RotationMatrix();
        mlcs_endcap1_rot->rotateY(M_PI); // this PMT points down
        G4VPhysicalVolume *phys_MLCS_endcap1 =
            new G4PVPlacement(mlcs_endcap1_rot, // no rotation
                              G4ThreeVector(0., 0., 3.415 * m + 40. * mm),
                              logi_MLCS_endcap,    // logical vol
                              "MLCS_endcap1_phys", // name
                              logi_MLCS_scinti,    // mother logical vol
                              false,               // no boolean ops
                              0);                  // copy number
        G4VPhysicalVolume *phys_MLCS_endcap2 =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., -3.415 * m - 40. * mm),
                              logi_MLCS_endcap,    // logical vol
                              "MLCS_endcap2_phys", // name
                              logi_MLCS_scinti,    // mother logical vol
                              false,               // no boolean ops
                              1);                  // copy number

        // internal discs
        G4LogicalVolume *logi_MLCS_mask =
            new G4LogicalVolume(new G4Tubs("MLCS_mask", 55. * mm, 69. * mm, // rmin, rmax
                                           0.5 * mm,                        // half-length
                                           0.0, twopi),
                                _stainless, "MLCS_mask_log", 0, 0, 0);
        for (int imask = -8; imask <= 8; imask++) {
            G4VPhysicalVolume *phys_MLCS_mask =
                new G4PVPlacement(0, // no rotation
                                  G4ThreeVector(0., 0., imask * 400. * mm),
                                  logi_MLCS_mask, // logical vol
                                  G4String("MLCS_mask_phys_") + G4String('A' + imask + 8), // name
                                  logi_MLCS_scinti, // mother logical vol
                                  false,            // no boolean ops
                                  0);               // copy number
            new G4LogicalBorderSurface(G4String("MLCS_mask_logsurf_") + G4String('A' + imask + 8),
                                       phys_MLCS_scinti, phys_MLCS_mask, BlackSheet_opsurf);
        }
    }

    ////////////////////////////////////////////////////////////////
    // A (3_m)^3 cube of iron, for physics process validation
    //
    {
        G4double box_size              = 3000. * millimeter / 2.0;
        G4Material *_iron              = new G4Material("Iron", // its name
                                           26.0,   // atomic number
                                           55.845 * gram / mole,
                                           //"mass of mole" according to .hh file and docs,
                                           // really mass of one nucleus, in grams!
                                           7.87 * gram / cm3, // density
                                           kStateSolid);      // solid,liqid,gas
        G4LogicalVolume *logi_iron_box = new G4LogicalVolume(
            new G4Box("iron_box", box_size, box_size, box_size), _iron, "iron_box_log", 0, 0, 0);
        G4VPhysicalVolume *phys_iron_box = new G4PVPlacement(0, // no rotation
                                                             G4ThreeVector(5.0 * m, 0., 0.),
                                                             logi_iron_box,   // logical vol
                                                             "iron_box_phys", // name
                                                             logiHall,        // mother logical vol
                                                             false,           // no boolean ops
                                                             0);              // copy number

        CupBoxSD *boxSD;
        boxSD = new CupBoxSD("/cupdet/box/Fe_cube");
        boxSD->SetZ0(box_size);
        boxSD->SetRadLength(_iron->GetRadlen());
        G4SDManager::GetSDMpointer()->AddNewDetector(boxSD);
        logi_iron_box->SetSensitiveDetector(boxSD);
    }

    ////////////////////////////////////////////////////////////////
    // a white cone, used for 5" PMT and waveshifter tests
    //
    {
        G4double box_size                = 2000. * millimeter / 2.0;
        G4LogicalVolume *logi_schmoo_box = new G4LogicalVolume(
            new G4Box("schmoo_box", box_size, box_size, box_size), _air, "schmoo_box_log", 0, 0, 0);
        logi_schmoo_box->SetVisAttributes(G4VisAttributes::Invisible);
        G4VPhysicalVolume *phys_schmoo_box = new G4PVPlacement(0, // no rotation
                                                               G4ThreeVector(5.0 * m, -5.0 * m, 0.),
                                                               logi_schmoo_box,   // logical vol
                                                               "schmoo_box_phys", // name
                                                               logiHall, // mother logical vol
                                                               false,    // no boolean ops
                                                               0);       // copy number

        G4double dz_cone = 342.9 * mm, rbot_cone = 558.8 * mm, rtop_cone = 127. * mm,
                 thk_cone = 3.0 * mm;
        G4double z_PMT5 = dz_cone + thk_cone, r_PMT5 = 64. * mm;
        G4double thk_scinti = 10.0 * mm;

        G4LogicalVolume *logiSchmooCone = new G4LogicalVolume(
            new G4Cons("SchmooCone_solid", rbot_cone - thk_cone, rbot_cone, rtop_cone - thk_cone,
                       rtop_cone, dz_cone, 0.0, 2.0 * M_PI),
            _tyvek, "SchmooCone_log");
        G4VPhysicalVolume *physSchmooCone = new G4PVPlacement(0, // no rotation
                                                              G4ThreeVector(0., 0., 0.),
                                                              "SchmooCone_phys", // name
                                                              logiSchmooCone,    // logical vol
                                                              phys_schmoo_box, // mother logical vol
                                                              false,           // no boolean ops
                                                              0);              // copy number
        new G4LogicalBorderSurface("SchmooCone_logsurf1", phys_schmoo_box, physSchmooCone,
                                   Tyvek_opsurf);
        new G4LogicalBorderSurface("SchmooCone_logsurf2", physSchmooCone, phys_schmoo_box,
                                   Tyvek_opsurf);

        G4LogicalVolume *logiSchmooBase = new G4LogicalVolume(
            new G4Tubs("SchmooBase_solid", 0.0, rbot_cone, thk_cone / 2.0, 0.0, twopi), _tyvek,
            "SchmooBase_log", 0, 0, 0);
        G4VPhysicalVolume *physSchmooBase =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., -(dz_cone + thk_cone / 2.0)),
                              "SchmooBase_phys", // name
                              logiSchmooBase,    // logical vol
                              phys_schmoo_box,   // mother logical vol
                              false,             // no boolean ops
                              0);                // copy number
        new G4LogicalBorderSurface("SchmooBase_logsurf1", phys_schmoo_box, physSchmooBase,
                                   Tyvek_opsurf);
        new G4LogicalBorderSurface("SchmooBase_logsurf2", physSchmooBase, phys_schmoo_box,
                                   Tyvek_opsurf);

        G4LogicalVolume *logiSchmooScint = new G4LogicalVolume(
            new G4Tubs("SchmooScint_solid", 0.0,
                       rbot_cone - thk_cone -
                           thk_scinti * (rbot_cone - rtop_cone) / (2.0 * dz_cone),
                       thk_scinti / 2.0, 0.0, twopi),
            Scintillator, "SchmooScint_log", 0, 0, 0);
        G4VisAttributes *_visScinti = new G4VisAttributes(G4Colour(0.6, 0.3, 1.0, 0.2));
        logiSchmooScint->SetVisAttributes(_visScinti);
        G4VPhysicalVolume *physSchmooScint =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., -(dz_cone - thk_scinti / 2.0)),
                              "SchmooScint_phys", // name
                              logiSchmooScint,    // logical vol
                              phys_schmoo_box,    // mother logical vol
                              false,              // no boolean ops
                              0);                 // copy number
        new G4LogicalBorderSurface("SchmooScint_logsurf", physSchmooScint, physSchmooBase,
                                   Tyvek_opsurf);

        CupPMTSD *pmtSD = new CupPMTSD("/cupdet/pmt/schmoo", 1);
        G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);

        G4LogicalVolume *logiSchmooPMT =
            new Cup_5inch_LogicalVolume("SchmooPMT", _air, _glass, Photocathode_opsurf, PMT_Vac,
                                        _stainless, // dynode material
                                        NULL,       // no mask
                                        pmtSD,      // sensitive detector hook
                                        whichPmtStyle);

        G4RotationMatrix *my_rot = new G4RotationMatrix();
        my_rot->rotateY(M_PI); // PMTs point down
        new G4PVPlacement(/* rotation = */ my_rot,
                          /* position = */ G4ThreeVector(0.0, 0.0, z_PMT5), "SchmooPMT_phys",
                          logiSchmooPMT,
                          phys_schmoo_box, // parent
                          /* many = */ false, 0);

        G4LogicalVolume *logiSchmooCap = new G4LogicalVolume(
            new G4Tubs("SchmooCap_solid", r_PMT5, rtop_cone, thk_cone / 2.0, 0.0, twopi),
            _stainless, "SchmooCap_log", 0, 0, 0);
        G4VisAttributes *_visStainless = new G4VisAttributes(G4Colour(0.6, 0.6, 0.7, 0.4));
        logiSchmooCap->SetVisAttributes(_visStainless);
        G4VPhysicalVolume *physSchmooCap =
            new G4PVPlacement(0, // no rotation
                              G4ThreeVector(0., 0., +(dz_cone + thk_cone / 2.0)),
                              "SchmooCap_phys", // name
                              logiSchmooCap,    // logical vol
                              phys_schmoo_box,  // mother logical vol
                              false,            // no boolean ops
                              0);               // copy number
        new G4LogicalBorderSurface("SchmooCap_logsurf1", phys_schmoo_box, physSchmooCap,
                                   Stainless_opsurf);
        new G4LogicalBorderSurface("SchmooCap_logsurf2", physSchmooCap, phys_schmoo_box,
                                   Stainless_opsurf);
    }
}
