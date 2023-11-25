#include "MCObjs/THit.hh"

ClassImp(THit);

//______________________________________________________________________________
THit::THit()
    : TObject(), Time(-1), PMTno(-1), Wavelength(-1), x(-1), y(-1), z(-1), px(-1), py(-1), pz(-1),
      polx(-1), poly(-1), polz(-1), Count(-1), ProcessTag(-1) {}

//______________________________________________________________________________
THit::THit(const THit &hit)
    : TObject(hit), Time(hit.Time), PMTno(hit.PMTno), Wavelength(hit.Wavelength), x(hit.x),
      y(hit.y), z(hit.z), px(hit.px), py(hit.py), pz(hit.pz), polx(hit.polx), poly(hit.poly),
      polz(hit.polz), Count(hit.Count), ProcessTag(hit.ProcessTag) {}
// Copy a track object

//______________________________________________________________________________
THit &THit::operator=(const THit &hit) {
    // Copy a track

    TObject::operator=(hit);
    Time             = hit.GetHitTime();
    PMTno            = hit.GetHitPMT();
    Wavelength       = hit.GetWaveLength();
    x                = hit.GetX();
    y                = hit.GetY();
    z                = hit.GetZ();
    px               = hit.GetPX();
    py               = hit.GetPY();
    pz               = hit.GetPZ();
    polx             = hit.GetPolX();
    poly             = hit.GetPolY();
    polz             = hit.GetPolZ();
    Count            = hit.GetHitCount();
    ProcessTag       = hit.GetProcessTag();

    return *this;
}

//______________________________________________________________________________
void THit::Clear(Option_t * /*option*/) { TObject::Clear(); }

