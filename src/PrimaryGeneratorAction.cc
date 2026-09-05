#include "PrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include "G4RandomTools.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  //G4double a = 6.35 * mm;
  G4int n_particle = 1;
  fParticleGun     = new G4ParticleGun(n_particle);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  G4String particleName;
  fParticleGun->SetParticleDefinition(
    particleTable->FindParticle(particleName = "alpha"));
  
  // Field Ring Z-Positions: 5.65, 15.912, 26.174, 36.436, 46.698, 56.96, 67.222, 77.484 (mm)
  
  
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 40*mm));
  //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
    
  fParticleGun->SetParticleEnergy(5.304 * MeV);  // 5.40753
 
}
/*
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // Generate random direction in the -z and -y hemisphere
    G4double theta = std::acos(G4UniformRand());  // Restricts theta to (0, pi/2), keeping it downward
    G4double phi = (3.0 / 2.0) * CLHEP::pi + (CLHEP::pi / 2.0) * G4UniformRand(); 
    // Restricts phi between 270° and 360° (3π/2 to 2π) to keep movement in -z and -y

    G4double ux = std::sin(theta) * std::cos(phi);
    G4double uy = -std::abs(std::sin(theta) * std::sin(phi)); // Ensures movement is only in -y
    G4double uz = -std::abs(std::cos(theta)); // Ensures movement is always in -z

    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));

    fParticleGun->GeneratePrimaryVertex(anEvent);
}
*/


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Generate random direction in -z hemisphere
  G4double theta = std::acos(G4UniformRand());  // Restricts theta to (0, pi/2)
  G4double phi = 2.0 * CLHEP::pi * G4UniformRand(); // Uniform azimuthal distribution

  G4double ux = std::sin(theta) * std::cos(phi);
  G4double uy = std::sin(theta) * std::sin(phi);
  G4double uz = -std::cos(theta); // Always in the -z direction

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
//{
//  fParticleGun->GeneratePrimaryVertex(anEvent);
//}

/*
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Generate random isotropic direction
  G4double theta = std::acos(2.0 * G4UniformRand() - 1.0); // Uniformly distributed in cos(theta)
  G4double phi = 2.0 * CLHEP::pi * G4UniformRand();        // Uniformly distributed in (0, 2pi)

  G4double ux = std::sin(theta) * std::cos(phi);
  G4double uy = std::sin(theta) * std::sin(phi);
  G4double uz = std::cos(theta);

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
*/
