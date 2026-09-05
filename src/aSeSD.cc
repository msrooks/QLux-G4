#include "aSeSD.hh"

#include "DetectorConstruction.hh"
#include "aSeHit.hh"
#include "UserTrackInformation.hh"

#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

aSeSD::aSeSD(G4String name)
  : G4VSensitiveDetector(name)
  , fHitCollection(nullptr)
  , fPixelPositionsX(nullptr)
  , fPixelPositionsY(nullptr)
  , fPixelPositionsZ(nullptr)
  , fHitCID(-1)
{
  collectionName.insert("aSeHitCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

aSeSD::~aSeSD()
{
  delete fPixelPositionsX;
  delete fPixelPositionsY;
  delete fPixelPositionsZ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void aSeSD::SetPixelPositions(const std::vector<G4ThreeVector>& positions)
{
  for(size_t i = 0; i < positions.size(); ++i)
  {
    if(fPixelPositionsX)
      fPixelPositionsX->push_back(positions[i].x());
    if(fPixelPositionsY)
      fPixelPositionsY->push_back(positions[i].y());
    if(fPixelPositionsZ)
      fPixelPositionsZ->push_back(positions[i].z());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void aSeSD::Initialize(G4HCofThisEvent* hitsCE)
{
  fHitCollection =
    new aSeHitsCollection(SensitiveDetectorName, collectionName[0]);

  if(fHitCID < 0)
  {
    fHitCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitCollection);
  }
  hitsCE->AddHitsCollection(fHitCID, fHitCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool aSeSD::ProcessHits(G4Step*, G4TouchableHistory*) { return false; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Generates a hit and uses the postStepPoint's mother volume replica number
// PostStepPoint because the hit is generated manually when the photon is
// detected at the a-Se sensitive surface

G4bool aSeSD::ProcessHits_boundary(const G4Step* aStep, G4TouchableHistory*)
{
  // Only process optical photons
  G4Track* track = aStep->GetTrack();
  if(track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;
  
  // Continue with a-Se pixel hit processing
  G4int pixelNumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
  G4VPhysicalVolume* physVol = aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);
  
  size_t n = fHitCollection->entries();
  aSeHit* hit = nullptr;
  for(size_t i = 0; i < n; ++i)
  {
    if((*fHitCollection)[i]->GetPixelNumber() == pixelNumber)
    {
      hit = (*fHitCollection)[i];
      break;
    }
  }
  
  if(hit == nullptr)
  {  // Pixel not hit before in this event—create a new hit.
    hit = new aSeHit();
    hit->SetPixelNumber(pixelNumber);
    hit->SetPixelPhysVol(physVol);
    fHitCollection->insert(hit);
    hit->SetPixelPos((*fPixelPositionsX)[pixelNumber],
                   (*fPixelPositionsY)[pixelNumber],
                   (*fPixelPositionsZ)[pixelNumber]);
  }
  
  hit->IncPhotonCount();
  G4double hitTime = track->GetGlobalTime();
  hit->SetTime(hitTime);
  
  return true;
}

