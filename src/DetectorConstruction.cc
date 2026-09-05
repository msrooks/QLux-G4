#include "DetectorConstruction.hh"
#include "MainVolume.hh"
#include "aSeSD.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4GeometryManager.hh"
#include "G4GenericMessenger.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4OpticalSurface.hh"
#include "G4PhysicalConstants.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UImanager.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4Scintillation.hh"
#include "G4OpWLS.hh"


DetectorConstruction::DetectorConstruction()
  : fGAr_mt(nullptr)
{
  fworld  = nullptr;
  fworld_logical  = nullptr;
  fworld_physical = nullptr;

  fH = fC = fN = fO = fSi = fAl = fMg = fFe = nullptr;
  fGAr = fVacuum = fAir = fGlass = ftpb = fAl_6061 = fg10 = fpeek = nullptr;
  fBr = fCa = fNa = fTi = nullptr;
  fSiO2 = fAl2O3 = fFe2O3 = fCaO = fMgO = fNa2O = fTiO2 = nullptr;
  ffibrous_glass = fepoxy_resin = fFR4 = nullptr; 

  fSaveThreshold = 0;
  fBigQL = false;
  fMainVolume = nullptr;

  fMessenger = new G4GenericMessenger(this, "/QLux/", "QLux detector control");
  auto& geometryCmd = fMessenger->DeclareMethod("Geometry", &DetectorConstruction::SetGeometry,
                                                "Select QLux geometry: Little or Big.");
  geometryCmd.SetParameterName("geometry", false);
  geometryCmd.SetCandidates("Little Big");
  
  DefineMaterials();
}

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
  if(fMainVolume)
  {
    delete fMainVolume;
  }
  delete fGAr_mt;
}

void DetectorConstruction::SetGeometry(const G4String& geometry)
{
  fBigQL = (geometry == "Big");
}

void DetectorConstruction::DefineMaterials()
{
  // Define Elements
  fH  = new G4Element("Hydrogen",  "H",  1.,  1.01*g/mole );
  fC  = new G4Element("Carbon",    "C",  6.,  12.01*g/mole);
  fN  = new G4Element("Nitrogen",  "N",  7.,  14.01*g/mole);
  fO  = new G4Element("Oxygen",    "O",  8.,  16.00*g/mole);
  fSi = new G4Element("Silicon",   "Si", 14., 28.09*g/mole);
  fAl = new G4Element("Aluminum",  "Al", 13., 26.98*g/mole);
  fMg = new G4Element("Magnesium", "Mg", 12., 24.30*g/mole);
  fFe = new G4Element("Iron",      "Fe", 26., 55.85*g/mole);
  fBr = new G4Element("Bromine",   "Br", 35., 79.90*g/mole);
  fCa = new G4Element("Calcium",   "Ca", 20., 40.08*g/mole);
  fNa = new G4Element("Sodium",    "Na", 11., 22.99*g/mole);
  fTi = new G4Element("Titanium",  "Ti", 22., 47.87*g/mole);
  
  
  G4double temperature = 298 * kelvin;
  G4double pressure = 1 * bar;  
  G4double density = (1.784e-3 * (pressure / bar)) * (g / cm3);  
  
  fGAr =           new G4Material("GAr",           18., 39.948 * g/mole, density, kStateGas, temperature);
  fVacuum =        new G4Material("Vacuum",        1., 1.01*g/mole, universe_mean_density, kStateGas, 0.1*kelvin, 1.e-19*pascal);
  fSiO2 =          new G4Material("SiO2",          2.200*g/cm3, 2);  fSiO2->AddElement(fSi, 1); fSiO2->AddElement(fO, 2);
  fAl2O3 =         new G4Material("Al2O3",         3.950*g/cm3, 2);  fAl2O3->AddElement(fAl, 2); fAl2O3->AddElement(fO, 3);
  fFe2O3 =         new G4Material("Fe2O3",         5.240*g/cm3, 2);  fFe2O3->AddElement(fFe, 2); fFe2O3->AddElement(fO, 3);
  fCaO =           new G4Material("CaO",           3.350*g/cm3, 2);  fCaO->AddElement(fCa, 1); fCaO->AddElement(fO, 1);
  fMgO =           new G4Material("MgO",           3.580*g/cm3, 2);  fMgO->AddElement(fMg, 1); fMgO->AddElement(fO, 1);
  fNa2O =          new G4Material("Na2O",          2.270*g/cm3, 2);  fNa2O->AddElement(fNa, 2); fNa2O->AddElement(fO, 1);
  fTiO2 =          new G4Material("TiO2",          4.230*g/cm3, 2);  fTiO2->AddElement(fTi, 1); fTiO2->AddElement(fO, 2);
  fAir =           new G4Material("Air",           0.001*g/cm3, 2);  fAir->AddElement(fN, 70*perCent); fAir->AddElement(fO, 30*perCent);
  fGlass =         new G4Material("Glass",         1.030*g/cm3, 2);  fGlass->AddElement(fC, 91.533*perCent); fGlass->AddElement(fH, 8.467*perCent);
  ftpb =           new G4Material("TPB",           1.080*g/cm3, 2);  ftpb->AddElement (fC, 28); ftpb->AddElement (fH, 22);
  fAl_6061 =       new G4Material("Al_6061",       2.700*g/cm3, 4);  fAl_6061->AddElement(fAl, 0.980); fAl_6061->AddElement(fMg, 0.010); fAl_6061->AddElement(fSi, 0.006); fAl_6061->AddElement(fFe, 0.004);
  fg10 =           new G4Material("G10",           1.700*g/cm3, 4);  fg10->AddElement(fSi, 1); fg10->AddElement(fO , 2); fg10->AddElement(fC , 3); fg10->AddElement(fH , 3);
  fpeek =          new G4Material("Peek",          1.310*g/cm3, 3);  fpeek->AddElement(fC, 19); fpeek->AddElement(fH, 12); fpeek->AddElement(fO, 3);  
  ffibrous_glass = new G4Material("fibrous_glass", 2.744*g/cm3, 7); ffibrous_glass->AddMaterial(fSiO2, 0.600); ffibrous_glass->AddMaterial(fAl2O3, 0.118); ffibrous_glass->AddMaterial(fFe2O3, 0.001); ffibrous_glass->AddMaterial(fCaO, 0.224); ffibrous_glass->AddMaterial(fMgO, 0.034); ffibrous_glass->AddMaterial(fNa2O, 0.010); ffibrous_glass->AddMaterial(fTiO2, 0.013);   
  fepoxy_resin =   new G4Material("epoxy_resin",   1.125*g/cm3, 4); fepoxy_resin->AddElement(fC, 38); fepoxy_resin->AddElement(fH, 40); fepoxy_resin->AddElement(fO, 6); fepoxy_resin->AddElement(fBr, 4); 
  fFR4 =           new G4Material("FR4",           1.983*g/cm3, 2); fFR4->AddMaterial(fepoxy_resin, 0.47); fFR4->AddMaterial(ffibrous_glass, 0.53);
  
  G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Au");
  G4NistManager::Instance()->BuildMaterialWithNewDensity("HDPE","G4_POLYETHYLENE",0.95*g/cm3);
  
  // GAr Properties
  std::vector<G4double> GArAbsLenEnergy = {4 * eV, 5 * eV, 6 * eV, 7 * eV, 8 * eV, 9 * eV, 10 * eV, 11 * eV};
  std::vector<G4double> GAr_ABSL = {1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm};

  std::vector<G4double> GArRayleighEnergy = {1.18626 * eV, 1.68626 * eV, 2.18626 * eV, 2.68626 * eV, 3.18626 * eV, 3.68626 * eV, 4.18626 * eV, 4.68626 * eV, 5.18626 * eV, 5.68626 * eV, 6.18626 * eV, 6.68626 * eV, 7.18626 * eV, 7.68626 * eV, 8.18626 * eV, 8.68626 * eV, 9.18626 * eV, 9.68626 * eV, 10.1863 * eV, 10.6863 * eV, 11.1863 * eV};
  std::vector<G4double> GArRayleighSpectrum = {1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm, 1.e9 * cm};

  
  std::vector<G4double> GAr_ENERGY = { 
    7.9*eV, 8.3*eV, 8.6*eV, 8.8*eV, 8.9*eV, 8.9*eV, 9.0*eV, 9.0*eV, 
    9.1*eV, 9.1*eV, 9.2*eV, 9.2*eV, 9.3*eV, 9.3*eV, 9.3*eV, 9.3*eV, 
    9.4*eV, 9.4*eV, 9.4*eV, 9.4*eV, 9.4*eV, 9.5*eV, 9.5*eV, 9.5*eV, 
    9.5*eV, 9.5*eV, 9.6*eV, 9.6*eV, 9.6*eV, 9.7*eV, 9.7*eV, 9.8*eV, 
    9.8*eV, 9.9*eV, 9.9*eV, 9.9*eV, 9.9*eV, 9.9*eV, 10.0*eV, 10.0*eV, 
    10.0*eV, 10.0*eV, 10.1*eV, 10.1*eV, 10.1*eV, 10.1*eV, 10.1*eV, 
    10.2*eV, 10.2*eV, 10.2*eV, 10.3*eV, 10.3*eV, 10.4*eV, 10.5*eV, 
    10.5*eV, 10.6*eV, 10.7*eV, 10.8*eV, 10.9*eV 
  };

  std::vector<G4double> GAr_Spectrum = {
    0.0, 0.12, 2.7, 2.8, 2.9, 3.3, 4.3, 5.9, 
    8.3, 12.0, 16.2, 20.5, 24.9, 29.0, 33.1, 
    37.3, 41.3, 45.9, 50.3, 54.4, 59.1, 63.4, 
    68.0, 72.6, 76.9, 80.9, 84.9, 89.1, 93.0, 
    97.3, 99.5, 98.0, 93.7, 89.6, 85.1, 80.9, 
    76.6, 72.0, 67.2, 61.2, 55.8, 51.6, 46.7, 
    42.1, 37.7, 32.3, 27.5, 22.8, 18.9, 14.9, 
    10.8, 7.0, 4.4, 3.0, 1.9, 1.1, 0.3, 0.1, 0.0};


  std::vector<G4double> GArIndexEnergy = {1.18626 * eV, 1.68626 * eV, 2.18626 * eV, 2.68626 * eV, 3.18626 * eV, 3.68626 * eV, 4.18626 * eV, 4.68626 * eV, 5.18626 * eV, 5.68626 * eV, 6.18626 * eV, 6.68626 * eV, 7.18626 * eV, 7.68626 * eV, 8.18626 * eV, 8.68626 * eV, 9.18626 * eV, 9.68626 * eV, 10.1863 * eV, 10.6863 * eV, 11.1863 * eV}; 
	std::vector<G4double> GArIndexSpectrum = {1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028, 1.00028};

  std::vector<G4double> GArScint_ENERGY = {100*keV, 1*MeV, 2*MeV, 3*MeV, 4*MeV, 5*MeV, 6*MeV, 7*MeV, 8*MeV, 10*MeV};
  std::vector<G4double> GArmip_Photons = {2400, 24000, 48000, 72000, 96000, 120000, 144000, 168000, 192000, 240000};
  
  std::vector<G4double> GAralpha_Photons;

  if (pressure == 1 * bar) {
      GAralpha_Photons = {1.869e3, 18.693e3, 37.386e3, 56.079e3, 74.772e3, 93.465e3, 112.158e3, 130.851e3, 149.554e3, 186.93e3};
  } else if (pressure == 2 * bar) {
      GAralpha_Photons = {1.899e3, 18.994e3, 37.988e3, 56.981e3, 75.975e3, 94.969e3, 113.963e3, 132.957e3, 151.950e3, 189.938e3};
  } else if (pressure == 3 * bar) {
      GAralpha_Photons = {1.929e3, 19.295e3, 38.589e3, 57.884e3, 77.178e3, 96.473e3, 115.767e3, 135.062e3, 154.357e3, 192.946e3};
  } else if (pressure == 4 * bar) {
      GAralpha_Photons = {1.960e3, 19.595e3, 39.191e3, 58.786e3, 78.381e3, 97.977e3, 117.572e3, 137.167e3, 156.763e3, 195.953e3};
  } else if (pressure == 5 * bar) {
      GAralpha_Photons = {1.990e3, 19.896e3, 39.792e3, 59.688e3, 79.584e3, 99.480e3, 119.377e3, 139.273e3, 159.169e3, 198.961e3};
  } else {
      G4cerr << "Warning: Pressure not in range (1-5 bar). Defaulting to 1 bar." << G4endl;
      GAralpha_Photons = {1.869e3, 18.693e3, 37.386e3, 56.079e3, 74.772e3, 93.465e3, 112.158e3, 130.851e3, 149.554e3, 186.93e3};
  }

  fGAr_mt = new G4MaterialPropertiesTable();

  fGAr_mt->AddProperty("ELECTRONSCINTILLATIONYIELD", GArScint_ENERGY, GArmip_Photons);
  fGAr_mt->AddProperty("SCINTILLATIONCOMPONENT1", GAr_ENERGY, GAr_Spectrum);
  fGAr_mt->AddProperty("SCINTILLATIONCOMPONENT2", GAr_ENERGY, GAr_Spectrum);
  fGAr_mt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 5. * ns);
  fGAr_mt->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 1260. * ns);
  
  fGAr_mt->AddConstProperty("SCINTILLATIONYIELD1", 0.2307692);
  fGAr_mt->AddConstProperty("SCINTILLATIONYIELD2", 0.7692307); 

  fGAr_mt->AddProperty("RINDEX", GArIndexEnergy, GArIndexSpectrum);
  fGAr_mt->AddProperty("ABSLENGTH", GArAbsLenEnergy, GAr_ABSL);
  
  fGAr_mt->AddProperty("ALPHASCINTILLATIONYIELD", GArScint_ENERGY, GAralpha_Photons); 
  fGAr_mt->AddConstProperty("ALPHASCINTILLATIONYIELD1", 0.56);
  fGAr_mt->AddConstProperty("ALPHASCINTILLATIONYIELD2", 0.44); 

  fGAr_mt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  fGAr_mt->AddProperty("RAYLEIGH", GArRayleighEnergy, GArRayleighSpectrum);
  
  fGAr->SetMaterialPropertiesTable(fGAr_mt);
  fGAr->GetIonisation()->SetBirksConstant(0);  

  std::vector<G4double> ephoton_wls = { 2 * eV, 10.33 * eV };
  std::vector<G4double> fake_RIND  = {1.00028, 1.00028};
  std::vector<G4double> fake_ABSL = {1000*cm, 1000*cm};
  
  G4MaterialPropertiesTable* glass_mt   = new G4MaterialPropertiesTable();
  glass_mt->AddProperty("ABSLENGTH", ephoton_wls, fake_ABSL);
  glass_mt->AddProperty("RINDEX", ephoton_wls,fake_RIND);
  fGlass->SetMaterialPropertiesTable(glass_mt);

  G4MaterialPropertiesTable* vacuum_mt = new G4MaterialPropertiesTable();
  vacuum_mt->AddProperty("RINDEX", "Air");
  fVacuum->SetMaterialPropertiesTable(vacuum_mt);
  fAir->SetMaterialPropertiesTable(vacuum_mt); 

  // https://link.springer.com/article/10.1140/epjc/s10052-018-5807-z
  std::vector<G4double> TpbEmmisionEnergies = {
    2.1*eV, 2.1*eV, 2.2*eV, 2.2*eV, 2.2*eV, 2.2*eV, 2.2*eV, 2.2*eV, 2.2*eV, 2.2*eV, 
    2.2*eV, 2.2*eV, 2.2*eV, 2.3*eV, 2.3*eV, 2.3*eV, 2.3*eV, 2.3*eV, 2.3*eV, 2.3*eV, 
    2.3*eV, 2.3*eV, 2.3*eV, 2.3*eV, 2.4*eV, 2.4*eV, 2.4*eV, 2.4*eV, 2.4*eV, 2.4*eV, 
    2.4*eV, 2.4*eV, 2.5*eV, 2.5*eV, 2.5*eV, 2.5*eV, 2.5*eV, 2.5*eV, 2.5*eV, 2.5*eV, 
    2.6*eV, 2.6*eV, 2.6*eV, 2.6*eV, 2.6*eV, 2.6*eV, 2.6*eV, 2.7*eV, 2.7*eV, 2.7*eV, 
    2.7*eV, 2.7*eV, 2.7*eV, 2.8*eV, 2.8*eV, 2.8*eV, 2.8*eV, 2.9*eV, 2.9*eV, 2.9*eV, 
    2.9*eV, 3.0*eV, 3.0*eV, 3.0*eV, 3.0*eV, 3.0*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 
    3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 3.1*eV, 
    3.1*eV, 3.1*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 
    3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 3.2*eV, 
    3.2*eV, 3.2*eV, 3.2*eV};

  
  std::vector<G4double> TpbEmmisionSpectrum = {
    0.0, 0.0, 0.0, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.002, 0.002, 
    0.002, 0.003, 0.003, 0.003, 0.004, 0.004, 0.005, 0.005, 0.006, 0.007, 0.008, 0.01, 
    0.011, 0.013, 0.015, 0.017, 0.02, 0.023, 0.026, 0.03, 0.034, 0.04, 0.046, 0.054, 0.062, 
    0.071, 0.082, 0.094, 0.11, 0.125, 0.143, 0.163, 0.186, 0.213, 0.246, 0.284, 0.324, 0.377, 
    0.423, 0.488, 0.541, 0.603, 0.664, 0.722, 0.793, 0.866, 0.948, 1.0, 0.968, 0.885, 0.779, 
    0.662, 0.573, 0.493, 0.422, 0.348, 0.296, 0.248, 0.216, 0.182, 0.154, 0.129, 0.109, 0.091, 
    0.078, 0.065, 0.056, 0.047, 0.04, 0.033, 0.028, 0.024, 0.02, 0.017, 0.014, 0.012, 0.01, 
    0.009, 0.007, 0.006, 0.005, 0.004, 0.004, 0.003, 0.002, 0.002, 0.002, 0.001, 0.001, 0.0};

  std::vector<G4double> TpbAbsorptionEnergies = {7.0*eV, 8.0*eV, 9.0*eV, 10.0*eV, 11.0*eV};
  std::vector<G4double> TpbAbsorptionLength = {400*nm, 400*nm, 400*nm, 400*nm, 400*nm};

  G4MaterialPropertiesTable* tpb_pt = new G4MaterialPropertiesTable();
  tpb_pt->AddProperty("RINDEX", GArIndexEnergy, GArIndexSpectrum);
  tpb_pt->AddProperty("WLSABSLENGTH", TpbAbsorptionEnergies, TpbAbsorptionLength);
  tpb_pt->AddProperty("WLSCOMPONENT", TpbEmmisionEnergies, TpbEmmisionSpectrum);
  tpb_pt->AddConstProperty("WLSTIMECONSTANT", 2.5*ns);
  tpb_pt->AddConstProperty("WLSYIELD", 0.6, true);
  ftpb->SetMaterialPropertiesTable(tpb_pt);

}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  fworld = new G4Tubs("world", 0., 16*cm, 35*cm, 0.*deg, 360.*deg);
  fworld_logical = new G4LogicalVolume(fworld, fVacuum, "world_logical", 0, 0, 0);
  fworld_physical = new G4PVPlacement(0, G4ThreeVector(), fworld_logical, "world", 0, false, 0);
  fworld_logical->SetVisAttributes(G4VisAttributes::GetInvisible());
  fMainVolume = new MainVolume(0, G4ThreeVector(), fworld_logical, false, 0, this, fBigQL);
  
  // Add this section to enable particle-specific scintillation
  G4Scintillation* scintillationProcess = new G4Scintillation("Scintillation");

  // Enable particle-specific scintillation yields
  scintillationProcess->SetScintillationByParticleType(true);

  // Get the particle table
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* particleIterator = particleTable->GetIterator();

  // Loop over all particles and attach the scintillation process
  particleIterator->reset();
  while ((*particleIterator)()) {
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      // Ensure that the particle has a valid process manager
      if (pmanager && scintillationProcess->IsApplicable(*particle)) {
          pmanager->AddProcess(scintillationProcess);
          pmanager->SetProcessOrderingToLast(scintillationProcess, idxAtRest);
          pmanager->SetProcessOrderingToLast(scintillationProcess, idxPostStep);
      }
  }
  
  return fworld_physical;
}

void DetectorConstruction::ConstructSDandField()
{
  
  // a-Se sensitive detector
  aSeSD* sensorSD = fASeSD.Get();
  if(!sensorSD)
  {
    G4cout << "Construction /QLux/aSeSD" << G4endl;
    aSeSD* newSensorSD = new aSeSD("/QLux/aSeSD");
    fASeSD.Put(newSensorSD);

    newSensorSD->InitPixels();
    newSensorSD->SetPixelPositions(fMainVolume->GetPixelPositions());
  }
  else
  {
    sensorSD->InitPixels();
    sensorSD->SetPixelPositions(fMainVolume->GetPixelPositions());
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fASeSD.Get());
  SetSensitiveDetector(fMainVolume->GetLogASeSurface(), fASeSD.Get());
     
}

