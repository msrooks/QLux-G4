#include "aSeHit.hh"

#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VVisManager.hh"

G4ThreadLocal G4Allocator<aSeHit>* aSeHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

aSeHit::aSeHit()
  : fPixelNumber(-1)
  , fPhotons(0)
  , fPhysVol(nullptr)
  , fDrawit(false)
  , time(0.0) // ~
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

aSeHit::~aSeHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

aSeHit::aSeHit(const aSeHit& right)
  : G4VHit()
{
  fPixelNumber = right.fPixelNumber;
  fPhotons   = right.fPhotons;
  fPhysVol   = right.fPhysVol;
  fDrawit    = right.fDrawit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const aSeHit& aSeHit::operator=(const aSeHit& right)
{
  fPixelNumber = right.fPixelNumber;
  fPhotons   = right.fPhotons;
  fPhysVol   = right.fPhysVol;
  fDrawit    = right.fDrawit;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool aSeHit::operator==(const aSeHit& right) const
{
  return (fPixelNumber == right.fPixelNumber);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void aSeHit::Draw()
{
  if(fDrawit && fPhysVol)
  {  // Redraw only the a-Se pixels that have hit counts > 0
    // Also need a physical volume to be able to draw anything
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
    {  // Make sure that the VisManager exists
      G4VisAttributes attribs(G4Colour(1., 0., 0.));
      attribs.SetForceSolid(true);
      G4RotationMatrix rot;
      if(fPhysVol->GetRotation())  // If a rotation is defined use it
        rot = *(fPhysVol->GetRotation());
      G4Transform3D trans(rot, fPhysVol->GetTranslation());  // Create transform
      pVVisManager->Draw(*fPhysVol, attribs, trans);         // Draw it
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void aSeHit::Print() {}
