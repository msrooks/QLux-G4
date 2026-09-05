#include "G4UserRunAction.hh"

#ifndef RunAction_h
#  define RunAction_h 1

class Run;
class G4Run;

class RunAction : public G4UserRunAction
{
 public:
  RunAction();
  ~RunAction();

  G4Run* GenerateRun() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

 private:
  Run* fRun;
};

#endif
