
#include "Trajectory.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4Polyline.hh"
#include "G4Polymarker.hh"
#include "G4ThreeVector.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryPoint.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"

G4ThreadLocal G4Allocator<Trajectory>* TrajectoryAllocator = nullptr;

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

Trajectory::Trajectory()
  : G4Trajectory()
  , fDrawit(false)
  , fForceNoDraw(false)
  , fForceDraw(false)
{
  fParticleDefinition = nullptr;
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

Trajectory::Trajectory(const G4Track* aTrack)
  : G4Trajectory(aTrack)
  , fDrawit(false)
{
  fParticleDefinition = aTrack->GetDefinition();
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

Trajectory::Trajectory(Trajectory& right)
  : G4Trajectory(right)
  , fDrawit(right.fDrawit)
{
  fParticleDefinition = right.fParticleDefinition;
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

Trajectory::~Trajectory() {}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void Trajectory::DrawTrajectory() const
{
  // Taken from G4VTrajectory and modified to select colours based on particle
  // type and to selectively eliminate drawing of certain trajectories.

  if(!fForceDraw && (!fDrawit || fForceNoDraw))
    return;

  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(!pVVisManager)
    return;

  const G4double markerSize = 0.05;
  G4bool lineRequired       = true;
  G4bool markersRequired    = true;

  G4Polyline trajectoryLine;
  G4Polymarker stepPoints;
  G4Polymarker auxiliaryPoints;

  for(G4int i = 0; i < GetPointEntries(); ++i)
  {
    G4VTrajectoryPoint* aTrajectoryPoint = GetPoint(i);
    const std::vector<G4ThreeVector>* auxiliaries =
      aTrajectoryPoint->GetAuxiliaryPoints();
    if(auxiliaries)
    {
      for(size_t iAux = 0; iAux < auxiliaries->size(); ++iAux)
      {
        const G4ThreeVector pos((*auxiliaries)[iAux]);
        if(lineRequired)
        {
          trajectoryLine.push_back(pos);
        }
        if(markersRequired)
        {
          auxiliaryPoints.push_back(pos);
        }
      }
    }
    const G4ThreeVector pos(aTrajectoryPoint->GetPosition());
    if(lineRequired)
    {
      trajectoryLine.push_back(pos);
    }
    if(markersRequired)
    {
      stepPoints.push_back(pos);
    }
  }

  if(lineRequired)
  {
    G4Colour colour;
  
    if(fParticleDefinition == G4OpticalPhoton::OpticalPhotonDefinition())
    {      
      {  
        colour = G4Colour(0., 1., 0.);
      }
    }
    else 
      colour = G4Colour(0., 0., 1.);

    G4VisAttributes trajectoryLineAttribs(colour);
    trajectoryLine.SetVisAttributes(&trajectoryLineAttribs);
    pVVisManager->Draw(trajectoryLine);
  }
  
  if(markersRequired)
  {
    auxiliaryPoints.SetMarkerType(G4Polymarker::squares);
    auxiliaryPoints.SetScreenSize(markerSize);
    auxiliaryPoints.SetFillStyle(G4VMarker::filled);
    G4VisAttributes auxiliaryPointsAttribs(G4Colour(0., 1., 1.));  // Magenta
    auxiliaryPoints.SetVisAttributes(&auxiliaryPointsAttribs);
    pVVisManager->Draw(auxiliaryPoints);

    stepPoints.SetMarkerType(G4Polymarker::circles);
    stepPoints.SetScreenSize(markerSize);
    stepPoints.SetFillStyle(G4VMarker::filled);
    G4VisAttributes stepPointsAttribs(G4Colour(1., 1., 0.,0));  // Yellow
    stepPoints.SetVisAttributes(&stepPointsAttribs);
    pVVisManager->Draw(stepPoints);
  }
}
