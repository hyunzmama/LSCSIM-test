
#include <sstream>
#include <string>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"

#include "CupSim/CupDebugMessenger.hh"
#include "CupSim/CupDetectorConstruction.hh"
#include "CupSim/CupPMTSD.hh"
#include "CupSim/CupParam.hh"
#include "CupSim/CupPrimaryGeneratorAction.hh"
#include "CupSim/CupRootNtuple.hh"
#include "CupSim/CupRootNtupleMessenger.hh"
#include "CupSim/CupScintHit.hh"
#include "CupSim/CupScintSD.hh"
#include "CupSim/CupScintillation.hh"
#include "CupSim/CupSteppingAction.hh"
#include "CupSim/CupTrackingAction.hh"
#include "CupSim/CupVEventAction.hh"
#include "CupSim/CupVertexGen.hh"
#include "CupSim/CupVetoSD.hh"

// Include files for ROOT.
#include "Rtypes.h"
#include "TBranch.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTree.h"

// Include files for the G4 classes
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Trajectory.hh"
#include "G4VHitsCollection.hh"
#include "G4VProcess.hh"
#include "G4VSolid.hh"
#include "G4ios.hh"
#include "globals.hh"

TROOT theROOT("CupSim/Cupsim", "Cup Geant4 simulation output tree");

CupRootNtuple::CupRootNtuple()
    : CupRecorderBase(), myMessenger(nullptr), fROOTOutputFile(nullptr), fROOTOutputTree(nullptr) {
    fFileCmd->SetGuidance("This will be a ROOT Format File;");
    timer         = new TStopwatch();
    myMessenger   = new CupRootNtupleMessenger(this);
    StatusPrimary = 0;
    StatusTrack   = 0;
    StatusStep    = 0;
    StatusPhoton  = 0;
    StatusScint   = 0;
    StatusMuon    = 0;
    nsrc          = 0;
}

CupRootNtuple::~CupRootNtuple() {
    CloseFile();
    delete Cprimary;
    delete Ctrack;
    delete Cstep;
    delete Cphoton;
    delete Ctgsd;
    delete CMuSD;
    delete Cscint;
    delete timer;

    delete myMessenger;
}

void CupRootNtuple::SetNewValue(G4UIcommand *command, G4String newValue) {
    if (command->GetCommandName() == "drawTracks")
        drawFlag = newValue;
    else if (command->GetCommandName() == "output_mode") {
        G4bool old_flagFullOutputMode = flagFullOutputMode;
        flagFullOutputMode            = (newValue == "full");
        if (flagFullOutputMode != old_flagFullOutputMode) {
            G4cout << "Switching output mode to " << (flagFullOutputMode ? "full" : "basic")
                   << G4endl;
        } else {
            G4cout << "FYI: new output mode == old output mode" << G4endl;
        }
    } else if (command->GetCommandName() == "output_file") {
        // always call CloseFile() here, in case file is open
        // (user code should ignore CloseFile() if file is not open)
        CloseFile();
        // open new file, if new filename given
        // (it is okay not to give a name, in case user just wants to close file)
        if (newValue.length() <= 0) {
            // G4cerr << "Null Output File Name" << G4endl;
            return;
        } else
            OpenFile(newValue, flagFullOutputMode);

    }
    //  else if (command->GetCommandName() == "doParameterizedScintillation")
    //    {
    //      fgDoParameterizedScintillation= (atoi((const char*)newValue)!=0);
    //    }
    else {
        G4cerr << "Unknown command ``" << command->GetCommandName()
               << " passed to CupEventAction::SetNewValue\n";
    }
}

void CupRootNtuple::OpenFile(const G4String filename, G4bool outputMode) {
    if (fROOTOutputFile != nullptr) CloseFile();

    flagFullOutputMode = outputMode;
    fROOTOutputFile =
        new TFile((filename + ".root").c_str(), "RECREATE", "Cup Geant4 simulation output file");
    if (fROOTOutputFile == nullptr) {
        G4cerr << "Could not open ROOT output file " << filename << G4endl;
        return;
    }
    CreateTree();
}

void CupRootNtuple::CloseFile() {
    if (fROOTOutputFile != nullptr) {
        fROOTOutputFile = fROOTOutputTree->GetCurrentFile();
        G4cout << "Closing " << fROOTOutputFile->GetName() << G4endl;
        fROOTOutputFile->Write();
        fROOTOutputFile->Close(); // this seems to delete the TTree, too.
        delete fROOTOutputFile;
        fROOTOutputFile = nullptr;
        fROOTOutputTree = nullptr; // because TTree already deleted (ROOT v 2.51 and 3.00)
        fROOTRunTree    = nullptr;
    }
}

void CupRootNtuple::CreateTree() {
    // Authorize Trees up to 2Terabytes(if the system can do it)
    //  TTree::SetMaxTreeSize(1000*Long64_t(2000000000));
    TTree::SetMaxTreeSize(2147483647);

    Cprimary = new Primary();
    tclvrtx  = Cprimary->GetVertex();
    Ctrack   = new EvtTrack();
    tcltr    = Ctrack->GetTrack();
    Cstep    = new EvtStep();
    tclst    = Cstep->GetStep();
    Cphoton  = new Photon();
    tclhit   = Cphoton->GetHit();
    Ctgsd    = new TGSD();
    tclcell  = Ctgsd->GetCell();
    CMuSD    = new MuonSD();
    tclmuon  = CMuSD->GetCell();
    Cscint   = new Scint();

    fROOTRunTree = new TTree("run_tree", "CupSim/Cupsim run data tree");
    fROOTRunTree->Branch("runID", &runID, "runID/I");

    fROOTOutputTree = new TTree("event_tree", "CupSim/Cupsim event tree");

    AddEventInfo();
    if (StatusPrimary) fROOTOutputTree->Branch("PRIMARY", &(Cprimary), 256000, 2);
    if (StatusTrack) fROOTOutputTree->Branch("TRACK", &(Ctrack), 256000, 2);
    if (StatusStep) fROOTOutputTree->Branch("STEP", &(Cstep), 256000, 2);
    if (StatusPhoton) fROOTOutputTree->Branch("PHOTONHIT", &(Cphoton), 256000, 2);
    if (StatusScint) fROOTOutputTree->Branch("SCINT", &(Cscint), 256000, 2);

    sdman = G4SDManager::GetSDMpointer();
    if (StatusMuon) {
        CupVetoSD *muonSD;
        muonSD = dynamic_cast<CupVetoSD *>(sdman->FindSensitiveDetector("/cupdet/MuonVetoSD"));
        if (muonSD) {
            fROOTOutputTree->Branch("MuonSD", &(CMuSD), 256000, 0);
        }
    }

    // TG Sensitive Detector
    CupScintSD* tgsd;
      tgsd= (CupScintSD*)(sdman->FindSensitiveDetector("/cupdet/TGSD"));
      if (tgsd) {
      fROOTOutputTree->Branch("TGSD", &(Ctgsd), 256000, 2);
      }

    AddPMTSD();
}

void CupRootNtuple::AddEventInfo() {
    Cevtinfo = new EvtInfo();
#if (ROOT_VERSION_CODE >= ROOT_VERSION(3, 01, 1))
    fROOTOutputTree->BranchOld("event_timer", "TStopwatch", &timer);
#else
    fROOTOutputTree->Branch("event_timer", "TStopwatch", &timer);
#endif
    fROOTOutputTree->Branch("EVENTINFO", &Cevtinfo, 256000, 2);
}

void CupRootNtuple::AddPMTSD() {
    // for each PMT sensitive detector, add n_pmt, etc.
    //  G4SDManager* sdman= G4SDManager::GetSDMpointer();
    // Cpmtsd = new PMTSD();
    CupPMTSD *pmtsd;
    pmtsd = (CupPMTSD *)(sdman->FindSensitiveDetector("/cupdet/pmt/inner"));
    if (pmtsd) {
        pmtsd_inner = new PMTSD();
        fROOTOutputTree->Branch("PMT_Inner", &(pmtsd_inner), 256000, 2);
    }
    pmtsd = (CupPMTSD *)(sdman->FindSensitiveDetector("/cupdet/pmt/outer"));
    if (pmtsd) {
        pmtsd_outer = new PMTSD();
        fROOTOutputTree->Branch("PMT_Outer", &(pmtsd_outer), 256000, 2);
    }
    pmtsd = (CupPMTSD *)(sdman->FindSensitiveDetector("/cupdet/pmt/plasScint"));
    if (pmtsd) {
        pmtsd_plasScint = new PMTSD();
        fROOTOutputTree->Branch("PMT_PlasScint", &(pmtsd_plasScint), 256000, 2);
    }
    pmtsd = (CupPMTSD *)(sdman->FindSensitiveDetector("/cupdet/pmt/MLCS"));
    if (pmtsd) {
        Cmlcs = new MLCS();
        fROOTOutputTree->Branch("PMT_MLCS", &(Cmlcs), 256000, 2);
    }
}

void CupRootNtuple::RecordBeginOfRun(const G4Run *a_run) {}

void CupRootNtuple::RecordEndOfRun(const G4Run *a_run) { G4cout << "NSource= " << nsrc << G4endl; }

void CupRootNtuple::RecordBeginOfEvent(const G4Event *a_event) {}

void CupRootNtuple::RecordEndOfEvent(const G4Event *a_event) {

    SetEventInfo(a_event);
    if (StatusPrimary) {
        SetPrimary(a_event);
    }
    if (StatusPhoton) {
        SetPhoton();
    }
    if (StatusScint) {
        SetScintillation();
    }

    SetTGSD(a_event);

    if (StatusTrack) {
        Ctrack->SetNtrack(nTrack);
    }
    if (StatusStep) {
        Cstep->SetNstep(nStep);
    }

    if (StatusMuon) {
        SetMuonSD(a_event);
    }

    // put to tree
    fROOTOutputTree->Fill();

    ClearEvent();

    G4cout << "///////////////////////////////// End of Event "
              "/////////////////////////////////////////"
           << G4endl;
}

void CupRootNtuple::RecordTrack(const G4Track *a_track) {
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // EJ: G4TrackStatus
    //     fAlive=0: Continue the tracking
    //     fStopButAlive=1: Invoke active rest physics processes and kill the current track
    //     afterward fStopAndKill=2: Kill the current track fKillTrackAndSecondaries=3: Kill the
    //     current track and also associated secondaries fSuspend=4: Suspend the current track
    //     fPostponeToNextEvent=5: Postpones the tracking of thecurrent track to the next event
    /////////////////////////////////////////////////////////////////////////////////////////////////

    Int_t trid          = a_track->GetTrackID();
    Int_t prntid        = a_track->GetParentID();
    G4String volume     = a_track->GetVolume()->GetName();
    G4String pname      = a_track->GetDefinition()->GetParticleName();
    G4int atomicmass    = a_track->GetDefinition()->GetAtomicMass();
    G4int atomicnumber  = a_track->GetDefinition()->GetAtomicNumber();
    Double_t ke         = a_track->GetKineticEnergy();
    Double_t globaltime = a_track->GetGlobalTime();
    Double_t localtime  = a_track->GetLocalTime();

    G4ThreeVector pos = a_track->GetPosition();
    Float_t xx        = (float)pos.x();
    Float_t yy        = (float)pos.y();
    Float_t zz        = (float)pos.z();

    G4String procname;
    const G4VProcess *creatorProcess = a_track->GetCreatorProcess();
    if (creatorProcess) procname = creatorProcess->GetProcessName();

    TTrack ttr;
    Int_t cuppdgcode;

    char partname[100];
    char volname[100];
    char prcsname[100];

    sprintf(partname, "%s", (char *)pname.data());
    sprintf(volname, "%s", (char *)volume.data());
    sprintf(prcsname, "%s", (char *)procname.data());

    G4ParticleDefinition *pdef = a_track->GetDefinition();
    if (G4IonTable::IsIon(pdef)) {
        cuppdgcode = (1000 * atomicnumber + atomicmass) * 10;
    } else {
        cuppdgcode = pdef->GetPDGEncoding();
    }

    if (StatusTrack) {
        ttr.SetParticleName(partname);
        ttr.SetAtomicNumber(atomicnumber);
        ttr.SetAtomicMass(atomicmass);
        ttr.SetPDGcode(cuppdgcode);
        ttr.SetTrackID(trid);
        ttr.SetParentID(prntid);
        ttr.SetKineticEnergy(ke);
        ttr.SetX(xx);
        ttr.SetY(yy);
        ttr.SetZ(zz);
        ttr.SetGlobalTime(globaltime);
        ttr.SetLocalTime(localtime);
        ttr.SetProcessName(prcsname);
        ttr.SetVolumeName(volname);

        new ((*tcltr)[nTrack]) TTrack(ttr);
        nTrack++;

        // if ( trid==1 && prntid==0 )  G4cout << "Track Info.: pname " << partname << ", track ID=
        // " << trid << ", parent ID= " << prntid << G4endl;
    }
}

void CupRootNtuple::RecordStep(const G4Step *a_step) {
    Int_t trid         = a_step->GetTrack()->GetTrackID();
    Int_t prntid       = a_step->GetTrack()->GetParentID();
    G4String volume    = a_step->GetTrack()->GetVolume()->GetName();
    G4int istep        = a_step->GetTrack()->GetCurrentStepNumber();
    G4String pname     = a_step->GetTrack()->GetDefinition()->GetParticleName();
    G4int atomicmass   = a_step->GetTrack()->GetDefinition()->GetAtomicMass();
    G4int atomicnumber = a_step->GetTrack()->GetDefinition()->GetAtomicNumber();
    Double_t dep       = a_step->GetTotalEnergyDeposit();

    G4StepPoint *preStep  = a_step->GetPreStepPoint();
    G4StepPoint *postStep = a_step->GetPostStepPoint();

    G4String procname   = postStep->GetProcessDefinedStep()->GetProcessName();
    Double_t ke         = postStep->GetKineticEnergy();
    Double_t globaltime = postStep->GetGlobalTime();
    Double_t localtime  = postStep->GetLocalTime();
    G4ThreeVector pos   = postStep->GetPosition();
    Float_t xx          = (float)pos.x();
    Float_t yy          = (float)pos.y();
    Float_t zz          = (float)pos.z();

    G4TouchableHandle theTouchable = preStep->GetTouchableHandle();
    G4int motherCopyNo             = 0;

    if (theTouchable->GetHistoryDepth() >= 1) {
        motherCopyNo = theTouchable->GetCopyNumber(1);
    }

    //	TTrack ttr;
    TStep tst;
    Int_t cuppdgcode;

    char partname[100];
    char prcsname[100];
    char volname[100];

    sprintf(partname, "%s", (char *)pname.data());
    sprintf(volname, "%s", (char *)volume.data());
    sprintf(prcsname, "%s", (char *)procname.data());

    G4ParticleDefinition *pdef = a_step->GetTrack()->GetDefinition();
    if (G4IonTable::IsIon(pdef)) {
        cuppdgcode = (1000 * atomicnumber + atomicmass) * 10;
    } else {
        cuppdgcode = pdef->GetPDGEncoding();
    }

    if (StatusStep) {
        tst.SetParticleName(partname);
        tst.SetPDGcode(cuppdgcode);
        tst.SetTrackID(trid);
        tst.SetParentID(prntid);
        tst.SetKineticEnergy(ke);
        tst.SetEnergyDeposit(dep);
        tst.SetX(xx);
        tst.SetY(yy);
        tst.SetZ(zz);
        tst.SetGlobalTime(globaltime);
        tst.SetLocalTime(localtime);
        tst.SetProcessName(prcsname);
        tst.SetVolumeName(volname);
        tst.SetStepNo(istep);

        new ((*tclst)[nStep]) TStep(tst);
        nStep++;
    }
    // Access physical volume where primary particles generated
    if (istep == 1 && prntid == 0 && trid == 1) {
        sprintf(volumeName, "%s", (char *)volume.data());
        copyNo = motherCopyNo;
        G4String motherVolName;
    }
}

void CupRootNtuple::SetEventInfo(const G4Event *a_event) {
    // fill event ID and run ID
    eventID = a_event->GetEventID();
    runID   = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
    G4cout << "Event= " << eventID << G4endl;

    if (fROOTOutputTree == 0) {
        G4cout << "Event " << eventID << " of run " << runID << " finished, no output file open!"
               << G4endl;
        G4cout.flush();
        return;
    }
    Cevtinfo->SetEventID(eventID);
    Cevtinfo->SetRunID(runID);

    // on first event, set run data, checking for existence of each branch
    if (eventID == 0) {
        CupParam &db(CupParam::GetDB());
        CupParam::iterator i;
        for (i = db.begin(); i != db.end(); i++) {
            G4String key((*i).first);
            char branchName[80];
            sprintf(branchName, "database.%s", key.c_str());
            TBranch *b = fROOTRunTree->GetBranch(branchName);
            if (b == NULL) {
                double dummy = 0.0;
                char leafFormat[80];
                sprintf(leafFormat, "%s/D", key.c_str());
                b                = fROOTRunTree->Branch(branchName, &dummy, leafFormat);
                int nTreeEntries = (int)(fROOTRunTree->GetEntries());
                if (nTreeEntries > 0) {
                    G4cerr << "Note: new database parameter detected on run " << runID << " with "
                           << nTreeEntries << " run tree entries already filled." << G4endl;
                    // need to fill in dummy entries to keep all branches in sync.
                    for (int ii = 0; ii < nTreeEntries; ii++)
                        b->Fill();
                }
            }
            b->SetAddress(&((*i).second));
        }
        fROOTRunTree->Fill();
    }

    // fill simulated universal time info
    CupPrimaryGeneratorAction *theCupPGA =
        CupPrimaryGeneratorAction::GetTheCupPrimaryGeneratorAction();
    UT       = theCupPGA->GetUniversalTime();
    delta_UT = theCupPGA->GetUniversalTimeSincePriorEvent();

    Cevtinfo->SetUT(UT);
    Cevtinfo->SetDeltaUT(delta_UT);
    G4cout << "GLOBAL TIME Info.: UT= " << UT << ", delta_UT= " << delta_UT << G4endl;

    // fill primary generator action "event type" info
    eventType = theCupPGA->GetTypeOfCurrentEvent();
    if (eventType == 3) nsrc++;
    Cevtinfo->SetEventType(eventType);
    Cevtinfo->SetNSource(nsrc);
}

void CupRootNtuple::SetPrimary(const G4Event *a_event) {
    Double_t ke;
    Vertex vrtx;
    THit phit;

    int iprim = 0;
    int nvrtx;
    G4ThreeVector vcentroid(0, 0, 0);
    double vketot = 0.0;

    nvrtx = a_event->GetNumberOfPrimaryVertex();
    for (int ivert = 0; ivert < nvrtx; ivert++) {
        G4PrimaryVertex *pv = a_event->GetPrimaryVertex(ivert);
        for (int i = 0; i < pv->GetNumberOfParticle() && iprim < max_primary_particles;
             i++, iprim++) {
            G4PrimaryParticle *p = pv->GetPrimary(i);
            G4int cuppdgcode     = p->GetPDGcode();
            if (cuppdgcode == 0 && p->GetG4code() != 0) {
                G4ParticleDefinition *pdef = p->GetG4code();
                if (G4IonTable::IsIon(pdef)) {
                    int atomicNumber = G4int(pdef->GetPDGCharge() / eplus);
                    int atomicMass   = pdef->GetBaryonNumber();
                    cuppdgcode =
                        CupVertexGen_HEPEvt::kIonCodeOffset + 1000 * atomicNumber + atomicMass;
                }
            }
            ke = sqrt(p->GetMass() * p->GetMass() + p->GetMomentum().mag2()) - p->GetMass();
            G4String parname = p->GetG4code()->GetParticleName();
            char tName[100];
            sprintf(tName, "%s", (char *)parname.data());

            vrtx.SetT0(pv->GetT0());
            vrtx.SetX0(pv->GetX0());
            vrtx.SetY0(pv->GetY0());
            vrtx.SetZ0(pv->GetZ0());
            vrtx.SetPDGcode(cuppdgcode);
            vrtx.SetPX(p->GetPx());
            vrtx.SetPY(p->GetPy());
            vrtx.SetPZ(p->GetPz());
            vrtx.SetKE(ke);
            vrtx.SetPolX(p->GetPolX());
            vrtx.SetPolY(p->GetPolY());
            vrtx.SetPolZ(p->GetPolZ());
            vrtx.SetParticleName(tName);
            vrtx.SetVolumeName(volumeName);
            vrtx.SetCopyNo(copyNo);

            new ((*tclvrtx)[iprim]) Vertex(vrtx);

            if (abs(cuppdgcode) < CupVertexGen_HEPEvt::kPDGcodeModulus) {
                vketot += ke;
                vcentroid += ke * pv->GetPosition();
            }
            G4cout << "EJ: iprim= " << iprim << ", pname= " << tName << ", pdgcode= " << cuppdgcode
                   << G4endl;
        }
    }
    //  vertex_info.n_particles= iprim;
    vcentroid *= 1.0 / vketot;
    Cprimary->SetNvertex(iprim);
    Cprimary->SetKEtot(vketot);
    Cprimary->SetCentroidX(vcentroid.x());
    Cprimary->SetCentroidY(vcentroid.y());
    Cprimary->SetCentroidZ(vcentroid.z());
}

void CupRootNtuple::SetPhoton() {
    THit phit;
    Int_t n_photon_hits    = 0;
    Int_t n_photoelectrons = 0;
    Int_t n_op_cerenkov    = 0;
    Int_t n_op_scint       = 0;
    Int_t n_op_reem        = 0;
    Float_t xx, yy, zz;
    Int_t tag;
    const int ndetPmt = 480; // from pmtcoordinates_ID.dat
    Int_t nhitInner = 0, nhitOuter = 0;
    Int_t nhitPmtInner = 0, nhitPmtOuter = 0;
    Int_t prePmtInnerId = -1, prePmtOuterId = -1;
    Int_t nhitPmt = theHitPMTCollection.GetEntries();
    G4cout << "nhitPMT= " << theHitPMTCollection.GetEntries() << G4endl;
    for (int ipmt = 0; ipmt < theHitPMTCollection.GetEntries(); ipmt++) {
        CupHitPMT *a_pmt = theHitPMTCollection.GetPMT(ipmt);
        a_pmt->SortTimeAscending(); // Sort the photons in time order
        // G4cout << "ipmt= " << ipmt << ", nPMThit= " << a_pmt->GetEntries() << G4endl;
        for (int i = 0; i < a_pmt->GetEntries(); i++) {
            const CupHitPhoton *photon = a_pmt->GetPhoton(i);
            phit.SetHitPMT(photon->GetPMTID());
            phit.SetHitTime(photon->GetTime());
            phit.SetHitCount(photon->GetCount());
            tag = photon->GetProcessTag();
            phit.SetProcessTag(tag);
            G4cout << "iphoton= " << i << ", pmt id= " << photon->GetPMTID()
                   << ", process tag= " << tag << G4endl;
            if (photon->GetPMTID() < ndetPmt) {
                if (prePmtInnerId != photon->GetPMTID()) nhitPmtInner++;
                prePmtInnerId = photon->GetPMTID();
                nhitInner++;
            } else {
                if (prePmtOuterId != photon->GetPMTID()) nhitPmtOuter++;
                prePmtOuterId = photon->GetPMTID();
                nhitOuter++;
            }
            if (tag == 1) n_op_cerenkov++;
            if (tag == 2) n_op_scint++;
            if (tag == 3) n_op_reem++;
            if (flagFullOutputMode) {
                phit.SetWaveLength(photon->GetWavelength());
                photon->GetPosition(xx, yy, zz);
                phit.SetX(xx);
                phit.SetY(yy);
                phit.SetZ(zz);
                photon->GetMomentum(xx, yy, zz);
                phit.SetPX(xx);
                phit.SetPY(yy);
                phit.SetPZ(zz);
                photon->GetPolarization(xx, yy, zz);
                phit.SetPolX(xx);
                phit.SetPolY(yy);
                phit.SetPolZ(zz);
            }
            new ((*tclhit)[n_photon_hits]) THit(phit);
            n_photon_hits++;

            n_photoelectrons += photon->GetCount(); // for the number of photoelectrons

            if (n_photon_hits >= max_hits_for_ROOT) break;
        }
    }
    if (pmtsd_inner) {
	G4cout << "nhitPMTInner= " << nhitPmtInner << G4endl;
        pmtsd_inner->SetNhitPmts(nhitPmtInner);
        pmtsd_inner->SetNhits(nhitInner);
    }
    if (pmtsd_outer) {
        pmtsd_outer->SetNhitPmts(nhitPmtOuter);
        pmtsd_outer->SetNhits(nhitOuter);
    }
    if (Cmlcs) {
        Cmlcs->SetNhitPmts(nhitPmt);
        Cmlcs->SetNhits(n_photon_hits);
    }
    Cphoton->SetNhitPmts(nhitPmt);
    Cphoton->SetNhits(n_photon_hits);
    Cphoton->SetNcerenkov(n_op_cerenkov);
    Cphoton->SetNscint(n_op_scint);
    Cphoton->SetNreem(n_op_reem);
    G4cout << "n_photon_hits= " << n_photon_hits << ", n_photoelectrons= " << n_photoelectrons
           << G4endl;
    G4cout << "n_op_cerenkov= " << n_op_cerenkov << ", n_op_scint= " << n_op_scint
           << ", n_op_reem= " << n_op_reem << G4endl;
}

void CupRootNtuple::SetScintillation() {
    Double_t totScintEdep;
    Double_t totScintEdepQuenched;
    Int_t totScintPhotons;
    totScintEdep         = CupScintillation::GetTotEdep();
    totScintEdepQuenched = CupScintillation::GetTotEdepQuenched();
    totScintPhotons      = CupScintillation::GetNScintPhotons();
    Cscint->SetTotScintEdep(totScintEdep);
    Cscint->SetTotScintEdepQuenched(totScintEdepQuenched);
    Cscint->SetTotScintPhotons(totScintPhotons);
    //  G4ThreeVector scint_centroid( CupScintillation::GetScintCentroid() );
    //  Cscint->SetCentroidX(scint_centroid.x());
    //  Cscint->SetCentroidY(scint_centroid.y());
    //  Cscint->SetCentroidZ(scint_centroid.z());
    G4cout << "totScintEdep= " << totScintEdep << ", totScintEdepQuenched= " << totScintEdepQuenched
           << ", totScintPhotons= " << totScintPhotons << G4endl;
}

void CupRootNtuple::SetMuonSD(const G4Event *a_event) {
    TCell tcell;
    G4int MuSDHCID = -1;
    G4double eDep = 0, eDepQ = 0;
    MuSDHCID             = sdman->GetCollectionID("VetoSDColl");
    G4HCofThisEvent *HCE = a_event->GetHCofThisEvent();
    if (MuSDHCID >= 0 && HCE) {
        CupVetoHitsCollection *MuHC = dynamic_cast<CupVetoHitsCollection *>(HCE->GetHC(MuSDHCID));
        if (MuHC) {
            G4int ent        = MuHC->entries();
            G4double totedep = 0, totedepq = 0;
            CMuSD->SetNTotCell(ent);
            for (int i = 0; i < ent; i++) {
                CupVetoHit *aHit = (*MuHC)[i];
                eDep             = aHit->GetEdep();
                totedep += eDep;
                eDepQ = aHit->GetEdepQuenched();
                totedepq += eDepQ;
                tcell.SetEdep(eDep / MeV);
                tcell.SetEdepQuenched(eDepQ / MeV);
                tcell.SetCellID(i);
                new ((*tclmuon)[i]) TCell(tcell);
            }
            CMuSD->SetTotEdep(totedep);
            CMuSD->SetTotEdepQuenched(totedepq);
        } else
            G4cout << "Muon Veto Hit Collection was not configured properly!!" << G4endl;
    } else
        G4cout << "Muon Veto Hit Collection was not configured properly!!" << G4endl;
}

void CupRootNtuple::SetTGSD(const G4Event *a_event) {
    //////////////////////////
    // TG Sensitive Detector
    /////////////////////////
    G4String colName;
    G4SDManager *SDman           = G4SDManager::GetSDMpointer();
    TGSDHCID                     = SDman->GetCollectionID(colName = "TGSD/TGSDColl");
    G4HCofThisEvent *HCE         = a_event->GetHCofThisEvent();
    CupScintHitsCollection *ECHC = 0;
    if (HCE) ECHC = (CupScintHitsCollection *)(HCE->GetHC(TGSDHCID));

    int iHit                 = 0;
    double totalE            = 0.;
    double totalEquenched    = 0.;
    int nTotCell             = 0;
    double tgTotEdep         = 0.;
    double tgTotEdepQuenched = 0.;
    G4String logVolName;
    int idxDetID = 0;
    char detName[300];

    int nCell = 0;
    TCell tcell;

    nTotCell = ECHC->entries();
    Ctgsd->SetNTotCell(nTotCell);
    double eDep;
    double eDepQuenched;

    Int_t DetectorType;                                          // EJ
    DetectorType = CupDetectorConstruction::GetDetectorType();   // EJ
    G4cout << "Ntuple DetectorType= " << DetectorType << G4endl; // EJ
    switch (DetectorType) {
        case kDetector_TestBench: // for TestBench
            G4cout << "###  TestBench  " << G4endl;
            for (int i = 0; i < nTotCell; i++) {
                tgcellEdep[i]         = 0.;
                tgcellEdepQuenched[i] = 0.;
                CupScintHit *aHit      = (*ECHC)[i];
                eDep                   = aHit->GetEdep();
                eDepQuenched           = aHit->GetEdepQuenched();

                tcell.SetCellID(idxDetID);
                tcell.SetEdep(eDep);
                tcell.SetEdepQuenched(eDepQuenched);
                if (eDep > 0.) {
                    iHit++;
                    totalE += eDep;
                    totalEquenched += eDepQuenched;
                }

                new ((*tclcell)[nCell]) TCell(tcell);
                nCell++;
            }
            break;
        default:
            G4cout << "### Detector type is not valid!!! Signal is not filled to Cell!!!\n";
            break;
    }
    (void)tgTotEdep;
    (void)tgTotEdepQuenched;
    Ctgsd->SetTotEdep(totalE);
    Ctgsd->SetTotEdepQuenched(totalEquenched);
    //  Ctgsd->SetNHitCell(iHit);
}

void CupRootNtuple::ClearEvent() {
    Cprimary->Clear();
    Ctrack->Clear();
    Cstep->Clear();
    Cphoton->Clear();
    Ctgsd->Clear();
    if (pmtsd_inner) pmtsd_inner->Clear();
    if (pmtsd_outer) pmtsd_outer->Clear();
    if (pmtsd_plasScint) pmtsd_plasScint->Clear();
    if (Cmlcs) Cmlcs->Clear();
    Cscint->Clear();

    nTrack        = 0;
    nStep         = 0;
    volumeName[0] = '\0';
    copyNo        = 99999;
}
