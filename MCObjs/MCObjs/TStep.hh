#ifndef TSTEP_H
#define TSTEP_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TStep class                                                          //
//                                                                      //
// Description of the step parameters                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TString.h"

class TDirectory;

class TStep : public TObject {

  private:
    Int_t PDGcode;
    Int_t TrackID;
    Int_t ParentID;
    Int_t StepNo;
    Float_t x, y, z;
    Float_t KineticEnergy;
    Float_t EnergyDeposit;
    Double_t Globaltime;
    Double_t Localtime;
    TString ParticleName;
    TString ProcessName;
    TString VolumeName;

  public:
    TStep();
    TStep(const TStep &orig);
    virtual ~TStep() { Clear(); }
    TStep &operator=(const TStep &orig);

    void Clear(Option_t *option = "");
    const char *GetParticleName() const { return ParticleName; }
    Int_t GetPDGcode() const { return PDGcode; }
    Int_t GetTrackID() const { return TrackID; }
    Int_t GetParentID() const { return ParentID; }
    Int_t GetStepNo() const { return StepNo; }
    Float_t GetX() const { return x; }
    Float_t GetY() const { return y; }
    Float_t GetZ() const { return z; }
    Float_t GetKineticEnergy() const { return KineticEnergy; }
    Float_t GetEnergyDeposit() const { return EnergyDeposit; }
    Double_t GetGlobalTime() const { return Globaltime; }
    Double_t GetLocalTime() const { return Localtime; }
    const char *GetProcessName() const { return ProcessName; }
    const char *GetVolumeName() const { return VolumeName; }

    void SetParticleName(const char *name) { ParticleName = name; }
    void SetPDGcode(int code) { PDGcode = code; }
    void SetTrackID(int id) { TrackID = id; }
    void SetParentID(int id) { ParentID = id; }
    void SetStepNo(int no) { StepNo = no; }
    void SetX(Float_t xx) { x = xx; }
    void SetY(Float_t yy) { y = yy; }
    void SetZ(Float_t zz) { z = zz; }
    void SetKineticEnergy(Float_t eng) { KineticEnergy = eng; }
    void SetEnergyDeposit(Float_t eng) { EnergyDeposit = eng; }
    void SetGlobalTime(Double_t time) { Globaltime = time; }
    void SetLocalTime(Double_t time) { Localtime = time; }
    void SetProcessName(const char *name) { ProcessName = name; }
    void SetVolumeName(const char *name) { VolumeName = name; }

    ClassDef(TStep, 10) // A track segment
};

#endif
