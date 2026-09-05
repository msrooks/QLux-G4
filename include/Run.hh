#ifndef Run_h
#define Run_h 1

#include "globals.hh"
#include "G4Run.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Run : public G4Run
{
 public:
  Run();
  ~Run();

  void IncPhotonCount_Scint(G4int count)
  {
    fPhotonCount_Scint += count;
    fPhotonCount_Scint2 += count * count;
  }
  
  void IncAbsorption(G4int count)
  {
    fAbsorptionCount += count;
    fAbsorptionCount2 += count * count;
  }
  void IncBoundaryAbsorption(G4int count)
  {
    fBoundaryAbsorptionCount += count;
    fBoundaryAbsorptionCount2 += count * count;
  }
  void IncHitCount(G4int count)
  {
    fHitCount += count;
    fHitCount2 += count * count;
  }
  void IncPixelsAboveThreshold(G4int count)
  {
    fPixelsAboveThreshold += count;
    fPixelsAboveThreshold2 += count * count;
  }

    
  void Merge(const G4Run* run) override;

  void EndOfRun();

 private:
  G4int fHitCount;
  G4int fHitCount2;
  G4int fPhotonCount_Scint;
  G4int fPhotonCount_Scint2;
  G4int fAbsorptionCount;
  G4int fAbsorptionCount2;
  G4int fBoundaryAbsorptionCount;
  G4int fBoundaryAbsorptionCount2;
  G4int fPixelsAboveThreshold;
  G4int fPixelsAboveThreshold2;

};

#endif  // Run_h
