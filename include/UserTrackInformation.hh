#include "G4VUserTrackInformation.hh"
#include "globals.hh"

#ifndef UserTrackInformation_h
#  define UserTrackInformation_h 1

enum TrackStatus
{
  active           = 1,
  hitASe           = 2,
  absorbed         = 4,
  boundaryAbsorbed = 8,
  inactive         = 14
};

class UserTrackInformation : public G4VUserTrackInformation
{
 public:
  UserTrackInformation();
  ~UserTrackInformation();

  void SetTrackStatusFlags(int s) { fStatus = s; }

  void AddTrackStatusFlag(int s);

  int GetTrackStatus() const { return fStatus; }

  void IncReflections() { ++fReflections; }
  G4int GetReflectionCount() const { return fReflections; }

  void SetForceDrawTrajectory(G4bool b) { fForcedraw = b; }
  G4bool GetForceDrawTrajectory() { return fForcedraw; }

  inline virtual void Print() const {};

 private:
  int fStatus;
  G4int fReflections;
  G4bool fForcedraw;
};

#endif
