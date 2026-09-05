#ifndef aSeSD_h
#define aSeSD_h 1

#include "aSeHit.hh"
#include "G4VSensitiveDetector.hh"
#include <vector>

class G4DataVector;
class G4HCofThisEvent;
class G4Step;

class aSeSD : public G4VSensitiveDetector
{
 public:
  aSeSD(G4String name);
  ~aSeSD();

  void Initialize(G4HCofThisEvent*) override;
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*) override;

  // A version of processHits active on boundary
  G4bool ProcessHits_boundary(const G4Step*, G4TouchableHistory*);

  // Initialize the arrays to store pixel positions
  inline void InitPixels()
  {
    if(fPixelPositionsX)
      delete fPixelPositionsX;
    if(fPixelPositionsY)
      delete fPixelPositionsY;
    if(fPixelPositionsZ)
      delete fPixelPositionsZ;
    fPixelPositionsX = new G4DataVector();
    fPixelPositionsY = new G4DataVector();
    fPixelPositionsZ = new G4DataVector();
  }

  // Store pixel positions
  void SetPixelPositions(const std::vector<G4ThreeVector>& positions);

 private:
  aSeHitsCollection* fHitCollection;

  G4DataVector* fPixelPositionsX;
  G4DataVector* fPixelPositionsY;
  G4DataVector* fPixelPositionsZ;

  G4int fHitCID;
};

#endif
