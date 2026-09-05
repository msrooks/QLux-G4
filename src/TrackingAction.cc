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

  // For optical photons, draw them only if they hit an a-Se pixel.
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    const G4VProcess* creator = aTrack->GetCreatorProcess();
    // For WLS photons, draw only if the hitASe flag is set.
    if(creator && creator->GetProcessName() == "OpWLS")
    {
      if(trackInformation->GetTrackStatus() & hitASe)
      {
         trajectory->WLS();
         trajectory->SetDrawTrajectory(true);
      }
    }
    // For non-WLS optical photons, also draw only if they hit an a-Se pixel.
    else
    {
      if(trackInformation->GetTrackStatus() & hitASe)
        trajectory->SetDrawTrajectory(true);
    }
  }
  // Draw all non-optical photon trajectories.
  else
  {
    trajectory->SetDrawTrajectory(true);
  }

  // Allow forced drawing if that flag is set.
  if(trackInformation->GetForceDrawTrajectory())
    trajectory->SetDrawTrajectory(true);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  Trajectory* trajectory =
    (Trajectory*) fpTrackingManager->GimmeTrajectory();
  UserTrackInformation* trackInformation =
    (UserTrackInformation*) aTrack->GetUserInformation();

  // Let's choose to draw only the photons that hit the sphere and an a-Se pixel
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    const G4VProcess* creator = aTrack->GetCreatorProcess();
    if(creator && creator->GetProcessName() == "OpWLS")
    {
      trajectory->WLS();
      trajectory->SetDrawTrajectory(true);
    }
   
    else
    {
      if(trackInformation->GetTrackStatus() & hitASe)
        trajectory->SetDrawTrajectory(true);
    }
  }
  // draw all other (not optical photon) trajectories
  else
    trajectory->SetDrawTrajectory(true);

  if(trackInformation->GetForceDrawTrajectory())
    trajectory->SetDrawTrajectory(true);
}
*/