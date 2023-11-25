//
#ifndef CupScintHit_h
#define CupScintHit_h 1

#include "G4Allocator.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4VHit.hh"
#include "G4Version.hh"

class G4AttDef;
class G4AttValue;

using namespace CLHEP;

class CupScintHit : public G4VHit {
  public:
    CupScintHit();
    CupScintHit(G4int z);
    virtual ~CupScintHit();
    CupScintHit(const CupScintHit &right);
    const CupScintHit &operator=(const CupScintHit &right);
    int operator==(const CupScintHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual void Draw();
    virtual const std::map<G4String, G4AttDef> *GetAttDefs() const;
    virtual std::vector<G4AttValue> *CreateAttValues() const;
    virtual void Print();

  private:
    G4int cellID;
    G4double edep;
    G4double edep_quenched;
    G4ThreeVector pos;
    G4RotationMatrix rot;
    const G4LogicalVolume *pLogV;

  public:
    inline void SetCellID(G4int z) { cellID = z; }
    inline G4int GetCellID() const { return cellID; }
    inline void SetEdep(G4double de) { edep = de; }
    inline void AddEdep(G4double de) { edep += de; }
    inline G4double GetEdep() const { return edep; }
    inline void SetEdepQuenched(G4double dequenched) { edep_quenched = dequenched; }
    inline void AddEdepQuenched(G4double dequenched) { edep_quenched += dequenched; }
    inline G4double GetEdepQuenched() const { return edep_quenched; }
    inline void SetPos(G4ThreeVector xyz) { pos = xyz; }
    inline G4ThreeVector GetPos() const { return pos; }
    inline void SetRot(G4RotationMatrix rmat) { rot = rmat; }
    inline G4RotationMatrix GetRot() const { return rot; }
    inline void SetLogV(G4LogicalVolume *val) { pLogV = val; }
    inline const G4LogicalVolume *GetLogV() const { return pLogV; }
};

typedef G4THitsCollection<CupScintHit> CupScintHitsCollection;

#if G4VERSION_NUMBER <= 999
extern G4Allocator<CupScintHit> *CupScintHitAllocator;
#else
extern G4ThreadLocal G4Allocator<CupScintHit> *CupScintHitAllocator;
#endif

inline void *CupScintHit::operator new(size_t) {
    void *aHit;
    if (!CupScintHitAllocator) CupScintHitAllocator = new G4Allocator<CupScintHit>;
    aHit = (void *)CupScintHitAllocator->MallocSingle();
    return aHit;
}

inline void CupScintHit::operator delete(void *aHit) {
    CupScintHitAllocator->FreeSingle((CupScintHit *)aHit);
}

#endif
