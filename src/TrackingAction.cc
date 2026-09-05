#include "TrackingAction.hh"
#include "DetectorConstruction.hh"
#include "Trajectory.hh"
#include "UserTrackInformation.hh"
#include "G4OpticalPhoton.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Let this be up to the user via vis.mac
  //  fpTrackingManager->SetStoreTrajectory(true);

  // Use custom trajectory class
  fpTrackingManager->SetTrajectory(new Trajectory(aTrack));

  // This user track information is only relevant to the photons
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

  // Allow forced drawing if that flag is set.
  if(trackInformation->GetForceDrawTrajectory())
    trajectory->SetDrawTrajectory(true);
}


