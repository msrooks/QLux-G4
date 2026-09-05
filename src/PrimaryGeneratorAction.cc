#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4RandomTools.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  G4String particleName;
  fParticleGun->SetParticleDefinition(
    particleTable->FindParticle(particleName = "alpha"));

  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 37 * mm));
  fParticleGun->SetParticleEnergy(5.304 * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Generate a random direction uniformly over the -z hemisphere.
  G4double theta = std::acos(G4UniformRand());
  G4double phi = 2.0 * CLHEP::pi * G4UniformRand();

  G4double ux = std::sin(theta) * std::cos(phi);
  G4double uy = std::sin(theta) * std::sin(phi);
  G4double uz = -std::cos(theta);

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
