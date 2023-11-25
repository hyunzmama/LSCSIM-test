
#include "CupSim/CupHitPhoton.hh"
#include <CLHEP/Units/PhysicalConstants.h>
//#include <CLHEP/Units/GlobalPhysicalConstants.h> //EJ

/// set kinetic energy and wavelength of photon.
void CupHitPhoton::SetKineticEnergy(double KE) { fKE = KE; }

/// set wavelength and kinetic energy of photon
void CupHitPhoton::SetWavelength(double wl) { fKE = 2 * CLHEP::pi * CLHEP::hbarc / wl; }

void CupHitPhoton::SetPosition(double x, double y, double z) {
    fPosition[0] = x;
    fPosition[1] = y;
    fPosition[2] = z;
}

void CupHitPhoton::SetMomentum(double x, double y, double z) {
    fMomentum[0] = x;
    fMomentum[1] = y;
    fMomentum[2] = z;
}

void CupHitPhoton::SetPolarization(double x, double y, double z) {
    fPolarization[0] = x;
    fPolarization[1] = y;
    fPolarization[2] = z;
}

double CupHitPhoton::GetKineticEnergy() const { return fKE; }

double CupHitPhoton::GetWavelength() const { return 2 * CLHEP::pi * CLHEP::hbarc / fKE; }
