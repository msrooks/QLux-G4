#ifndef aSeHit_h
#define aSeHit_h 1

#include "G4Allocator.hh"
#include "G4LogicalVolume.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"
#include "G4VPhysicalVolume.hh"

class aSeHit : public G4VHit
{
 public:
  aSeHit();
  aSeHit(const aSeHit& right);
  ~aSeHit();

  const aSeHit& operator=(const aSeHit& right);
  G4bool operator==(const aSeHit& right) const;

  inline void* operator new(size_t);
  inline void operator delete(void* aHit);

  virtual void Draw();
  virtual void Print();

  inline void SetDrawit(G4bool b) { fDrawit = b; }
  inline G4bool GetDrawit() { return fDrawit; }

  inline void IncPhotonCount() { ++fPhotons; }
  inline G4int GetPhotonCount() { return fPhotons; }

  inline void SetPixelNumber(G4int n) { fPixelNumber = n; }
  inline G4int GetPixelNumber() { return fPixelNumber; }

  inline void SetPixelPhysVol(G4VPhysicalVolume* physVol)
  {
    this->fPhysVol = physVol;
  }
  inline G4VPhysicalVolume* GetPixelPhysVol() { return fPhysVol; }

  inline void SetPixelPos(G4double x, G4double y, G4double z)
  {
    fPos = G4ThreeVector(x, y, z);
  }

  inline G4ThreeVector GetPixelPos() { return fPos; }
  
  inline void SetTime(G4double t) { time = t; } // ~
  inline G4double GetTime() const { return time; } // ~


 private:
  G4int fPixelNumber;
  G4int fPhotons;
  G4ThreeVector fPos;
  G4VPhysicalVolume* fPhysVol;
  G4bool fDrawit;
  G4double time; // ~
};

typedef G4THitsCollection<aSeHit> aSeHitsCollection;

extern G4ThreadLocal G4Allocator<aSeHit>* aSeHitAllocator;

inline void* aSeHit::operator new(size_t)
{
  if(!aSeHitAllocator)
    aSeHitAllocator = new G4Allocator<aSeHit>;
  return (void*) aSeHitAllocator->MallocSingle();
}

inline void aSeHit::operator delete(void* aHit)
{
  aSeHitAllocator->FreeSingle((aSeHit*) aHit);
}

#endif
