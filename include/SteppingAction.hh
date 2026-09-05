
#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "globals.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UserSteppingAction.hh"

class EventAction;
class TrackingAction;

class SteppingAction : public G4UserSteppingAction
{
 public:
  SteppingAction(EventAction*);
  ~SteppingAction();

  void UserSteppingAction(const G4Step*) override;

 private:

  EventAction* fEventAction;

  G4OpBoundaryProcessStatus fExpectedNextStatus;
};

#endif
