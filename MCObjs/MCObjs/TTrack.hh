#ifndef TTRACK_H
#define TTRACK_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TTrack class                                                         //
//                                                                      //
// Description of the track parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TString.h"

// class TDirectory;

class TTrack : public TObject {

  private:
    Int_t AtomicNumber;
    Int_t AtomicMass;
    Int_t PDGcode;
    Int_t TrackID;
    Int_t ParentID;
    Float_t x, y, z;
    Double_t KineticEnergy;
    Double_t Globaltime;
    Double_t Localtime;
    TString ParticleName;
    TString ProcessName;
    TString VolumeName;

  public:
    TTrack();
    TTrack(const TTrack &orig);
    virtual ~TTrack() { Clear(); }
    TTrack &operator=(const TTrack &orig);

    void Clear(Option_t *option = "");
    const char *GetParticleName() const { return ParticleName; }
    Int_t GetAtomicNumber() const { return AtomicNumber; }
    Int_t GetAtomicMass() const { return AtomicMass; }
    Int_t GetPDGcode() const { return PDGcode; }
    Int_t GetTrackID() const { return TrackID; }
    Int_t GetParentID() const { return ParentID; }
    Float_t GetX() const { return x; }
    Float_t GetY() const { return y; }
    Float_t GetZ() const { return z; }
    Float_t GetKineticEnergy() const { return KineticEnergy; }
    Double_t GetGlobalTime() const { return Globaltime; }
    Double_t GetLocalTime() const { return Localtime; }
    const char *GetProcessName() const { return ProcessName; }
    const char *GetVolumeName() const { return VolumeName; }

    void SetParticleName(const char *name) { ParticleName = name; }
    void SetAtomicNumber(int anum) { AtomicNumber = anum; }
    void SetAtomicMass(int amass) { AtomicMass = amass; }
    void SetPDGcode(int code) { PDGcode = code; }
    void SetTrackID(int id) { TrackID = id; }
    void SetParentID(int id) { ParentID = id; }
    void SetX(Float_t xx) { x = xx; }
    void SetY(Float_t yy) { y = yy; }
    void SetZ(Float_t zz) { z = zz; }
    void SetKineticEnergy(Float_t eng) { KineticEnergy = eng; }
    void SetGlobalTime(Double_t time) { Globaltime = time; }
    void SetLocalTime(Double_t time) { Localtime = time; }
    void SetProcessName(const char *name) { ProcessName = name; }
    void SetVolumeName(const char *name) { VolumeName = name; }

    ClassDef(TTrack, 10) // A track segment
};

#endif
