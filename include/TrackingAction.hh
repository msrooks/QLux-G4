#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "globals.hh"
#include "G4UserTrackingAction.hh"

class TrackingAction : public G4UserTrackingAction
{
 public:
  TrackingAction();
  ~TrackingAction(){};

  void PreUserTrackingAction(const G4Track*) override;
  void PostUserTrackingAction(const G4Track*) override;

 private:
};

#endif
