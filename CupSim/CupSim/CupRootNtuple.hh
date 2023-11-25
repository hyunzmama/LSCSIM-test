// CupRootNtuple.hh

#ifndef CupRootNtuple_h
#define CupRootNtuple_h 1

// The CupRecorderBase object.  We're implementing this abstract class
// with methods that fill histograms.
#include "TBranch.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTree.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "TClonesArray.h"

#include "CupSim/CupRecorderBase.hh"
#include "CupSim/CupVEventAction.hh"

#include "MCObjs/EvtInfo.hh"
#include "MCObjs/EvtStep.hh"
#include "MCObjs/EvtTrack.hh"
#include "MCObjs/MLCS.hh"
#include "MCObjs/MuonSD.hh"
#include "MCObjs/PMTSD.hh"
#include "MCObjs/Photon.hh"
#include "MCObjs/Primary.hh"
#include "MCObjs/Scint.hh"
#include "MCObjs/TGSD.hh"

// Forward declarations for ROOT.
// class TFile;
// class TNtuple;

// Forward declarations of G4 classes that are arguments to
// CupRecorderBase methods.
class G4Run;
class G4Event;
class G4Track;
class G4Step;

// class CupRootNtuple: public CupRecorderBase
class CupRootNtuple : public CupRecorderBase, public CupVEventAction {
  private:
    class CupRootNtupleMessenger *myMessenger;

  protected:
    typedef enum {
        kDetector_TestBench,
        kNumGenericDetectors
    } eDetector; // should be matched to definitions in CupDetectorCondtruction.hh

    TFile *fROOTOutputFile;
    TTree *fROOTOutputTree;
    TTree *fROOTRunTree;
    TStopwatch *timer;

    G4SDManager *sdman;

    G4int TGSDHCID;
    G4int nsrc;

    Primary *Cprimary;
    TClonesArray *tclvrtx;
    EvtTrack *Ctrack;
    TClonesArray *tcltr;
    EvtStep *Cstep;
    TClonesArray *tclst;
    Photon *Cphoton;
    TClonesArray *tclhit;
    TGSD *Ctgsd;
    TClonesArray *tclcell;
    MuonSD *CMuSD;
    TClonesArray *tclmuon;
    PMTSD *pmtsd_inner;
    PMTSD *pmtsd_outer;
    PMTSD *pmtsd_plasScint;
    MLCS *Cmlcs;
    Scint *Cscint;
    EvtInfo *Cevtinfo;

    int StatusPrimary;
    int StatusTrack;
    int StatusStep;
    int StatusPhoton;
    int StatusScint;
    int StatusMuon;

    Int_t eventID;
    Int_t runID;
    Float_t UT;
    Float_t delta_UT;
    Int_t eventType;

    Char_t volumeName[100];
    Int_t copyNo;

    Int_t nTrack;
    Int_t nStep;

    // target sensitive detector
    Double_t tgcellEdep[1000];
    Double_t tgcellEdepQuenched[1000];

  public:
    CupRootNtuple();
    ~CupRootNtuple();

    // For this example, we're opening and closing our HBOOK file at the
    // start and end of a run, respectively.  We're filling histograms
    // in the UserEndOfEvent and UserSteppingAction classes.
    void RecordBeginOfRun(const G4Run *);
    void RecordEndOfRun(const G4Run *);
    void RecordBeginOfEvent(const G4Event *);
    void RecordEndOfEvent(const G4Event *);
    virtual void RecordTrack(const G4Track *);
    virtual void RecordStep(const G4Step *);

    virtual void OpenFile(const G4String filename, G4bool outputMode);
    virtual void CloseFile();
    void Clear() { ; }

    void SetPrimary(const G4Step *);

    void SetEventInfo(const G4Event *a_event);
    void SetPrimary(const G4Event *a_event);
    void SetPhoton();
    void SetScintillation();
    virtual void SetTGSD(const G4Event *a_event);
    virtual void SetMuonSD(const G4Event *a_event);
    void ClearEvent();

    virtual void SetNewValue(G4UIcommand *command, G4String newValue);

    virtual void CreateTree();
    void AddEventInfo();
    void AddPMTSD();

    int GetNtuplePrimaryStatus(void) { return StatusPrimary; }
    virtual void SetNtuplePrimary(int val) { StatusPrimary = val; }
    int GetNtupleTrackStatus(void) { return StatusTrack; }
    virtual void SetNtupleTrack(int val) { StatusTrack = val; }
    int GetNtupleStepStatus(void) { return StatusStep; }
    virtual void SetNtupleStep(int val) { StatusStep = val; }
    int GetNtuplePhotonStatus(void) { return StatusPhoton; }
    virtual void SetNtuplePhoton(int val) { StatusPhoton = val; }
    int GetNtupleScintStatus(void) { return StatusScint; }
    virtual void SetNtupleScint(int val) { StatusScint = val; }
    int GetNtupleMuonStatus(void) { return StatusMuon; }
    virtual void SetNtupleMuon(int val) { StatusMuon = val; }

    enum {
        max_primary_particles   = 16,
        max_hits_for_ROOT       = 200000,
        max_gammas              = 10000,
        max_opticalphotons      = 10000,
        max_protons             = 1000,
        max_nTrk                = 1000000,
        max_secondary_particles = 1000
    };
    enum { nLayer_vessel = 4, inout_muon = 2 };
    enum {
        max_muonSecondary           = 10000,
        max_neutronCapture          = 100,
        max_neutronCaptureSecondary = 100,
        max_correlBKG               = 1000000
    };
};
#endif
