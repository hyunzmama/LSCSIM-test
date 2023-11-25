#include "MCObjs/EvtInfo.hh"

ClassImp(EvtInfo);

//______________________________________________________________________________
EvtInfo::EvtInfo() : TObject(), eventID(0), runID(0), eventType(0), UT(0), delta_UT(0) {}

//______________________________________________________________________________
EvtInfo::EvtInfo(const EvtInfo &ev)
    : TObject(ev), eventID(ev.eventID), runID(ev.runID), eventType(ev.delta_UT), UT(ev.UT),
      delta_UT(ev.delta_UT) {} // Copy a track object

//______________________________________________________________________________
EvtInfo &EvtInfo::operator=(const EvtInfo &ev) {
    // Copy a track

    TObject::operator=(ev);
    eventID          = ev.GetEventID();
    runID            = ev.GetRunID();
    UT               = ev.GetUT();
    delta_UT         = ev.GetDeltaUT();
    eventType        = ev.GetEventType();

    return *this;
}

//______________________________________________________________________________
void EvtInfo::Clear(Option_t * /*option*/) { TObject::Clear(); }

