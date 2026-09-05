
#include "Run.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::Run()
  : G4Run()
{
  fHitCount = fHitCount2 = 0;
  fPhotonCount_Scint = fPhotonCount_Scint2 = 0;
  fAbsorptionCount = fAbsorptionCount2 = 0;
  fBoundaryAbsorptionCount = fBoundaryAbsorptionCount2 = 0;
  fPixelsAboveThreshold = fPixelsAboveThreshold2 = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::Merge(const G4Run* run)
{
  const Run* localRun = static_cast<const Run*>(run);

  fHitCount += localRun->fHitCount;
  fHitCount2 += localRun->fHitCount2;
  fPixelsAboveThreshold += localRun->fPixelsAboveThreshold;
  fPixelsAboveThreshold2 += localRun->fPixelsAboveThreshold2;
  fPhotonCount_Scint += localRun->fPhotonCount_Scint;
  fPhotonCount_Scint2 += localRun->fPhotonCount_Scint2;
  fAbsorptionCount += localRun->fAbsorptionCount;
  fAbsorptionCount2 += localRun->fAbsorptionCount2;
  fBoundaryAbsorptionCount += localRun->fBoundaryAbsorptionCount;
  fBoundaryAbsorptionCount2 += localRun->fBoundaryAbsorptionCount2;
  G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun()
{
  G4cout << "\n ======================== run summary ======================\n";

  G4int prec = G4cout.precision();

  G4double n_evt = (G4double) numberOfEvent;
  if (n_evt == 0.0)
    return;

  G4cout << "The run was " << numberOfEvent << " events." << G4endl;

  G4cout.precision(4);
  G4double hits     = G4double(fHitCount) / n_evt;
  G4double hits2    = G4double(fHitCount2) / n_evt;
  G4double rms_hits = hits2 - hits * hits;
  if(rms_hits > 0.)
    rms_hits = std::sqrt(rms_hits / n_evt);
  else
    rms_hits = 0.;
  G4cout << "Number of hits per event:\t " << hits << " +- " << rms_hits
         << G4endl;

  G4double hitsAbove     = G4double(fPixelsAboveThreshold) / n_evt;
  G4double hitsAbove2    = G4double(fPixelsAboveThreshold2) / n_evt;
  G4double rms_hitsAbove = hitsAbove2 - hitsAbove * hitsAbove;
  if(rms_hitsAbove > 0.)
    rms_hitsAbove = std::sqrt(rms_hitsAbove / n_evt);
  else
    rms_hitsAbove = 0.;

  G4cout << "Number of hits per event above threshold:\t " << hitsAbove
         << " +- " << rms_hitsAbove << G4endl;

  G4double scint     = G4double(fPhotonCount_Scint) / n_evt;
  G4double scint2    = G4double(fPhotonCount_Scint2) / n_evt;
  G4double rms_scint = scint2 - scint * scint;
  if(rms_scint > 0.)
    rms_scint = std::sqrt(rms_scint / n_evt);
  else
    rms_scint = 0.;

  G4cout << "Number of scintillation photons per event :\t " << scint/1000.0 << "k +- "
         << rms_scint/1000.0 << "k" << G4endl;

  G4double absorb     = G4double(fAbsorptionCount) / n_evt;
  G4double absorb2    = G4double(fAbsorptionCount2) / n_evt;
  G4double rms_absorb = absorb2 - absorb * absorb;
  if(rms_absorb > 0.)
    rms_absorb = std::sqrt(rms_absorb / n_evt);
  else
    rms_absorb = 0.;

  G4cout << "Number of absorbed photons per event :\t " << absorb << " +- "
         << rms_absorb << G4endl;

  G4double bdry     = G4double(fBoundaryAbsorptionCount) / n_evt;
  G4double bdry2    = G4double(fBoundaryAbsorptionCount2) / n_evt;
  G4double rms_bdry = bdry2 - bdry * bdry;
  if(rms_bdry > 0.)
    rms_bdry = std::sqrt(rms_bdry / n_evt);
  else
    rms_bdry = 0.;

  G4cout << "Number of photons absorbed at boundary per event:\t " << bdry/1000.0
         << "k +- " << rms_bdry/1000.0 << "k" << G4endl;

  G4double detPercent = 0.0;

  if (scint > 0.0)
    detPercent = hits * 100.0 / scint;

  G4cout << "Detection yield:\t " << detPercent << G4endl;

  G4cout << G4endl;
  G4cout.precision(prec);

  G4double unaccounted = scint  - (hits + absorb + bdry);
  G4cout << "Unaccounted photons per event:\t " << unaccounted << G4endl;
  
}
