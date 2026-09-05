#include "RunAction.hh"
#include "EventAction.hh"
#include "Run.hh"
#include "G4RunManager.hh"
#include "G4Timer.hh"

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

namespace
{
G4Timer runTimer;
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

RunAction::RunAction()
  : fRun(nullptr)
{

}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

RunAction::~RunAction()
{

}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

G4Run* RunAction::GenerateRun()
{
  fRun = new Run();

  return fRun;
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void RunAction::BeginOfRunAction(const G4Run*)
{
  if(isMaster)
  {
    runTimer.Start();
  }

}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void RunAction::EndOfRunAction(const G4Run* run)
{
  const G4UserEventAction* baseEventAction = G4RunManager::GetRunManager()->GetUserEventAction();

  if(baseEventAction)
  {
    EventAction* eventAction = const_cast<EventAction*>(static_cast<const EventAction*>(baseEventAction));
    eventAction->FlushPhotonData();
  }

  if(isMaster)
  {
    fRun->EndOfRun();
  }

  if(isMaster)
  {
    runTimer.Stop();

    G4double elapsed = runTimer.GetRealElapsed();
    G4int events = run->GetNumberOfEvent();

    G4cout << G4endl;
    G4cout << "========================================" << G4endl;
    G4cout << "Run time: " << elapsed << " s" << G4endl;
    G4cout << "Events:   " << events << G4endl;
    G4cout << "Rate:     " << events / elapsed << " events/s" << G4endl;
    G4cout << "========================================" << G4endl;
  }
}
