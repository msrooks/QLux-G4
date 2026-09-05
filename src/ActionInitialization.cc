#include "ActionInitialization.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

ActionInitialization::ActionInitialization()
  : G4VUserActionInitialization()
{}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

ActionInitialization::~ActionInitialization() {}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction());
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction());

  EventAction* eventAction = new EventAction();
  SetUserAction(eventAction);
  SetUserAction(new StackingAction(eventAction));

  SetUserAction(new RunAction());
  SetUserAction(new TrackingAction());
  SetUserAction(new SteppingAction(eventAction));
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···
