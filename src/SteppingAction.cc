#include "SteppingAction.hh"
#include "EventAction.hh"
#include "aSeSD.hh"
#include "Trajectory.hh"
#include "UserTrackInformation.hh"
#include "G4AutoLock.hh"
#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"

#include <cstdint>
#include <fstream>
#include <vector>
#include <filesystem>

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

namespace
{
struct dEdxRow
{
  std::int32_t eventID;
  float distance_mm;
  float step_mm;
  float edep_MeV;
  float dEdx_MeV_per_mm;
  float x_mm;
  float y_mm;
  float z_mm;
};

G4Mutex dEdxFileMutex = G4MUTEX_INITIALIZER;

thread_local std::vector<dEdxRow> dEdxBuffer;

const std::size_t dEdxBufferSize = 100000;

void FlushdEdxBuffer(const G4String& fileName)
{
  if(dEdxBuffer.empty())
    return;

  G4AutoLock lock(&dEdxFileMutex);

  std::filesystem::create_directories("data");
  std::ofstream file("data/" + fileName, std::ios::binary | std::ios::app);

  file.write(reinterpret_cast<const char*>(dEdxBuffer.data()),
             dEdxBuffer.size() * sizeof(dEdxRow));

  dEdxBuffer.clear();
}
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

SteppingAction::SteppingAction(EventAction* ea)
  : fEventAction(ea)
{
  fExpectedNextStatus = Undefined;
  dEdxBuffer.reserve(dEdxBufferSize);
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

SteppingAction::~SteppingAction()
{
  FlushdEdxBuffer(fEventAction->GetEDepFileName());

}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void SteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track* theTrack = theStep->GetTrack();
  
  if(theTrack->GetCurrentStepNumber() == 1)
    fExpectedNextStatus = Undefined;

  UserTrackInformation* trackInformation = (UserTrackInformation*) theTrack->GetUserInformation();

  G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;
   

  if(theTrack->GetParentID() == 0)
  {
    G4double stepLength = theStep->GetStepLength();
    G4double edep = theStep->GetTotalEnergyDeposit();

    fEventAction->AddTrackLength(stepLength);

    if(stepLength > 0.0)
    {
      G4double distance = fEventAction->GetTrackLength() - 0.5 * stepLength;
      G4double dEdx = edep / stepLength;

      G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

      G4ThreeVector position = 0.5 * (theStep->GetPreStepPoint()->GetPosition() + theStep->GetPostStepPoint()->GetPosition());

      dEdxRow row;

      row.eventID = static_cast<std::int32_t>(eventID);
      row.distance_mm = static_cast<float>(distance / mm);
      row.step_mm = static_cast<float>(stepLength / mm);
      row.edep_MeV = static_cast<float>(edep / MeV);
      row.dEdx_MeV_per_mm = static_cast<float>(dEdx / (MeV / mm));
      row.x_mm = static_cast<float>(position.x() / mm);
      row.y_mm = static_cast<float>(position.y() / mm);
      row.z_mm = static_cast<float>(position.z() / mm);

      dEdxBuffer.push_back(row);

      if(dEdxBuffer.size() >= dEdxBufferSize)
        FlushdEdxBuffer(fEventAction->GetEDepFileName());
    }
  }

  if(!boundary)
  {
    G4ProcessManager* pm = theStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    G4ProcessVector* pv = pm->GetProcessList();

    for(G4int i = 0; i < nprocesses; ++i)
    {
      if((*pv)[i]->GetProcessName() == "OpBoundary")
      {
        boundary = (G4OpBoundaryProcess*) (*pv)[i];
        break;
      }
    }
  }


  if(!thePostPV)
  {
    fExpectedNextStatus = Undefined;
    return;
  }

  if(theTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    if(thePostPV->GetName() == "expHall")
      theTrack->SetTrackStatus(fStopAndKill);

    const G4VProcess* processDefinedStep = thePostPoint->GetProcessDefinedStep();

    if(processDefinedStep && processDefinedStep->GetProcessName() == "OpAbsorption")
    {
      fEventAction->IncAbsorption();
      trackInformation->AddTrackStatusFlag(absorbed);
    }

    boundaryStatus = boundary->GetStatus();

    if(thePostPoint->GetStepStatus() == fGeomBoundary)
    {
      
      switch(boundaryStatus)
      {
        case Absorption:
          trackInformation->AddTrackStatusFlag(boundaryAbsorbed);
          fEventAction->IncBoundaryAbsorption();
          break;

        case Detection:
        {
          G4SDManager* SDman = G4SDManager::GetSDMpointer();
          G4String sdName = "/QLux/aSeSD";
          aSeSD* sensorSD = (aSeSD*) SDman->FindSensitiveDetector(sdName);

          if(sensorSD)
            sensorSD->ProcessHits_boundary(theStep, nullptr);

          trackInformation->AddTrackStatusFlag(hitASe);
          break;
        }

        case FresnelReflection:
        case TotalInternalReflection:
        case LambertianReflection:
        case LobeReflection:
        case SpikeReflection:
        case BackScattering:
          trackInformation->IncReflections();
          fExpectedNextStatus = StepTooSmall;
          break;

        default:
          break;
      }

    }
  }
}