#include "StackingAction.hh"
#include "EventAction.hh"
#include "G4OpticalPhoton.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

StackingAction::StackingAction(EventAction* ea)
  : fEventAction(ea)
{}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

StackingAction::~StackingAction() {}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(
  const G4Track* aTrack)
{
  
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    
    if(aTrack->GetParentID() > 0)
    {
      
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
        fEventAction->IncPhotonCount_Scint();
    }
  }
  return fUrgent;
}
