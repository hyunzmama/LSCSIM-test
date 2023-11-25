
#include "CupSim/CupTorusStack.hh"

#include "G4AffineTransform.hh"
#include "G4VoxelLimits.hh"

#include "G4VPVParameterisation.hh"

#include "meshdefs.hh"

#include "G4Polyhedron.hh"
#include "G4VGraphicsScene.hh"
#include "G4VisExtent.hh"

#include "G4ios.hh" // for G4cerr

#include "G4GeometryTolerance.hh"
#include "Randomize.hh"

// debugging
#ifdef DEBUG_CupTorusStack
#undef DEBUG_CupTorusStack
#define DEBUG_CupTorusStack(A) A
#else
#define DEBUG_CupTorusStack(A)
#endif

// a helpful function I use a lot:
static inline double square(double x) { return x * x; }
// #if defined(__GNUC__) && (__GNUC__ < 3)
// [rayd] renamed hypot as cup_hypot, to avoid problems with gcc 2.96
static inline double cup_hypot(double x, double y) { return sqrt(x * x + y * y); }
// #endif

// another helpful function
static inline bool samesign(double x, double y)
//{ return (x>0.0) ? (y>0.0) : !(y>0.0); }
{
    return (x * y > 0.0);
}

static inline bool TestTolerance(double a, double b) {
    double kRadTolerance = G4GeometryTolerance::GetInstance()->GetRadialTolerance();
    return a <= b + kRadTolerance && a >= b - kRadTolerance;
}

// Constructor
CupTorusStack::CupTorusStack(const G4String &pName) : G4CSGSolid(pName) {
    n     = 0;
    inner = 0;
}

void CupTorusStack::SetAllParameters(G4int n_,                 // number of Z-segments
                                     const G4double z_edge_[], // n+1 edges of Z-segments
                                     const double rho_edge_[], // n+1 dist. from Z-axis
                                     const G4double z_o_[],    // z-origins (n total)
                                     CupTorusStack *inner_)    // inner TorusStack to subtract
{
    using namespace std;
    int i;
    int n_increasing;

    // delete old arrays if necessary (also used by destructor)
    if (n != 0) {
        delete[] z_edge;
        delete[] rho_edge;
        delete[] z_o;
        delete[] a;
        delete[] b;
        delete[] area_ratio_segment;
        delete[] z_dist_segment;
        delete[] phi_start_segment;
        delete[] phi_end_segment;
        n = 0;
    }
    if (n_ <= 0) return;

    // allocate new memory for arrays
    n                  = n_;
    z_edge             = new G4double[n + 1];
    rho_edge           = new G4double[n + 1];
    z_o                = new G4double[n];
    a                  = new G4double[n];
    b                  = new G4double[n];
    area_ratio_segment = new G4double[n + 2];
    z_dist_segment     = new G4double[n + 2];
    phi_start_segment  = new G4double[n + 2];
    phi_end_segment    = new G4double[n + 2];

    // copy arrays, and check for monotonicity
    n_increasing = 0;
    if (z_edge_[n] > z_edge_[0]) {
        for (i = 0; i < n; i++) {
            z_edge[i]   = z_edge_[i];
            rho_edge[i] = rho_edge_[i];
            z_o[i]      = z_o_[i];
            if (z_edge_[i + 1] >= z_edge_[i]) n_increasing++;
        }
        z_edge[n]   = z_edge_[n];
        rho_edge[n] = rho_edge_[n];
    } else {
        for (i = 0; i < n; i++) {
            z_edge[i]   = z_edge_[n - i];
            rho_edge[i] = rho_edge_[n - i];
            z_o[i]      = z_o_[n - 1 - i];
            if (z_edge_[i + 1] <= z_edge_[i]) n_increasing++;
        }
        z_edge[n]   = z_edge_[0];
        rho_edge[n] = rho_edge_[0];
    }

    // z_edge must be monotonically increasing
    if (n_increasing != n) {
        // G4Exception("Error in CupTorusStack::SetAllParameters - "
        G4Exception(" ", " ", JustWarning,
                    "Error in CupTorusStack::SetAllParameters - "
                    "z_edge[] must be ordered!");
    }

    // set a[] and b[].  Also check z_o for validity and set max_rho
    max_rho = rho_edge[n];
    for (i = 0; i < n; i++) {
        if (rho_edge[i] > max_rho) max_rho = rho_edge[i];
        if ((z_o[i] > z_edge[i]) && (z_o[i] < z_edge[i + 1])) {
            // G4Exception("Error in CupTorusStack::SetAllParameters - "
            G4Exception(" ", " ", JustWarning,
                        "Error in CupTorusStack::SetAllParameters - "
                        "z_edge[i] < z_o[i] < z_edge[i+1] NOT ALLOWED!"
                        " (Please subdivide your TorusStack)");
        }
        if (rho_edge[i] == rho_edge[i + 1]) { // cylinder -- special case!
            a[i] = rho_edge[i];
            b[i] = 0.0;
        } else {
            double zc, dz, zminus, zplus; // used for calculating allowed range of z0
            zc     = (z_edge[i + 1] + z_edge[i]) / 2.0;
            dz     = square(rho_edge[i + 1] - rho_edge[i]) / (z_edge[i + 1] - z_edge[i]) / 2.0;
            zminus = zc - dz;
            zplus  = zc + dz;
            // consistent z_o must be above zplus or below zminus
            // (otherwise, solution for intercepting toric surface has multiple
            // rho values for a given z)
            double kRadTolerance = G4GeometryTolerance::GetInstance()->GetRadialTolerance();
            if (z_o[i] > zminus + kRadTolerance && z_o[i] < zplus - kRadTolerance) {
                // we have trouble.  Fix it by adjusting z_o to nearest of two values.
                double new_z_o = (z_o[i] > zc) ? zplus : zminus;
                G4cerr << "CupSim/CupTorusStack::SetAllParameters: (" << GetName()
                       << ") adjusting z_o[" << i << "] from " << z_o[i] << " to " << new_z_o
                       << ", delta is " << new_z_o - z_o[i] << G4endl;
                z_o[i] = new_z_o;
            }
            a[i] = (((z_edge[i + 1] - z_edge[i]) * (z_edge[i + 1] + z_edge[i] - 2 * z_o[i]) /
                     (rho_edge[i + 1] - rho_edge[i])) +
                    (rho_edge[i + 1] + rho_edge[i])) /
                   2.0;
            b[i] = sqrt(square(z_edge[i] - z_o[i]) + square(rho_edge[i] - a[i]));
            if (rho_edge[i] > a[i] - kRadTolerance && rho_edge[i + 1] > a[i] - kRadTolerance) {
                // all is well, sign of b is correct
                ;
            } else if (rho_edge[i] < a[i] + kRadTolerance &&
                       rho_edge[i + 1] < a[i] + kRadTolerance) {
                // all is well, but need to flip sign of b -- negative curvature here
                b[i] = -b[i];
            } else {
                // all is not well -- despite our best efforts, "a" has ended up
                // somewhere between the two rho values.  THis can't happen..
                // G4Exception("Error in CupTorusStack::SetAllParameters - "
                G4Exception(" ", " ", JustWarning,
                            "Error in CupTorusStack::SetAllParameters - "
                            "can't figure out curvature -- math or logic failure?");
            }
        }
    }

    // set myRadTolerance
    double kRadTolerance = G4GeometryTolerance::GetInstance()->GetRadialTolerance();
    double kAngTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance();
    myRadTolerance       = max(kRadTolerance, kAngTolerance * max_rho);

    // set Inner solid
    inner = inner_;

    using namespace CLHEP;
    double normalize_const = 0;
    normalize_const += (area_ratio_segment[0] = pi * rho_edge[0] * rho_edge[0]);
    normalize_const += (area_ratio_segment[n + 1] = pi * rho_edge[n] * rho_edge[n]);
    z_dist_segment[0]     = z_edge[0];
    z_dist_segment[n + 1] = z_edge[n];
    phi_start_segment[0] = phi_end_segment[0] = 0.;
    phi_start_segment[n + 1] = phi_end_segment[n + 1] = 0.;

    for (i = 0; i < n; i++) {
        double now_a   = a[i];
        double now_b   = b[i];
        double delta_z = z_edge[i + 1] - z_edge[i];
        if (now_b == 0) { // For cylinder
            area_ratio_segment[i + 1] = 2. * pi * now_a * delta_z;
            z_dist_segment[i + 1]     = z_edge[i];
            phi_start_segment[i + 1] = phi_end_segment[i + 1] = 0.;
        } else { // Otherwise
            double start_x = rho_edge[i] - now_a;
            double end_x   = rho_edge[i + 1] - now_a;
            double test_end_x, delta_phi;
            phi_start_segment[i + 1] = asin(start_x / now_b);
            phi_end_segment[i + 1]   = acos(cos(phi_start_segment[i + 1]) + delta_z / now_b);
            test_end_x               = now_b * sin(phi_end_segment[i + 1]);
            if (!TestTolerance(test_end_x, end_x)) {
                phi_start_segment[i + 1] =
                    (now_b > 0) ? pi - asin(start_x / now_b) : -pi + asin(start_x / now_b);
                phi_end_segment[i + 1] = acos(cos(phi_start_segment[i + 1]) + delta_z / now_b);
                test_end_x             = now_b * sin(phi_end_segment[i + 1]);
                if (!TestTolerance(test_end_x, end_x)) {
                    G4Exception("CupTorusStack::SetAllParameters", "ParamErr", JustWarning,
                                "phi_end_segment is weird -- math or logic failure?");
                }
            }
            z_dist_segment[i + 1] =
                z_edge[i] + (((now_b > 0) ? (-1) : (+1)) * now_b * cos(phi_start_segment[i + 1]));
            delta_phi = (now_b > 0) ? phi_start_segment[i + 1] - phi_end_segment[i + 1]
                                    : phi_end_segment[i + 1] - phi_start_segment[i + 1];
            if (abs(now_a) < abs(now_b)) {
                double delta_cos_phi = cos(phi_end_segment[i + 1]) - cos(phi_start_segment[i + 1]);
                double first_term    = 2. * pi * abs(now_b) * abs(now_b) * delta_cos_phi;
                double second_term   = 2. * pi * abs(now_b) * abs(now_a) * delta_phi;
                area_ratio_segment[i + 1] = first_term + second_term;
            } else
                area_ratio_segment[i + 1] = 2. * pi * abs(now_a * now_b * delta_phi);
        }
        normalize_const += area_ratio_segment[i + 1];
    }
    for (i = 0; i < n + 2; i++)
        area_ratio_segment[i] /= normalize_const;
}

G4ThreeVector CupTorusStack::GetPointOnSurface() const {
    using namespace CLHEP;
    int selectedIndex = 0;

    double r, nowRandom = G4UniformRand();
    while (selectedIndex < n + 2 && nowRandom > area_ratio_segment[selectedIndex])
        nowRandom -= area_ratio_segment[selectedIndex++];

    G4ThreeVector retValue(0, 0, z_dist_segment[selectedIndex]);
    G4ThreeVector transPhi1;
    double rotPhi1 = G4UniformRand() * 2. * pi;

    if (selectedIndex == 0 || selectedIndex == n + 2) {
        r         = rho_edge[0];
        transPhi1 = G4ThreeVector(G4UniformRand() * r, 0, 0);
        transPhi1.rotateZ(rotPhi1);
        retValue += transPhi1;
    } else if (selectedIndex == n + 1) {
        r         = rho_edge[n];
        transPhi1 = G4ThreeVector(G4UniformRand() * r, 0, 0);
        transPhi1.rotateZ(rotPhi1);
        retValue += transPhi1;
    } else {
        G4ThreeVector transPhi2;
        double delta_z = z_edge[selectedIndex] - z_edge[selectedIndex - 1];
        transPhi1      = G4ThreeVector(a[selectedIndex - 1], 0,
                                  ((b[selectedIndex - 1] == 0) ? G4UniformRand() * delta_z : 0.));
        transPhi2      = G4ThreeVector(0, 0, b[selectedIndex - 1]);

        double rotPhi2 =
            (phi_start_segment[selectedIndex] > 0)
                ? phi_end_segment[selectedIndex] +
                      G4UniformRand() *
                          (phi_start_segment[selectedIndex] - phi_end_segment[selectedIndex])
                : phi_end_segment[selectedIndex] -
                      G4UniformRand() *
                          (phi_end_segment[selectedIndex] - phi_start_segment[selectedIndex]);

        transPhi1.rotateZ(rotPhi1);
        transPhi2.rotateY(rotPhi2);
        transPhi2.rotateZ(rotPhi1);

        retValue += transPhi1;
        retValue += transPhi2;
    }
    return retValue;
}

// Destructor
CupTorusStack::~CupTorusStack() { SetAllParameters(0, NULL, NULL, NULL); }

// --------------------------------------------------------------------------

// Dispatch to parameterisation for replication mechanism dimension
// computation & modification.

void CupTorusStack::ComputeDimensions(G4VPVParameterisation *, const G4int,
                                      const G4VPhysicalVolume *) {
    G4cerr << "Warning: ComputeDimensions is not defined for CupTorusStack. "
              "It shouldn't be called.\n";
    // but note that G4Polycone just silently ignores calls to ComputeDimensions!
    // ComputeDimensions seems to be unimplemented in all classes -- just
    // dispatches to methods that ultimately turn out to be no-ops!
    // Never called by anything to do any real work in Geant4.0.1 ? ! ?
}

// -------------------------------------------------------------------------
// Function for finding first intersection of a ray (starting from p along
// v) with a torus section, given by rho(z)= a+b*sqrt(1-(z/b)**2).
// returns 1 if intersection found in given interval, 0 otherwise

// the function a TorusFunc to return distance to root
class CupTorusStack_TorusFunc : public CupTorusStack::RootFinder {
  public:
    G4double rr, ru, uu, z, w;
    G4double a, b;
    void f_and_Df(G4double s, G4double &f, G4double &Df) {
        // note: f > 0 <==> outside; f < 0 <==> inside
        G4double rho = sqrt(rr + (2 * ru + uu * s) * s);
        if ((b > 0.0) ? (rho >= a) : (rho <= a)) {
            f  = (square(rho - a) + square(z + w * s)) / b - b;
            Df = ((1.0 - a / rho) * (ru + uu * s) + w * (z + w * s)) / b;
        } else {
            f  = (square(z + w * s) - square(rho - a)) / b - b;
            Df = (w * (z + w * s) - (1.0 - a / rho) * (ru + uu * s)) / b;
        }
    }
};

G4int CupTorusStack::FindFirstTorusRoot(
    G4double a,             // swept radius
    G4double b,             // radius of torus section
    const G4ThreeVector &p, // start point relative to torus centroid
    const G4ThreeVector &v, // direction vector
    G4double smin,          // lower bracket on root
    G4double smax,          // upper bracket on root
    G4bool fEntering,       // true if looking for out->in crossing
    G4double &sout)         // distance to root, if found
{
    CupTorusStack_TorusFunc tfunc;

    tfunc.rr = square(p.x()) + square(p.y());
    tfunc.ru = p.x() * v.x() + p.y() * v.y();
    tfunc.uu = square(v.x()) + square(v.y());
    tfunc.z  = p.z();
    tfunc.w  = v.z();
    tfunc.a  = a;
    tfunc.b  = b;

    if (b == 0.0) { // special case: cylinder
        if (tfunc.uu == 0.0 && tfunc.ru == 0.0)
            return 0;
        else {
            // solve quadratic Q = A*t**2 + B*t + C = 0
            //  Q > 0 <==> outside; Q < 0 <==> inside
            G4double A = tfunc.uu;
            G4double B = 2.0 * tfunc.ru;
            G4double C = (tfunc.rr - square(a));
            if (A == 0.0)
                sout = -C / B; // tangent -- only one root
            else {
                C = B * B - 4 * A * C;
                if (C < 0.0) // no intersection
                    return 0;
                else { // two roots
                    C = sqrt(C);
                    B /= 2 * A;
                    C /= 2 * A;
                    if (fEntering)
                        sout = -B - C; // want first root if looking for entrance point
                    else
                        sout = -B + C; // want second root if looking for exit point
                }
            }
            return (sout >= smin && sout <= smax) ? 1 : 0;
        }
    }
    if (a == 0.0) { // special case: sphere
        // solve quadratic Q = t**2 + B*t + C = 0
        // Q > 0 <==> outside; Q < 0 <==> inside
        G4double B = 2.0 * (tfunc.ru + tfunc.z * tfunc.w);
        G4double C = (tfunc.rr + square(tfunc.z) - square(b));
        C          = B * B - 4 * C;
        if (C < 0.0) // no intersection
            return 0;
        else { // two roots
            C = sqrt(C);
            B /= 2.;
            C /= 2.;
            if (fEntering)
                sout = -B - C; // want first root if looking for entrance
            else
                sout = -B + C; // want second root if looking for exit
        }
        return (sout >= smin && sout <= smax) ? 1 : 0;
    }

    double kRadTolerance = G4GeometryTolerance::GetInstance()->GetRadialTolerance();
    return tfunc.FindRoot(smin, smax, 0.25 * kRadTolerance, fEntering, sout);
}

// ---------------------------------------------------------------------------

// RootFinder::FindRoot finds the lowest root in a given range using
// Newton's method and bisection
int CupTorusStack::RootFinder::FindRoot(G4double smin, G4double smax, G4double tol,
                                        G4bool fFindFallingRoot, G4double &sout) {
    G4double f1, Df1, f2, Df2, s, f, Df, oldds;
    bool ok_to_retry = true;

    // initial function calls
    f_and_Df(smin, f1, Df1);
    f_and_Df(smax, f2, Df2);

    // adjust endpoints so it looks like a valid bracket:
    //  want f1 < 0, Df1 > 0 if fFindFallingRoot == false
    //  want f1 > 0, Df1 < 0 if fFindFallingRoot == true
    {
        G4double sign = fFindFallingRoot ? -1.0 : 1.0;
        oldds         = (smax - smin) / 64.0;
        while (!(sign * Df1 > 0.0 && sign * f1 < 0.0)) {
            smin += oldds;
            if (smin >= smax) return 0;
            f_and_Df(smin, f1, Df1);
        }
    }

    // root-finding loop
retry: // will retry at most once
    s     = smin;
    f     = f1;
    Df    = Df1;
    oldds = fabs(smax - smin);
    while (oldds > tol) {
        G4double ds   = -f / Df;
        G4double stry = s + ds;
        if ((G4double)stry == (G4double)s) {
            oldds = 0.0; // done if correction is too small for machine precision
            break;
        }
        if (stry <= smin || stry >= smax || fabs(ds) > 0.5 * oldds) {
            stry = 0.5 * (smax + smin);
            ds   = stry - s;
            if (fabs(ds) <= fabs(stry + s) * DBL_EPSILON)
                // You might think that this test would only matter if we hit
                // this block twice in a row, in which case stry==s, and so
                // ds=0.0; thus, the loop would terminate soon.  However, it
                // doesn't always work that way if optimization is turned on
                // and it uses internal floating point registers -- ds could
                // end up close to, but not quite, zero.  The test above takes
                // care of that.  Anyway, we want to break here in order to
                // preserve value of "oldds".
                break;
            s = stry;
        } else {
            s = stry;
        }
        f_and_Df(s, f, Df);
        if (samesign(f1, f)) { // update lower bound
            // only update lower bound if have clearly valid bracket of root
            if (!samesign(f1, f2)) {
                smin = s;
                f1   = f;
                Df1  = Df;
            }
        } else { // update upper bound
            smax = s;
            f2   = f;
            Df2  = Df;
        }
        oldds = fabs(ds);
    }
    sout = s;

    if ((samesign(f1, f2)) || (oldds > tol)) {
        // do not have clearly valid bracket of root.  Maybe there is no root!
        // if both derivatives point in same direction, assume no root
        if (samesign(Df1, Df2)) return 0;
        // the following code is rarely called!
        // scan for the minimum using 64 divisions of the interval
        G4double ds = (smax - smin) / 64.0;
        for (s = smin + 0.5 * ds; s < smax; s += ds) {
            f_and_Df(s, f, Df);
            if (!samesign(f1, f) || !samesign(Df1, Df)) break;
        }
        if (ok_to_retry && !samesign(f1, f)) {
            smax        = s;
            f2          = f;
            Df2         = Df;
            ok_to_retry = false;
            goto retry; // found a bracket, so try again! (Infinite loop impossible)
        }
        return 0; // function entirely on one side!
    }

    return 1; // found the root!
}

// ---------------------------------------------------------------------------

// return integer i such that z[i] <= z_lu < z[i+1]
// or -1 if z_lu < z[0] or z_lu >= z[n-1]
G4int CupTorusStack::FindInOrderedList(double z_lu, const double *z, int n) {
    int a, b, c;

#ifdef G4DEBUG
    if (n <= 0 || z == NULL) {
        // G4Exception("Error in CupTorusStack::FindInOrderedList - "
        G4Exception(" ", " ", JustWarning,
                    "Error in CupTorusStack::FindInOrderedList - "
                    "NULL pointer or non-positive size parameter");
    }
#endif

    if (z_lu < z[0] || z_lu >= z[n - 1]) return -1;

    a = 0;
    b = n - 1;
    while (b - a > 1) {
        c = (a + b) / 2;
        if (z[c] <= z_lu)
            a = c;
        else
            b = c;
    }
    return a;
}

// ---------------------------------------------------------------------------

// find nearest segment in this torus stack to point at given (r,z)
G4int CupTorusStack::FindNearestSegment(G4double pr, G4double pz) const {
    G4double distmin = kInfinity;
    G4int imin       = -1;
    G4int i0         = FindInOrderedList(pz, z_edge, n + 1);
    G4int i;

    if (i0 < 0) {
        if (pz >= z_edge[n])
            i0 = n - 1;
        else
            i0 = 0;
    }

    if (i0 == 0 && pr - rho_edge[0] < -(pz - z_edge[0])) {
        distmin = pz - z_edge[0];
        imin    = -1; // maybe nearest flat bottom surface
        // if (distmin <= kCarTolerance)
        if (distmin <= G4GeometryTolerance::GetInstance()->GetSurfaceTolerance())
            return imin; // on or below flat bottom surface
    }

    if (i0 == n - 1 && pr - rho_edge[n] < (pz - z_edge[n])) {
        distmin = z_edge[n] - pz;
        imin    = n; // maybe nearest flat top surface
        // if (distmin <= kCarTolerance)
        if (distmin <= G4GeometryTolerance::GetInstance()->GetSurfaceTolerance())
            return imin; // on or above flat top surface
    }

    // check bounding cylinder of this region
    {
        G4double r1, r2, dr;
        if (rho_edge[i0] < rho_edge[i0 + 1]) {
            r1 = rho_edge[i0];
            r2 = rho_edge[i0 + 1];
        } else {
            r2 = rho_edge[i0];
            r1 = rho_edge[i0 + 1];
        }
        if (pr < r1)
            dr = r1 - pr;
        else if (pr > r2)
            dr = pr - r2;
        else
            return i0;
        if (dr < distmin) {
            distmin = dr;
            imin    = i0;
        }
    }

    // check edges above
    G4double dist2min = distmin * distmin;
    for (i = i0 + 1; i < n; i++) {
        G4double dz = fabs(pz - z_edge[i]);
        if (dz > distmin) break;
        G4double r1, r2, dr;
        if (rho_edge[i] < rho_edge[i + 1]) {
            r1 = rho_edge[i];
            r2 = rho_edge[i + 1];
        } else {
            r2 = rho_edge[i];
            r1 = rho_edge[i + 1];
        }
        if (pr < r1)
            dr = r1 - pr;
        else if (pr > r2)
            dr = pr - r2;
        else
            dr = 0.0;
        ;
        G4double dist2 = dr * dr + dz * dz;
        if (dist2 < dist2min) {
            dist2min = dist2;
            imin     = i;
        }
    }
    // check edges below
    for (i = i0 - 1; i >= 0; i--) {
        G4double dz = fabs(pz - z_edge[i + 1]);
        if (dz > distmin) break;
        G4double r1, r2, dr;
        if (rho_edge[i] < rho_edge[i + 1]) {
            r1 = rho_edge[i];
            r2 = rho_edge[i + 1];
        } else {
            r2 = rho_edge[i];
            r1 = rho_edge[i + 1];
        }
        if (pr < r1)
            dr = r1 - pr;
        else if (pr > r2)
            dr = pr - r2;
        else
            dr = 0.0;
        G4double dist2 = dr * dr + dz * dz;
        if (dist2 < dist2min) {
            dist2min = dist2;
            imin     = i;
        }
    }
    // note: distmin is no longer used
    //  if (dist2min < distmin*distmin)
    //    distmin= sqrt(dist2min);

    return imin;
}

// ---------------------------------------------------------------------------

// Calculate extent under transform and specified limit
G4bool CupTorusStack::CalculateExtent(const EAxis pAxis, const G4VoxelLimits &pVoxelLimit,
                                      const G4AffineTransform &pTransform, G4double &pMin,
                                      G4double &pMax) const {
    G4int i, noEntries, noBetweenSections4;
    G4bool existsAfterClip = false;

    // Calculate rotated vertex coordinates
    G4ThreeVectorList *vertices;
    G4int noPolygonVertices; // will be 4
    vertices = CreateRotatedVertices(pTransform, noPolygonVertices);

    pMin = +kInfinity;
    pMax = -kInfinity;

    noEntries          = vertices->size();
    noBetweenSections4 = noEntries - noPolygonVertices;

    for (i = 0; i < noEntries; i += noPolygonVertices) {
        ClipCrossSection(vertices, i, pVoxelLimit, pAxis, pMin, pMax);
    }

    for (i = 0; i < noBetweenSections4; i += noPolygonVertices) {
        ClipBetweenSections(vertices, i, pVoxelLimit, pAxis, pMin, pMax);
    }

    if (pMin != kInfinity || pMax != -kInfinity) {
        existsAfterClip = true;

        // Add 2*tolerance to avoid precision troubles
        // pMin-=kCarTolerance;
        // pMax+=kCarTolerance;
        pMin -= G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
        pMax += G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();

    } else {
        // Check for case where completely enveloping clipping volume
        // If point inside then we are confident that the solid completely
        // envelopes the clipping volume. Hence set min/max extents according
        // to clipping volume extents along the specified axis.
        G4ThreeVector clipCentre((pVoxelLimit.GetMinXExtent() + pVoxelLimit.GetMaxXExtent()) * 0.5,
                                 (pVoxelLimit.GetMinYExtent() + pVoxelLimit.GetMaxYExtent()) * 0.5,
                                 (pVoxelLimit.GetMinZExtent() + pVoxelLimit.GetMaxZExtent()) * 0.5);

        if (Inside(pTransform.Inverse().TransformPoint(clipCentre)) != kOutside) {
            existsAfterClip = true;
            pMin            = pVoxelLimit.GetMinExtent(pAxis);
            pMax            = pVoxelLimit.GetMaxExtent(pAxis);
        }
    }
    delete vertices;
    return existsAfterClip;
}

// -----------------------------------------------------------------

// Return whether point inside/outside/on surface

EInside CupTorusStack::Inside(const G4ThreeVector &p) const {
    EInside in = Inside1(p);

    if (in == kOutside) return in;

    // check inner solid, if there is one
    if (inner) {
        EInside in2 = inner->Inside1(p);
        if (in2 == kInside)
            return in = kOutside;
        else if (in2 == kOutside)
            return in;
        else
            return (in == kSurface) ? kOutside : kSurface;
    }

    return in;
}

EInside CupTorusStack::Inside1(const G4ThreeVector &p) const {
    EInside in = kInside;
    G4int i;

    // see if z is outside
    if (p.z() <= z_edge[0] + myRadTolerance / 2.) {
        if (p.z() < z_edge[0] - myRadTolerance / 2. || in == kSurface) return in = kOutside;
        in = kSurface;
        i  = 0;
    } else if (p.z() >= z_edge[n] - myRadTolerance / 2.) {
        if (p.z() > z_edge[n] + myRadTolerance / 2. || in == kSurface) return in = kOutside;
        in = kSurface;
        i  = n - 1;
    } else
        i = FindInOrderedList(p.z(), z_edge, n + 1); // find z in table

    // 2-d distance from axis
    G4double rp = cup_hypot(p.x(), p.y());

    // early decision -- 2*wider tolerance for safety
    if (rp > std::max(rho_edge[i], rho_edge[i + 1]) + myRadTolerance) return in = kOutside;

    // detailed check
    G4double drtor;
    if (b[i] == 0.0) {
        drtor = rp - a[i];
    } else {
        if ((b[i] > 0.0) ? (rp >= a[i]) : (rp <= a[i])) {
            drtor = 0.5 * ((square(rp - a[i]) + square(p.z() - z_o[i])) / b[i] - b[i]);
        } else {
            drtor = 0.5 * (square(p.z() - z_o[i]) / b[i] - b[i]);
        }
    }

    if (drtor > 0.5 * myRadTolerance)
        in = kOutside;
    else if (drtor > -0.5 * myRadTolerance)
        in = kSurface;
    else {
        ; /* leave as is -- we may have set in= kSurface above, else inside */
    }
    return in;
}

// -----------------------------------------------------------------------

// Return unit normal of surface closest to p
// returns silly value if point is exactly on z axis

G4ThreeVector CupTorusStack::SurfaceNormal(const G4ThreeVector &p) const {
    // before doing anything else, see if we have an inner solid
    if (inner) {
        // if so, is this point on the inner surface or inside the inner volume?
        if (inner->Inside1(p) != kOutside) {
            // use inner surface
            return -(inner->SurfaceNormal(p));
        }
    }

    // otherwise, proceed to find normal of surface of main (exterior) solid
    G4ThreeVector norm;
    G4double pr, pz;

    pz = p.z();
    pr = cup_hypot(p.x(), p.y());

    // find index of region containing nearest point on surface
    G4int i = FindNearestSegment(pr, pz);

    // set normal
    if (i == -1) { // bottom surface
        norm = G4ThreeVector(0.0, 0.0, -1.0);
    } else if (i == n) { // top surface
        norm = G4ThreeVector(0.0, 0.0, +1.0);
    } else if (pr == 0.0) { // point is on z-axis -- it is silly!
        norm = G4ThreeVector(0.0, 0.0, +1.0);
    } else if (b[i] == 0.0) { // cylindrical surface
        norm = G4ThreeVector(p.x() / pr, p.y() / pr, 0.0);
    } else { // toroidal surface
        G4double dz  = pz - z_o[i];
        G4double dr  = pr - a[i];
        G4double nrm = cup_hypot(dz, dr);
        if (nrm == 0.0) { // point is at center of toroid x-section -- ok
            norm = G4ThreeVector(p.x() / pr, p.y() / pr, 0.0);
        } else { // this is the usual case
            norm = G4ThreeVector(p.x() * dr / (pr * nrm), p.y() * dr / (pr * nrm), dz / nrm);
        }
        if (b[i] < 0.0) // handle concave surface
            norm = -norm;
        if ((b[i] < 0.0) != (dr < 0.0)) {
            G4cout.flush();
            G4cerr << "Warning from CupTorusStack::SurfaceNormal: (" << GetName()
                   << ") position inconsistent with concavity\n\tb[i]=" << b[i]
                   << " but a[i]=" << a[i] << " and pr=" << pr << G4endl;
            G4cerr.flush();
        }
    }

    return norm;
}

// Calculate distance to shape from outside, along normalised vector
// - return kInfinity if no intersection
// - return 0 if inside or on surface and moving inside
// - if there is an inner solid, assumes it is completely contained in main
//   except possibly at flat ends (z_edge[0] and z_edge[n])

G4double CupTorusStack::DistanceToIn(const G4ThreeVector &p, const G4ThreeVector &v) const {
    G4double dist_to_in = kInfinity;
    DEBUG_CupTorusStack(dist_to_in = 1.01e100;)

        // do we have an inner solid to consider?
        if (inner) {
        // are we inside or on the surface of the inner (subtracted) solid?
        if (inner->Inside1(p) != kOutside) {
            dist_to_in = inner->DistanceToOut(p, v);
            if (Inside1(p + dist_to_in * v) == kInside)
                return dist_to_in;
            else {
                DEBUG_CupTorusStack(return 1.02e100;) return kInfinity; // no intersection
            }
        }
    }
    // note: if we are here, then inner==NULL or inner->Inside(p)==kOutside

    // proceed with DistanceToIn for main (exterior) solid
    G4int iedge, idir;

    // find where we are in z
    // if (p.z() > z_edge[n]-kCarTolerance) {  // above top of torus stack
    if (p.z() >
        z_edge[n] -
            G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()) { // above top of torus stack
        if (v.z() >= 0.0) {
            DEBUG_CupTorusStack(return 1.03e100;) return kInfinity;
        }
        iedge = n - 1;
        idir  = -1;
    }
    // else if (p.z() < z_edge[0]+kCarTolerance) {
    else if (p.z() < z_edge[0] + G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()) {
        if (v.z() <= 0.0) {
            DEBUG_CupTorusStack(return 1.04e100;) return kInfinity;
        }
        iedge = 0;
        idir  = +1;
    } else {
        iedge = FindInOrderedList(p.z(), z_edge, n + 1); // find z in table
        idir  = (v.z() >= 0.0) ? +1 : -1;
    }

    // set up for radial distance calcs
    G4double rp2   = square(p.x()) + square(p.y());
    G4double rpv   = p.x() * v.x() + p.y() * v.y();
    G4double rv2   = square(v.x()) + square(v.y());
    G4double rmin2 = (rpv >= 0.0) ? rp2 : rp2 - square(rpv) / rv2;

    // check to see if we're totally outside
    if (rmin2 > square(max_rho + myRadTolerance)) {
        DEBUG_CupTorusStack(return 1.05e100;) return kInfinity;
    }

    // check for intersection with flat end -- early exit if so
    // if (p.z() > z_edge[n]-kCarTolerance
    if (p.z() > z_edge[n] - G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() &&
        v.z() < 0.0 && square(rho_edge[n]) >= rmin2) {
        G4double distZ   = (z_edge[n] - p.z()) / v.z();
        G4ThreeVector pi = p + distZ * v;
        if (square(pi.x()) + square(pi.y()) <= square(rho_edge[n])) {
            if (inner == NULL || inner->Inside1(pi) == kOutside) {
                dist_to_in = distZ;
                if (dist_to_in < myRadTolerance) dist_to_in = 0.0;
                return dist_to_in;
            }
        }
    }
    // if (p.z() < z_edge[0]+kCarTolerance
    if (p.z() < z_edge[0] + G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() &&
        v.z() > 0.0 && square(rho_edge[0]) >= rmin2) {
        G4double distZ   = (z_edge[0] - p.z()) / v.z();
        G4ThreeVector pi = p + distZ * v;
        if (square(pi.x()) + square(pi.y()) <= square(rho_edge[0])) {
            if (inner == NULL || inner->Inside1(pi) == kOutside) {
                dist_to_in = distZ;
                if (dist_to_in < myRadTolerance) dist_to_in = 0.0;
                return dist_to_in;
            }
        }
    }

    // loop checking for intersections
    G4double tmin = (rv2 > 0.0) ? (-rpv / rv2) : (0.0);
    for (; iedge < n && iedge >= 0; iedge += idir) {
        G4double r_out2 = square(std::max(rho_edge[iedge], rho_edge[iedge + 1]));
        if (rmin2 > r_out2) continue;
        G4double tup, tdown;
        if (v.z() == 0.0) {
            tdown = (p.z() == z_edge[iedge]) ? 0.0 : (z_edge[iedge] - p.z()) * kInfinity;
            tup   = (p.z() == z_edge[iedge + 1]) ? 0.0 : (z_edge[iedge + 1] - p.z()) * kInfinity;
        } else {
            tdown = (z_edge[iedge] - p.z()) / v.z();
            tup   = (z_edge[iedge + 1] - p.z()) / v.z();
        }
        if (((tdown < tmin) ^ (tup < tmin))                    // closest approach is inside
            || rp2 + (2 * rpv + rv2 * tdown) * tdown <= r_out2 // inside at bottom
            || rp2 + (2 * rpv + rv2 * tup) * tup <= r_out2) {  // inside at top
            // track passes through outer bounding cylinder of this segment
            G4double s;
            G4int nroots;
            G4double s1, s2;
            if (tup < tdown) { // set lower and upper brackets
                s1 = tup - myRadTolerance;
                s2 = tdown + myRadTolerance;
            } else {
                s1 = tdown - myRadTolerance;
                s2 = tup + myRadTolerance;
            }
            if (s1 < myRadTolerance)  // clip minimum distance of root
                s1 = -myRadTolerance; // "-" to include all of surface
            if (rv2 > 0.0) {
                s = tmin + sqrt(tmin * tmin + (r_out2 - rp2) / rv2) + myRadTolerance;
                if (s1 < s && s2 > s) s2 = s; // clip maximum distance of root
            }
            nroots = FindFirstTorusRoot(a[iedge], // swept radius
                                        b[iedge], // cross-section radius
                                        G4ThreeVector(p.x(), p.y(), p.z() - z_o[iedge]), // start
                                        v, // ray direction
                                        s1, s2, true,
                                        s); // returned root
            if (nroots > 0) {               // found the intersection!
                dist_to_in = s;
                break;
            }
        } // end of exact intercept for track through bounding cyl
    }     // end of edge scan

    // don't bother checking to see if intercept is inside
    // subtracted (inner) volume, since that should only happen on ends

    // all done
    if (dist_to_in < myRadTolerance) dist_to_in = 0.0;
    return dist_to_in;
}

// ----------------------------------------------------------------------

// Calculate distance (<= actual) to closest surface of shape from outside
// - Return 0 if point inside

G4double CupTorusStack::DistanceToIn(const G4ThreeVector &p) const {
    G4double safe;
    G4double pr, pz;

    pz = p.z();
    pr = cup_hypot(p.x(), p.y());

    // find index of region containing nearest point on surface
    G4int i = FindNearestSegment(pr, pz);

    if (i == -1) { // bottom surface
        safe = z_edge[0] - pz;
    } else if (i == n) { // top surface
        safe = pz - z_edge[n];
    } else {
        safe = pr - std::max(rho_edge[i], rho_edge[i + 1]);
    }

    if (safe < 0.0) safe = 0.0;

    // if we're inside both the exterior and the inner solid,
    // then use the safe DistanceToOut of inner solid.
    if (safe == 0.0 && inner != NULL && inner->Inside1(p) == kInside) {
        safe = inner->DistanceToOut(p);
        if (safe < 0.0) safe = 0.0;
    }

    return safe;
}

// Calculate distance to surface of shape from `inside', allowing for tolerance

G4double CupTorusStack::DistanceToOut(const G4ThreeVector &p, const G4ThreeVector &v,
                                      const G4bool calcNorm, G4bool *validNorm,
                                      G4ThreeVector *norm) const {
    G4double dist_to_out = kInfinity;

    // do we have an inner solid to consider?
    if (inner) {
        // We should be outside the inner (subtracted) solid
        // and inside the main (exterior) solid.
        // If track intercepts inner solid at point inside the main solid,
        // then that must be first intercept with any boundary.
        // Otherwise, does not intercept with the inner solid within main solid.
        G4double inner_dist_to_in = inner->DistanceToIn(p, v);
        if (inner_dist_to_in < kInfinity) {
            G4ThreeVector pi = p + inner_dist_to_in * v;
            if (Inside1(pi) != kOutside) {
                if (calcNorm) {
                    *validNorm = false;
                    *norm      = -(inner->SurfaceNormal(pi));
                }
                return inner_dist_to_in;
            }
        }
    }

    // proceed with DistanceToOut for main (exterior) solid
    G4int iedge, idir, isurface;

    // default for validNorm:
    if (calcNorm && validNorm != NULL) {
        *validNorm = false;
    }
    isurface = -2;

    // find where we are in z
    if (p.z() > z_edge[n] + 0.5 * myRadTolerance) {
        return 0.0;
    }
    if (p.z() < z_edge[0] - 0.5 * myRadTolerance) {
        return 0.0;
    }
    if (p.z() >= z_edge[n] - 0.5 * myRadTolerance) {
        if (v.z() >= 0.0) return 0.0;
        iedge = n - 1;
        idir  = -1;
    } else if (p.z() <= z_edge[0] + 0.5 * myRadTolerance) {
        if (v.z() <= 0.0) return 0.0;
        iedge = 0;
        idir  = +1;
    } else {
        iedge = FindInOrderedList(p.z(), z_edge, n + 1); // find z in table
        idir  = (v.z() >= 0.0) ? +1 : -1;
    }

    // set up for radial distance calcs
    G4double rp2 = square(p.x()) + square(p.y());
    G4double rpv = p.x() * v.x() + p.y() * v.y();
    G4double rv2 = square(v.x()) + square(v.y());

    // check to see if we're totally outside
    if (rp2 > square(max_rho + myRadTolerance)) {
        return 0.0;
    }

    // Note: check for intercept with ends is done after segment loop

    // loop checking for intersections with sides
    G4double tmin = (rv2 > 0) ? (-rpv / rv2) : 0.0;
    for (; iedge < n && iedge >= 0; iedge += idir) {
        G4double r_in2 = square(std::min(rho_edge[iedge], rho_edge[iedge + 1]));
        G4double tup, tdown;
        if (v.z() == 0.0) {
            tdown = 0.0;
            tup   = 2.0 * max_rho + myRadTolerance;
        } else {
            tdown = (z_edge[iedge] - p.z()) / v.z();
            tup   = (z_edge[iedge + 1] - p.z()) / v.z();
        }
        if (rp2 + (2 * rpv + rv2 * tdown) * tdown >= r_in2   // outside at bottom
            || rp2 + (2 * rpv + rv2 * tup) * tup >= r_in2) { // outside at top
            // track passes through inner bounding cylinder of this segment
            G4double s;
            G4int nroots;
            G4double s1, s2;
            if (tup < tdown) { // set lower and upper brackets
                s1 = tup;
                s2 = tdown;
            } else {
                s1 = tdown;
                s2 = tup;
            }
            if (s1 < myRadTolerance)  // clip minimum distance of root
                s1 = -myRadTolerance; // "-" to include all of surface
            if (rv2 > 0.0) {
                s = tmin + sqrt(tmin * tmin + (max_rho * max_rho - rp2) / rv2) + myRadTolerance;
                if (s1 < s && s2 > s) s2 = s; // clip maximum distance of root
            }
            nroots = FindFirstTorusRoot(a[iedge], // swept radius
                                        b[iedge], // cross-section radius
                                        G4ThreeVector(p.x(), p.y(), p.z() - z_o[iedge]), // start
                                        v, // ray direction
                                        s1, s2, false,
                                        s); // returned root
            if (nroots > 0) {               // found the intersection!
                dist_to_out = s;
                isurface    = iedge;
                break;
            }
        } // end of exact intercept for track through bounding cyl
    }     // end of edge scan

    // check for intersection with flat end
    if (v.z() > 0.0) {
        G4double t = (z_edge[n] - p.z()) / v.z();
        if (t > -0.5 * myRadTolerance && t < dist_to_out) {
            dist_to_out = t;
            isurface    = n;
        }
    } else if (v.z() < 0.0) {
        G4double t = (z_edge[0] - p.z()) / v.z();
        if (t > -0.5 * myRadTolerance && t < dist_to_out) {
            dist_to_out = t;
            isurface    = -1;
        }
    }

#ifdef G4DEBUG
    if (dist_to_out >= kInfinity) {
        G4cerr << "WARNING from CupTorusStack::DistanceToOut: " << GetName()
               << "did not find an intercept with the track!\n";
    }
#endif

    if (calcNorm && norm) {
        if (isurface < 0) {
            *norm      = G4ThreeVector(0., 0., -1.);
            *validNorm = (isurface == -1);
        } else if (isurface >= n) {
            *norm      = G4ThreeVector(0., 0., +1.);
            *validNorm = (isurface == n);
        } else {
            G4ThreeVector psurf = p + v * dist_to_out;
            G4double pr, pz;
            pz = psurf.z();
            pr = cup_hypot(psurf.x(), psurf.y());
            if (pr == 0.0) { // point is on z-axis -- it is silly!
                *norm      = G4ThreeVector(0.0, 0.0, +1.0);
                *validNorm = false;
            } else if (b[isurface] == 0.0) { // cylindrical surface
                *norm      = G4ThreeVector(psurf.x() / pr, psurf.y() / pr, 0.0);
                *validNorm = true;
            } else { // toroidal surface
                G4double dz  = pz - z_o[isurface];
                G4double dr  = pr - a[isurface];
                G4double nrm = cup_hypot(dz, dr);
                if (nrm == 0.0) { // point is at center of toroid x-section -- ok
                    *norm = G4ThreeVector(psurf.x() / pr, psurf.y() / pr, 0.0);
                } else { // this is the usual case
                    *norm = G4ThreeVector(psurf.x() * dr / (pr * nrm), psurf.y() * dr / (pr * nrm),
                                          dz / nrm);
                }
                if (b[isurface] < 0.0) // handle concave surface
                    *norm = -*norm;
                *validNorm = true; /* (b[isurface] > 0.0); */
                if (dr != 0.0 && (b[isurface] < 0.0) != (dr < 0.0)) {
                    G4cout.flush();
                    G4cerr
                        << "Warning from CupTorusStack::DistanceToOut surface normal calculation ("
                        << GetName()
                        << "): position inconsistent with concavity\n\tb[isurface]=" << b[isurface]
                        << " but a[isurface]=" << a[isurface] << " and pr=" << pr << G4endl;
                    G4cerr.flush();
                }
            }
        }
        if ((*norm) * v <= 0.0) {
            G4cout.flush();
            G4cerr << "Warning from CupTorusStack::DistanceToOut: (" << GetName()
                   << ") I have calculated a normal that is antiparallel to the momentum vector!  "
                      "I must "
                      "have done something wrong! isurface="
                   << isurface << " a[isurface]=" << a[isurface] << " b[isurface]=" << b[isurface]
                   << " v=" << v << " norm=" << (*norm) << G4endl;
            *norm = -*norm;
            G4cerr.flush();
        }
    }

    if (dist_to_out < myRadTolerance) dist_to_out = 0.0;
    return dist_to_out;
}

// Calcluate distance (<=actual) to closest surface of shape from inside
G4double CupTorusStack::DistanceToOut(const G4ThreeVector &p) const {
    G4double safe;
    G4double pr, pz;

    pz = p.z();
    pr = cup_hypot(p.x(), p.y());

    // find index of region containing nearest point on surface
    G4int i = FindNearestSegment(pr, pz);

    if (i == -1) { // bottom surface
        safe = pz - z_edge[0];
    } else if (i == n) { // top surface
        safe = z_edge[n] - pz;
    } else {
        safe = std::min(rho_edge[i], rho_edge[i + 1]) - pr;
    }

    if (safe < 0.0) safe = 0.0;

    // if we're safely inside the exterior solid,
    // and we have an inner solid,
    // then check safe DistanceToIn of inner solid.
    // use minimum safe distance
    if (safe > 0.0 && inner != NULL) {
        G4double safe2 = inner->DistanceToIn(p);
        if (safe2 < 0.0) safe2 = 0.0;
        if (safe2 < safe) safe = safe2;
    }

    return safe;
}

// ----------------------------------------------------------------------

// Create a List containing the transformed vertices
//
// For now, just return vertices of bounding cylinder
// (just like G4Torus as of Geant 4.0.1 -- fixme? ? ?)
//
// Note:
//  Caller has deletion resposibility
//
G4ThreeVectorList *CupTorusStack::CreateRotatedVertices(const G4AffineTransform &pTransform,
                                                        G4int &noPolygonVertices) const {
    G4ThreeVectorList *vertices;
    G4ThreeVector vertex0, vertex1, vertex2, vertex3;
    G4double meshAngle, meshRMax, crossAngle, cosCrossAngle, sinCrossAngle, sAngle;
    G4double rMaxX, rMaxY;
    G4int crossSection, noCrossSections;

    // Compute no of cross-sections necessary to mesh tube
    noCrossSections = G4int(2.0 * M_PI / kMeshAngleDefault) + 1;
    if (noCrossSections < kMinMeshSections) {
        noCrossSections = kMinMeshSections;
    } else if (noCrossSections > kMaxMeshSections) {
        noCrossSections = kMaxMeshSections;
    }

    meshAngle = 2.0 * M_PI / (noCrossSections - 1);
    meshRMax  = max_rho / cos(meshAngle * 0.5);

    // set start angle such that mesh will be at fRmax
    // on the x axis. Will give better extent calculations when not rotated.
    sAngle = -meshAngle * 0.5;

    vertices = new G4ThreeVectorList(noCrossSections * 4);
    if (vertices) {
        for (crossSection = 0; crossSection < noCrossSections; crossSection++) {
            // Compute coordinates of cross section at section crossSection
            crossAngle    = sAngle + crossSection * meshAngle;
            cosCrossAngle = cos(crossAngle);
            sinCrossAngle = sin(crossAngle);

            rMaxX   = meshRMax * cosCrossAngle;
            rMaxY   = meshRMax * sinCrossAngle;
            vertex0 = G4ThreeVector(0.0, 0.0, z_edge[0]);
            vertex1 = G4ThreeVector(rMaxX, rMaxY, z_edge[0]);
            vertex2 = G4ThreeVector(rMaxX, rMaxY, z_edge[n]);
            vertex3 = G4ThreeVector(0.0, 0.0, z_edge[n]);

            vertices->push_back(pTransform.TransformPoint(vertex0));
            vertices->push_back(pTransform.TransformPoint(vertex1));
            vertices->push_back(pTransform.TransformPoint(vertex2));
            vertices->push_back(pTransform.TransformPoint(vertex3));
        }
        noPolygonVertices = 4;
    } else {
        // G4Exception("CupSim/CupTorusStack::CreateRotatedVertices Out of memory - Cannot alloc
        // vertices");
        G4Exception(
            " ", " ", JustWarning,
            "CupSim/CupTorusStack::CreateRotatedVertices Out of memory - Cannot alloc vertices");
    }
    return vertices;
}

void CupTorusStack::DescribeYourselfTo(G4VGraphicsScene &scene) const {
#if (G4VERSION_NUMBER <= 960)
    scene.AddThis(*this); // function was named AddThis for a long time...
#else
    scene.AddSolid(*this); // renamed to AddSolid in Geant4 7.01
#endif
}

G4VisExtent CupTorusStack::GetExtent() const {
    // Define the sides of the box into which the CupTorusStack instance would fit.
    return G4VisExtent(-max_rho, max_rho, -max_rho, max_rho, z_edge[0], z_edge[n]);
}

// ================================================================

class CupPolyhedronTorusStack : public G4Polyhedron {
  public:
    CupPolyhedronTorusStack(const G4int n, const G4double z_edge[], const G4double rho_edge[],
                            const G4double z_o[], const G4double a[], const G4double b[],
                            const G4int inner_n, const G4double inner_z_edge[],
                            const G4double inner_rho_edge[], const G4double inner_z_o[],
                            const G4double inner_a[], const G4double inner_b[]);
    static G4int MakeSegment(G4double rho0, G4double rho1, G4double z0, G4double z1, G4double z_o,
                             G4double a, G4double b, G4int ns, G4double rr[], G4double zz[]);
    virtual G4Visible &operator=(const G4Visible &from) { return G4Visible::operator=(from); }
    // EJ: start
    //  virtual G4VVisPrim& operator=(const G4VVisPrim &from)
    //    { return G4VVisPrim::operator = (from); }
    // EJ: endl
};

G4int CupPolyhedronTorusStack::MakeSegment(G4double rho0, G4double rho1, G4double z0, G4double z1,
                                           G4double z_o, G4double a, G4double b, G4int ns,
                                           G4double rr[], G4double zz[]) {
    G4int j;

    j = 0;
    if (rho1 == rho0) {
        zz[j] = z0;
        rr[j] = rho0;
        j++;
    } else {
        double dz  = fabs(z0 - z1);
        double dr  = fabs(rho0 - rho1);
        double fac = (dr + dz) / fabs(b) / 1.5; // crude apx to arc length/(pi/2)
        if (fac < 1.0) {
            ns = (int)((ns - 1.5) * fac + 1.5);
            if (ns < 2) ns = 2;
        }
        if (dz > dr) {
            for (G4int k = 0; k < ns; k++) {
                zz[j]         = z0 + k / (double)ns * (z1 - z0);
                G4double dtmp = 1.0 - square((zz[j] - z_o) / b);
                if (dtmp < 0.0) dtmp = 0.0;
                rr[j] = a + b * sqrt(dtmp);
                j++;
            }
        } else {
            for (G4int k = 0; k < ns; k++) {
                rr[j]         = rho0 + k / (double)ns * (rho1 - rho0);
                G4double dtmp = 1.0 - square((rr[j] - a) / b);
                if (dtmp < 0.0) dtmp = 0.0;
                zz[j] = z_o + fabs(b) * sqrt(dtmp) * (z0 > z_o ? +1 : -1);
                j++;
            }
        }
    }

    return j;
}

CupPolyhedronTorusStack::CupPolyhedronTorusStack(const G4int n, const G4double z_edge[],
                                                 const G4double rho_edge[], const G4double z_o[],
                                                 const G4double a[], const G4double b[],
                                                 const G4int inner_n, const G4double inner_z_edge[],
                                                 const G4double inner_rho_edge[],
                                                 const G4double inner_z_o[],
                                                 const G4double inner_a[], const G4double inner_b[])
/***********************************************************************
 *                                                                     *
 * Name: CupPolyhedronTorusStack                                      *
 * Author: G.Horton-Smith (Tohoku)                 Revised: 1999.11.11 *
 *                                                                     *
 * Function: Constructor of polyhedron for   TorusStack                *
 *                                                                     *
 * Input: n        - number of segments                                *
 *        z_edge   - n+1 edges of Z-segments                           *
 *        rho_edge - n+1 dist. from Z-axis                             *
 *        z_o      - location of torus segment center on Z-axis        *
 *        a        - swept radius of torus segment                     *
 *        b        - cross-section radius of torus segment             *
 *                                                                     *
 ***********************************************************************/
{
    //   C H E C K   I N P U T   P A R A M E T E R S

    if (n <= 0 || (&z_edge) == NULL || (&rho_edge) == NULL || (&z_o) == NULL) {
        G4cerr << "Error: bad parameters in CupPolyhedronTorusStack!" << G4endl;
        // G4Exception( "CupSim/CupPolyhedronTorusStack: bad parameters!" );
        G4Exception(" ", " ", JustWarning, "CupSim/CupPolyhedronTorusStack: bad parameters!");
    }

    //   P R E P A R E   T W O   P O L Y L I N E S

    G4int ns  = GetNumberOfRotationSteps() / 4 + 2;
    G4int np1 = n * ns + 3; // ns steps for each segment + 1, plus one on each end
    G4int np2 = (inner_n == 0) ? 1 : (inner_n)*ns + 3;

    G4double *zz, *rr;
    zz = new G4double[np1 + np2];
    rr = new G4double[np1 + np2];
    if (!zz || !rr) {
        // G4Exception("Out of memory in CupPolyhedronTorusStack!");
        G4Exception(" ", " ", JustWarning, "Out of memory in CupPolyhedronTorusStack!");
    }

    zz[0] = z_edge[0];
    rr[0] = (inner_n == 0) ? 0.0 : inner_rho_edge[0]; // closes end
    int i, j;
    for (i = 0, j = 1; i < n; i++) {
        j += MakeSegment(rho_edge[i], rho_edge[i + 1], z_edge[i], z_edge[i + 1], z_o[i], a[i], b[i],
                         ns, rr + j, zz + j);
    }
    zz[j] = z_edge[n];
    rr[j] = rho_edge[n];
    j++;
    if (j >= np1 - 1) {
        // G4Exception("Logic error in CupPolyhedronTorusStack, memory corrupted!");
        G4Exception(" ", " ", JustWarning,
                    "Logic error in CupPolyhedronTorusStack, memory corrupted!");
    }
    zz[j] = z_edge[n];
    rr[j] = (inner_n == 0) ? 0.0 : inner_rho_edge[inner_n]; // closes end
    j++;
    np1 = j;
    // generate inner surface
    if (inner_n == 0) {
        zz[j] = 0.0;
        rr[j] = 0.0;
        np1--;
    } else {
        for (i = inner_n - 1; i >= 0; i--) {
            if (inner_z_edge[i + 1] > z_edge[n] || inner_z_edge[i] < z_edge[0]) continue;
            j += MakeSegment(inner_rho_edge[i + 1], inner_rho_edge[i], inner_z_edge[i + 1],
                             inner_z_edge[i], inner_z_o[i], inner_a[i], inner_b[i], ns, rr + j,
                             zz + j);
        }
        zz[j] = zz[0];
        rr[j] = rr[0];
        j++;
    }
    if (j - np1 > np2) {
        // G4Exception("Logic error 2 in CupPolyhedronTorusStack, memory corrupted!");
        G4Exception(" ", " ", JustWarning,
                    "Logic error 2 in CupPolyhedronTorusStack, memory corrupted!");
    }
    np2 = j - np1;

    //   R O T A T E    P O L Y L I N E S

    RotateAroundZ(0, 0.0, 2.0 * M_PI, np1, np2, zz, rr, -1, 1);
    SetReferences();

    delete[] zz;
    delete[] rr;
}

G4Polyhedron *CupTorusStack::CreatePolyhedron() const {
    if (inner == NULL) {
        return new CupPolyhedronTorusStack(n, z_edge, rho_edge, z_o, a, b, 0, NULL, NULL, NULL,
                                           NULL, NULL);
    } else {
        return new CupPolyhedronTorusStack(n, z_edge, rho_edge, z_o, a, b, inner->n, inner->z_edge,
                                           inner->rho_edge, inner->z_o, inner->a, inner->b);
    }
}

#if G4VERSION_NUMBER <= 999
#include "G4NURBS.hh"
#include "G4NURBScylinder.hh"
#include "G4NURBStube.hh"
#include "G4NURBStubesector.hh"
G4NURBS *CupTorusStack::CreateNURBS() const {
    G4NURBS *pNURBS;
    pNURBS = new G4NURBScylinder(max_rho, std::max(fabs(z_edge[0]), fabs(z_edge[n])));
    return pNURBS;
}
#endif
