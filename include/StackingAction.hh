#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"

class EventAction;

class StackingAction : public G4UserStackingAction
{
 public:
  StackingAction(EventAction*);
  ~StackingAction();

  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack) override;

 private:
  EventAction* fEventAction;
};

#endif
