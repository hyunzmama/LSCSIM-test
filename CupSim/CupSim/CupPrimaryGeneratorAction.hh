
#ifndef __CupPrimaryGeneratorAction_hh__
#define __CupPrimaryGeneratorAction_hh__ 1

#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh" // for user primary vertex gen.
#include "globals.hh"

#include "G4Version.hh"

class CupPrimaryGeneratorMessenger;
class CupDetectorConstruction;
class G4Event;
class G4Track;
class G4String;
class CupVPosGen;
class CupVVertexGen;

using namespace CLHEP;
class CupPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  public:
    CupPrimaryGeneratorAction(CupDetectorConstruction *argDC);
    ~CupPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event *argEvent); // generate primary particles

    void DeferTrackToLaterEvent(const G4Track *track); // postpone to later

    void NotifyTimeToNextStackedEvent(double t); // note time to stacked evt

    double GetUniversalTime() const { return myUniversalTime; }

    double GetUniversalTimeSincePriorEvent() { return myUniversalTimeSincePriorEvent; }

    int GetTypeOfCurrentEvent() const { return myTypeOfCurrentEvent; }

    double GetEventRate(int i) const { return myEventRate[i]; }
    void SetEventRate(int i, double r);

    int GetEventTriggerCondition(int iev) const { return myEventTriggerCondition[iev]; }
    void SetEventTriggerCondition(int iev, int itc);

    double GetEventWindow() const { return myEventWindow; }
    void SetEventWindow(double argEventWindow);

    double GetChainClip() const { return myChainClip; }
    void SetChainClip(double argChainClip);

    static G4String GetEventTypeName(int argEventType);

    static const char *GetPositionCodeName(int argPosCode) {
        return thePositionCodeNames[argPosCode];
    }
    static const char *GetVertexCodeName(int argVertexCode) {
        return theVertexCodeNames[argVertexCode];
    }
    static int GetPositionCodeForEventType(int argEventType) {
        return theEventGeneratorCodes[argEventType].poscode;
    }
    static int GetVertexCodeForEventType(int argEventType) {
        return theEventGeneratorCodes[argEventType].vertexcode;
    }

    static CupVVertexGen *GetVertexGenerator(int i) { return theVertexGenerators[i]; }
    static CupVPosGen *GetPositionGenerator(int i) { return thePositionGenerators[i]; }

    inline bool GetPileupStatus() const { return disablePileup; }
    inline void SetPileupStatus(bool a) { disablePileup = a; }

    static CupPrimaryGeneratorAction *GetTheCupPrimaryGeneratorAction() {
        return theCupPrimaryGeneratorAction;
    }

    enum {
        kGunEvtIndex   = 3,
        kGunPosIndex   = 9,
        kGunVtxIndex   = 17,
        kDelayEvtIndex = 51,
        kDelayPosIndex = 12,
        kDelayVtxIndex = 19
    };
    enum { theNumEventTypes = 52, theNumPosGenCodes = 13, theNumVertexGenCodes = 20 };
    enum {
        kGeneratorTriggerNormal     = 0,
        kGeneratorTriggerPileupOnly = 1,
        kGeneratorTriggerDelay      = 2
    };

  private:
    CupDetectorConstruction *myDetector;
    CupPrimaryGeneratorMessenger *myMessenger;

    double myUniversalTime;
    double myUniversalTimeSincePriorEvent;
    int myTypeOfCurrentEvent;
    double myEventWindow;
    double myChainClip;
    double myEventRate[theNumEventTypes];
    int myEventTriggerCondition[theNumEventTypes];
    double myTimeToNextEvent[theNumEventTypes];
    bool disablePileup;

#if G4VERSION_NUMBER >= 1000
    static G4ThreadLocal CupVVertexGen *theVertexGenerators[theNumVertexGenCodes];
    static G4ThreadLocal CupVPosGen *thePositionGenerators[theNumPosGenCodes];
    static G4ThreadLocal CupPrimaryGeneratorAction *theCupPrimaryGeneratorAction;
#else
    static CupVVertexGen *theVertexGenerators[theNumVertexGenCodes];
    static CupVPosGen *thePositionGenerators[theNumPosGenCodes];
    static CupPrimaryGeneratorAction *theCupPrimaryGeneratorAction;
#endif

    static const char *theVertexCodeNames[theNumVertexGenCodes];
    static const char *thePositionCodeNames[theNumPosGenCodes];
    typedef struct codepair_s {
        int poscode, vertexcode;
    } codepair_t;
    static codepair_t theEventGeneratorCodes[theNumEventTypes];
};

#endif
