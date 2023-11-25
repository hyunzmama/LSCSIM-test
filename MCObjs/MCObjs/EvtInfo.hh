#ifndef EVTINFO_H
#define EVTINFO_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// EvtInfo                                                               //
//                                                                      //
// Description of the event parameters                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class EvtInfo : public TObject {

  private:
    Int_t eventID;
    Int_t runID;
    Int_t eventType;
    Int_t nsrc;
    Float_t UT;
    Float_t delta_UT;

  public:
    EvtInfo();
    EvtInfo(const EvtInfo &orig);
    virtual ~EvtInfo() { Clear(); }
    EvtInfo &operator=(const EvtInfo &orig);

    void Clear(Option_t *option = "");
    Int_t GetEventID() const { return eventID; }
    Int_t GetRunID() const { return runID; }
    Int_t GetEventType() const { return eventType; }
    Int_t GetNSource() const { return nsrc; }
    Float_t GetUT() const { return UT; }
    Float_t GetDeltaUT() const { return delta_UT; }

    void SetEventID(Int_t id) { eventID = id; }
    void SetRunID(Int_t id) { runID = id; }
    void SetEventType(Int_t type) { eventType = type; }
    void SetNSource(Int_t nn) { nsrc = nn; }
    void SetUT(Float_t t) { UT = t; }
    void SetDeltaUT(Float_t dt) { delta_UT = dt; }

    ClassDef(EvtInfo, 3) // Track structure
};

#endif
