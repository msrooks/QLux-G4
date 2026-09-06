#ifndef EventMessenger_h
#define EventMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class EventAction;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

class EventMessenger : public G4UImessenger
{
 public:
  EventMessenger(EventAction*);
  ~EventMessenger();

  void SetNewValue(G4UIcommand*, G4String) override;

 private:
  EventAction* fEventAction;
  G4UIcmdWithAnInteger* fVerboseCmd;
  G4UIcmdWithABool* fForceDrawPhotonsCmd;
  G4UIcmdWithABool* fForceDrawNoPhotonsCmd;
  
  G4UIcmdWithAString* fPhotonFileNameCmd;
  G4UIcmdWithAString* fEDepFileNameCmd;
  G4UIcmdWithABool* fEDepOutputCmd;
  G4UIcmdWithAString* fSecondaryFileNameCmd;
  G4UIcmdWithABool* fSecondaryOutputCmd;
  
};

#endif
