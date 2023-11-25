
#include "CupSim/CupVetoHit.hh"
#include "G4AttDef.hh"
#include "G4AttDefStore.hh"
#include "G4AttValue.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

#if G4VERSION_NUMBER <= 999
G4Allocator<CupVetoHit> *CupVetoHitAllocator = nullptr;
#else
G4ThreadLocal G4Allocator<CupVetoHit> *CupVetoHitAllocator = nullptr;
#endif

CupVetoHit::CupVetoHit() {
    cellID        = -1;
    edep          = 0.;
    edep_quenched = 0.;
    pLogV         = 0;
}

CupVetoHit::CupVetoHit(G4int z) {
    cellID        = z;
    edep          = 0.;
    edep_quenched = 0.;
    pLogV         = 0;
}

CupVetoHit::~CupVetoHit() { ; }

CupVetoHit::CupVetoHit(const CupVetoHit &right) : G4VHit() {
    cellID        = right.cellID;
    edep          = right.edep;
    edep_quenched = right.edep_quenched;
    pos           = right.pos;
    rot           = right.rot;
    pLogV         = right.pLogV;
}

const CupVetoHit &CupVetoHit::operator=(const CupVetoHit &right) {
    cellID        = right.cellID;
    edep          = right.edep;
    edep_quenched = right.edep_quenched;
    pos           = right.pos;
    rot           = right.rot;
    pLogV         = right.pLogV;
    return *this;
}

int CupVetoHit::operator==(const CupVetoHit &right) const { return (cellID == right.cellID); }

void CupVetoHit::Draw() {
    G4VVisManager *pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager && (edep > 0.)) {
        // Draw a calorimeter cell with a color corresponding to its energy deposit
        G4Transform3D trans(rot.inverse(), pos);
        G4VisAttributes attribs;
        const G4VisAttributes *pVA = pLogV->GetVisAttributes();
        if (pVA) attribs = *pVA;
        G4double rcol = edep / (0.7 * GeV);
        if (rcol > 1.) rcol = 1.;
        if (rcol < 0.4) rcol = 0.4;
        G4Colour colour(rcol, 0., 0.);
        attribs.SetColour(colour);
        attribs.SetForceSolid(true);
        pVVisManager->Draw(*pLogV, attribs, trans);
    }
}

const std::map<G4String, G4AttDef> *CupVetoHit::GetAttDefs() const {
    G4bool isNew;
    std::map<G4String, G4AttDef> *store = G4AttDefStore::GetInstance("CupSim/CupVetoHit", isNew);
    if (isNew) {
        G4String HitType("HitType");
        (*store)[HitType] = G4AttDef(HitType, "Hit Type", "Physics", "", "G4String");

        G4String ID("ID");
        (*store)[ID] = G4AttDef(ID, "ID", "Physics", "", "G4int");

        G4String Energy("Energy");
        (*store)[Energy] =
            G4AttDef(Energy, "Energy Deposited", "Physics", "G4BestUnit", "G4double");

        G4String Pos("Pos");
        (*store)[Pos] = G4AttDef(Pos, "Position", "Physics", "G4BestUnit", "G4ThreeVector");

        G4String LVol("LVol");
        (*store)[LVol] = G4AttDef(LVol, "Logical Volume", "Physics", "", "G4String");
    }
    return store;
}

std::vector<G4AttValue> *CupVetoHit::CreateAttValues() const {
    std::vector<G4AttValue> *values = new std::vector<G4AttValue>;

    values->push_back(G4AttValue("HitType", "TGSDHit", ""));

    values->push_back(G4AttValue("ID", G4UIcommand::ConvertToString(cellID), ""));

    values->push_back(G4AttValue("Energy", G4BestUnit(edep, "Energy"), ""));

    values->push_back(
        G4AttValue("EnergyQuenched", G4BestUnit(edep_quenched, "EnergyQuenched"), ""));

    values->push_back(G4AttValue("Pos", G4BestUnit(pos, "Length"), ""));

    if (pLogV)
        values->push_back(G4AttValue("LVol", pLogV->GetName(), ""));
    else
        values->push_back(G4AttValue("LVol", " ", ""));

    return values;
}

void CupVetoHit::Print() {
    G4cout << "  Cell[" << cellID << "] " << edep / MeV << " (MeV)" << G4endl;
}
