#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "aSeHit.hh"
#include "Run.hh"
#include "Trajectory.hh"

#include "G4Event.hh"
#include "G4AutoLock.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include <fstream>
#include <cstdint>
#include <algorithm>
#include <filesystem>


namespace
{
  G4Mutex photonFileMutex = G4MUTEX_INITIALIZER;
}

EventAction::EventAction(const DetectorConstruction* det)
  : fDetector(det)
  , fHitCollID(-1)
  , fVerbose(0)
  , fPixelThreshold(1) 
  , fForcedrawphotons(false)
  , fForcenophotons(false)

{
  fEventMessenger = new EventMessenger(this);

  fHitCount                = 0;
  fPhotonCount_Scint       = 0;
  fAbsorptionCount         = 0;
  fBoundaryAbsorptionCount = 0;
  fConvPosSet = false;
  fPixelsAboveThreshold = 0;

  fPhotonEvents.reserve(50000);
}

EventAction::~EventAction()
{
  FlushPhotonData();
  delete fEventMessenger;
}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fHitCount                = 0;
  fPhotonCount_Scint       = 0;
  fAbsorptionCount         = 0;
  fBoundaryAbsorptionCount = 0;
  
  fConvPosSet = false;
  fPixelsAboveThreshold = 0;
  fInitialTrackLength = 0.0;

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if(fHitCollID < 0)
    fHitCollID = SDman->GetCollectionID("aSeHitCollection");
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EventAction::EndOfEventAction(const G4Event* anEvent)
{ 
  if(G4RunManager::GetRunManager()->GetCurrentEvent()->IsAborted()) {
    G4cout << "Warning: Event " << anEvent->GetEventID() << " was aborted. Skipping to next event." << G4endl;
    return;  // Skip processing this event without crashing the batch
  }

  G4TrajectoryContainer* trajectoryContainer =
    anEvent->GetTrajectoryContainer();

  G4int n_trajectories = 0;
  if(trajectoryContainer)
    n_trajectories = trajectoryContainer->entries();

  // extract the trajectories and draw them
  if(G4VVisManager::GetConcreteInstance())
  {
    for(G4int i = 0; i < n_trajectories; ++i)
    {
      Trajectory* trj =
        (Trajectory*) ((*(anEvent->GetTrajectoryContainer()))[i]);
      if(trj->GetParticleName() == "opticalphoton")
      {
        trj->SetForceDrawTrajectory(fForcedrawphotons);
        trj->SetForceNoDrawTrajectory(fForcenophotons);
      }
      trj->DrawTrajectory();
    }
  }

  aSeHitsCollection* hitCollection     = nullptr;
  G4HCofThisEvent* hitsCE         = anEvent->GetHCofThisEvent();

  // Get the hit collections
  if(hitsCE)
  {
    if(fHitCollID >= 0)
    {
      hitCollection = (aSeHitsCollection*) (hitsCE->GetHC(fHitCollID));
    }
  }

  if (hitCollection) {
    G4ThreeVector reconPos(0., 0., 0.);
    size_t hits = hitCollection->entries();
    
    // Gather information from all a-Se pixel hits
    for (size_t i = 0; i < hits; ++i) {
        fHitCount += (*hitCollection)[i]->GetPhotonCount();
        reconPos += (*hitCollection)[i]->GetPixelPos() * (*hitCollection)[i]->GetPhotonCount();
        
        if ((*hitCollection)[i]->GetPhotonCount() >= fPixelThreshold) {
            ++fPixelsAboveThreshold;
        } else {
            // If it wasn't above the threshold, turn it off
            (*hitCollection)[i]->SetDrawit(false);
        }
    }

    // Print detected photons only when event verbosity is enabled
    if (fVerbose > 0) {
      for (size_t i = 0; i < hits; ++i) {
        G4cout << "Pixel " << i << ": " << (*hitCollection)[i]->GetPhotonCount() << " photons detected" << G4endl;
      }
    }
  

    // Reconstruct the position of the hits
    if (fHitCount > 0) {
        reconPos /= fHitCount;
        if (fVerbose > 0) {
            G4cout << "\tReconstructed position of hits on a-Se array : " << reconPos / mm << G4endl;
        }
        fReconPos = reconPos;
    }
     
    // Draw all hits
    hitCollection->DrawAllHits();

  }

  PhotonEvent photonEvent;
  photonEvent.fileName = fPhotonFileName;
  photonEvent.eventID = anEvent->GetEventID();
  photonEvent.scintillationPhotons = fPhotonCount_Scint;

  if (hitCollection)
  {
    size_t hits = hitCollection->entries();

    for (size_t i = 0; i < hits; ++i)
    {
      G4int id = (*hitCollection)[i]->GetPixelNumber();

      if (id >= 0 && id < 9)
        photonEvent.pixelCounts[id] = (*hitCollection)[i]->GetPhotonCount();
    }
  }

  fPhotonEvents.push_back(photonEvent);

  
  if(fVerbose > 0)
  {
    // End of event output. later to be controlled by a verbose level
    G4cout << "\tNumber of photons detected by a-Se pixels in this event : " << fHitCount
           << G4endl;
    G4cout << "\tNumber of pixels above threshold(" << fPixelThreshold
           << ") : " << fPixelsAboveThreshold << G4endl;
    G4cout << "\tNumber of photons produced by scintillation in this event : "
           << fPhotonCount_Scint << G4endl;
    G4cout << "\tNumber of photons absorbed by GAr in this event : "
           << fAbsorptionCount << G4endl;
    G4cout << "\tNumber of photons absorbed at boundaries in "
           << "this event : " << fBoundaryAbsorptionCount << G4endl;
    G4cout << "Unaccounted for photons in this event : "
           << (fPhotonCount_Scint - fAbsorptionCount -
               fHitCount - fBoundaryAbsorptionCount)
           << G4endl;
    
  }

  // update the run statistics
  Run* run = static_cast<Run*>(
    G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  run->IncHitCount(fHitCount);
  run->IncPhotonCount_Scint(fPhotonCount_Scint);
  run->IncAbsorption(fAbsorptionCount);
  run->IncBoundaryAbsorption(fBoundaryAbsorptionCount);
  run->IncPixelsAboveThreshold(fPixelsAboveThreshold);

  // If we have set the flag to save 'special' events, save here
  if(fPhotonCount_Scint < fDetector->GetSaveThreshold())
  {
    G4RunManager::GetRunManager()->rndmSaveThisEvent();
  }
}


void EventAction::FlushPhotonData()
{
  if (fPhotonEvents.empty())
    return;

  std::sort(fPhotonEvents.begin(), fPhotonEvents.end(),
            [](const PhotonEvent& a, const PhotonEvent& b)
            {
              if (a.fileName != b.fileName)
                return a.fileName < b.fileName;
              return a.eventID < b.eventID;
            });

  G4AutoLock lock(&photonFileMutex);

  std::filesystem::create_directories("data");

  size_t first = 0;

  while (first < fPhotonEvents.size())
  {
    G4String fileName = fPhotonEvents[first].fileName;

    std::ofstream file(
        "data/" + fileName,
        std::ios::binary | std::ios::app);

    size_t i = first;

    while (i < fPhotonEvents.size() && fPhotonEvents[i].fileName == fileName)
    {
      const PhotonEvent& event = fPhotonEvents[i];

      std::int32_t eventID = static_cast<std::int32_t>(event.eventID);
      std::int32_t scintillationPhotons =
        static_cast<std::int32_t>(event.scintillationPhotons);

      file.write(reinterpret_cast<const char*>(&eventID), sizeof(eventID));
      file.write(reinterpret_cast<const char*>(&scintillationPhotons),
                 sizeof(scintillationPhotons));

      for (G4int id = 0; id < 9; ++id)
      {
        std::int32_t pixelCount =
          static_cast<std::int32_t>(event.pixelCounts[id]);

        file.write(reinterpret_cast<const char*>(&pixelCount),
                   sizeof(pixelCount));
      }

      ++i;
    }

    first = i;
  }

  fPhotonEvents.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......