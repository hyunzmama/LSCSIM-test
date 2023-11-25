#ifndef PRIMARY_H
#define PRIMARY_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Primary                                                                //
//                                                                      //
// Description of the event and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "TObject.h"
//#include "TMath.h"
#include "MCObjs/Vertex.hh"

class TDirectory;

class Primary : public TObject {

  private:
    Int_t fNvertex;
    TClonesArray *fVertex; //->array with all tracks
    Double_t ketot;
    Double_t centroid_x; // Vertex centroid x
    Double_t centroid_y; // Vertex centroid y
    Double_t centroid_z; // Vertex centroid z

    static TClonesArray *fgVertex;

  public:
    Primary();
    virtual ~Primary();
    void Clear(Option_t *option = "");

    void SetNvertex(Int_t n) { fNvertex = n; }
    void SetKEtot(Double_t ke) { ketot = ke; }
    void SetCentroidX(Double_t x) { centroid_x = x; }
    void SetCentroidY(Double_t y) { centroid_y = y; }
    void SetCentroidZ(Double_t z) { centroid_z = z; }

    Int_t GetNvertex() const { return fNvertex; }
    Double_t GetKEtot() const { return ketot; }
    Double_t GetCentroidX() const { return centroid_x; }
    Double_t GetCentroidY() const { return centroid_y; }
    Double_t GetCentroidZ() const { return centroid_z; }
    TClonesArray *GetVertex() const { return fVertex; }

    ClassDef(Primary, 2) // Primary structure
};

#endif
