#include "EventMessenger.hh"
#include "EventAction.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

EventMessenger::EventMessenger(EventAction* event)
  : fEventAction(event)
{
  fVerboseCmd = new G4UIcmdWithAnInteger("/QLux/eventVerbose", this);
  fVerboseCmd->SetGuidance("Set the verbosity of event data.");
  fVerboseCmd->SetParameterName("verbose", true);
  fVerboseCmd->SetDefaultValue(1);

  fPhotonFileNameCmd = new G4UIcmdWithAString("/QLux/PhotonFileName", this);
  fPhotonFileNameCmd->SetGuidance("Set photon output filename.");
  fPhotonFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fEDepFileNameCmd = new G4UIcmdWithAString("/QLux/EDepFileName", this);
  fEDepFileNameCmd->SetGuidance("Set energy-deposition output filename.");
  fEDepFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fEDepOutputCmd = new G4UIcmdWithABool("/QLux/EDepOutput", this);

  fEDepOutputCmd->SetGuidance("Enable or disable primary-alpha dE/dx output.");
  fEDepOutputCmd->SetDefaultValue(false);
  fEDepOutputCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSecondaryFileNameCmd = new G4UIcmdWithAString("/QLux/SecondaryFileName", this);
  fSecondaryFileNameCmd->SetGuidance("Set secondary-particle output filename.");
  fSecondaryFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSecondaryOutputCmd = new G4UIcmdWithABool("/QLux/SecondaryOutput", this);
  fSecondaryOutputCmd->SetGuidance("Enable or disable secondary-particle output.");
  fSecondaryOutputCmd->SetDefaultValue(false);
  fSecondaryOutputCmd->AvailableForStates(G4State_PreInit, G4State_Idle);


  fForceDrawPhotonsCmd = new G4UIcmdWithABool("/QLux/forceDrawPhotons", this);
  fForceDrawPhotonsCmd->SetGuidance("Force drawing of photons.");
  fForceDrawPhotonsCmd->SetGuidance(
    "(Higher priority than /QLux/forceDrawNoPhotons)");

  fForceDrawNoPhotonsCmd =
    new G4UIcmdWithABool("/QLux/forceDrawNoPhotons", this);
  fForceDrawNoPhotonsCmd->SetGuidance("Force no drawing of photons.");
  fForceDrawNoPhotonsCmd->SetGuidance(
    "(Lower priority than /QLux/forceDrawPhotons)");
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

EventMessenger::~EventMessenger()
{
  delete fVerboseCmd;
  delete fForceDrawPhotonsCmd;
  delete fForceDrawNoPhotonsCmd;
  delete fPhotonFileNameCmd;
  delete fEDepFileNameCmd;
  delete fEDepOutputCmd;
  delete fSecondaryFileNameCmd;
  delete fSecondaryOutputCmd;
}

//···QLux···✦···QLux···✦···QLux···✦···QLux···✦···QLux···

void EventMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == fVerboseCmd)
  {
    fEventAction->SetEventVerbose(fVerboseCmd->GetNewIntValue(newValue));
  }
  else if(command == fPhotonFileNameCmd)
  {
    fEventAction->SetPhotonFileName(newValue);
  }
  else if(command == fEDepFileNameCmd)
  {
    fEventAction->SetEDepFileName(newValue);
  }
  else if(command == fEDepOutputCmd)
  {
    fEventAction->SetEDepOutput(fEDepOutputCmd->GetNewBoolValue(newValue));
  }
  else if(command == fSecondaryFileNameCmd)
  {
    fEventAction->SetSecondaryFileName(newValue);
  }
  else if(command == fSecondaryOutputCmd)
  {
    fEventAction->SetSecondaryOutput(fSecondaryOutputCmd->GetNewBoolValue(newValue));
  }

  else if(command == fForceDrawPhotonsCmd)
  {
    fEventAction->SetForceDrawPhotons(fForceDrawPhotonsCmd->GetNewBoolValue(newValue));
  }
  else if(command == fForceDrawNoPhotonsCmd)
  {
    fEventAction->SetForceDrawNoPhotons(fForceDrawNoPhotonsCmd->GetNewBoolValue(newValue));
  }
}
