#include "MCObjs/TTrack.hh"

ClassImp(TTrack);

//______________________________________________________________________________
TTrack::TTrack()
    : TObject(), AtomicNumber(-1), AtomicMass(-1), PDGcode(-1), TrackID(-1), ParentID(-1), x(-1),
      y(-1), z(-1), KineticEnergy(-1), Globaltime(-1), Localtime(-1), ParticleName(), ProcessName(),
      VolumeName() {}

//______________________________________________________________________________
TTrack::TTrack(const TTrack &trk)
    : TObject(trk), AtomicNumber(trk.AtomicNumber), AtomicMass(trk.AtomicMass),
      PDGcode(trk.PDGcode), TrackID(trk.TrackID), ParentID(trk.ParentID), x(trk.x), y(trk.y),
      z(trk.z), KineticEnergy(trk.KineticEnergy), Globaltime(trk.Globaltime),
      Localtime(trk.Localtime), ParticleName(trk.ParticleName), ProcessName(trk.ProcessName),
      VolumeName(trk.VolumeName) {}
// Copy a track object

//______________________________________________________________________________
TTrack &TTrack::operator=(const TTrack &trk) {
    // Copy a track

    TObject::operator=(trk);
    ParticleName     = trk.GetParticleName();
    AtomicNumber     = trk.GetAtomicNumber();
    AtomicMass       = trk.GetAtomicMass();
    PDGcode          = trk.GetPDGcode();
    TrackID          = trk.GetTrackID();
    ParentID         = trk.GetParentID();
    x                = trk.GetX();
    y                = trk.GetY();
    z                = trk.GetZ();
    KineticEnergy    = trk.GetKineticEnergy();
    Globaltime       = trk.GetGlobalTime();
    Localtime        = trk.GetLocalTime();
    ProcessName      = trk.GetProcessName();
    VolumeName       = trk.GetVolumeName();

    return *this;
}

//______________________________________________________________________________
void TTrack::Clear(Option_t * /*option*/) { TObject::Clear(); }

