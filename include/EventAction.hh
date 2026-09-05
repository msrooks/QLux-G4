#ifndef EventAction_h
#define EventAction_h 1

#include "EventMessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "globals.hh"
#include "G4UserEventAction.hh"
#include <array>
#include <vector>

class G4Event;

class EventAction : public G4UserEventAction
{
 public:
  EventAction();
  ~EventAction();

 public:
  void AddTrackLength(G4double stepLength) { fInitialTrackLength += stepLength; }
  G4double GetTrackLength() const { return fInitialTrackLength; }
  
  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;

  void SetEventVerbose(G4int v) { fVerbose = v; }

  void SetPixelThreshold(G4int t) { fPixelThreshold = t; }
  
  void SetPhotonFileName(const G4String& name) { fPhotonFileName = name; }
  void SetEDepFileName(const G4String& name) { fEDepFileName = name; }

  const G4String& GetPhotonFileName() const { return fPhotonFileName; }
  const G4String& GetEDepFileName() const { return fEDepFileName; }

  void FlushPhotonData();
  
  void SetForceDrawPhotons(G4bool b) { fForcedrawphotons = b; }
  void SetForceDrawNoPhotons(G4bool b) { fForcenophotons = b; }

  void IncPhotonCount_Scint() { ++fPhotonCount_Scint; }
  void IncAbsorption() { ++fAbsorptionCount; }
  void IncBoundaryAbsorption() { ++fBoundaryAbsorptionCount; }
  void IncHitCount(G4int i = 1) { fHitCount += i; }
    
 private:
  struct PhotonEvent
  {
    G4String fileName;
    G4int eventID;
    G4int scintillationPhotons;
    std::array<G4int, 9> pixelCounts{};
  };

  std::vector<PhotonEvent> fPhotonEvents;

  EventMessenger* fEventMessenger;  
  G4double fInitialTrackLength;
  
  G4int fHitCollID;

  G4int fVerbose;
  G4String fPhotonFileName = "photons.bin";
  G4String fEDepFileName = "dEdx.bin";
  G4int fPixelThreshold;
  G4bool fForcedrawphotons;
  G4bool fForcenophotons;

  G4int fHitCount;
  G4int fPhotonCount_Scint;
  G4int fAbsorptionCount;
  G4int fBoundaryAbsorptionCount;
  
  G4int fPixelsAboveThreshold;
};

#endif

