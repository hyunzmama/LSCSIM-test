#include "MCObjs/TStep.hh"

ClassImp(TStep);

//______________________________________________________________________________
TStep::TStep()
    : TObject(), PDGcode(-1), TrackID(-1), ParentID(-1), StepNo(-1), x(-1), y(-1), z(-1),
      KineticEnergy(-1), EnergyDeposit(-1), Globaltime(-1), Localtime(-1), ParticleName(),
      ProcessName(), VolumeName() {}

//______________________________________________________________________________
TStep::TStep(const TStep &stp)
    : TObject(stp), PDGcode(stp.PDGcode), TrackID(stp.TrackID), ParentID(stp.ParentID),
      StepNo(stp.StepNo), x(stp.x), y(stp.y), z(stp.z), KineticEnergy(stp.KineticEnergy),
      EnergyDeposit(stp.EnergyDeposit), Globaltime(stp.Globaltime), Localtime(stp.Localtime),
      ParticleName(stp.ParticleName), ProcessName(stp.ProcessName), VolumeName(stp.VolumeName) {
} // Copy a track object

//______________________________________________________________________________
TStep &TStep::operator=(const TStep &stp) {
    // Copy a track

    TObject::operator=(stp);
    ParticleName     = stp.GetParticleName();
    PDGcode          = stp.GetPDGcode();
    TrackID          = stp.GetTrackID();
    ParentID         = stp.GetParentID();
    StepNo           = stp.GetStepNo();
    x                = stp.GetX();
    y                = stp.GetY();
    z                = stp.GetZ();
    KineticEnergy    = stp.GetKineticEnergy();
    EnergyDeposit    = stp.GetEnergyDeposit();
    Globaltime       = stp.GetGlobalTime();
    Localtime        = stp.GetLocalTime();
    ProcessName      = stp.GetProcessName();
    VolumeName       = stp.GetVolumeName();

    return *this;
}

//______________________________________________________________________________
void TStep::Clear(Option_t * /*option*/) {
    ParticleName.Clear();
    VolumeName.Clear();
    ProcessName.Clear();
    TObject::Clear();
}
