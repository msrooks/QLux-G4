#include "TrackingAction.hh"
#include "DetectorConstruction.hh"
#include "Trajectory.hh"
#include "UserTrackInformation.hh"
#include "G4OpticalPhoton.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

TrackingAction::TrackingAction() {}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  
  fpTrackingManager->SetTrajectory(new Trajectory(aTrack));

  fpTrackingManager->SetUserTrackInformation(new UserTrackInformation);
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  Trajectory* trajectory =
    (Trajectory*) fpTrackingManager->GimmeTrajectory();
  UserTrackInformation* trackInformation =
    (UserTrackInformation*) aTrack->GetUserInformation();

 if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
{
  if(trackInformation->GetTrackStatus() & hitASe)
    trajectory->SetDrawTrajectory(true);
}
else
{
  trajectory->SetDrawTrajectory(true);
}

  if(trackInformation->GetForceDrawTrajectory())
    trajectory->SetDrawTrajectory(true);
}


