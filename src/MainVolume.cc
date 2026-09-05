#include "MainVolume.hh"
#include "globals.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4Sphere.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "CADMesh.hh"
#include "G4Torus.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4UniformElectricField.hh"
#include "G4FieldManager.hh"
#include "G4UserLimits.hh"

MainVolume::MainVolume(G4RotationMatrix* pRot, const G4ThreeVector& tlate,
                             G4LogicalVolume* pMotherLogical, G4bool pMany,
                             G4int pCopyNo, DetectorConstruction* c, G4bool bigQL)
  // Pass info to the G4PVPlacement constructor
  : G4PVPlacement(pRot, tlate,
                  // Temp logical volume must be created here
                  new G4LogicalVolume(new G4Box("temp", 1, 1, 1),
                                      G4Material::GetMaterial("Vacuum"), "temp",
                                      0, 0, 0),
                  "cryostat", pMotherLogical, pMany, pCopyNo)
  , fConstructor(c)
  , fBigQL(bigQL)
{
  
  // CRYOSTAT AND LAR AS SCINTILLATOR

  cryostat = new G4Box("cryo", 10.0 * cm, 10.0 * cm, 10.0 * cm);
  fcryostat_logical = new G4LogicalVolume(cryostat, G4Material::GetMaterial("G4_STAINLESS-STEEL"), "cryostat_logical", 0, 0, 0);
  
  GAr_scintillator = new G4Tubs("Gar_scint", 0., 13.2461*cm, 30.48*cm, 0.*deg, 360.*deg);
  fScint_logical   = new G4LogicalVolume(GAr_scintillator, G4Material::GetMaterial("GAr"),"scint_log", 0, 0, 0);
  
  new G4PVPlacement(0, G4ThreeVector(), fScint_logical, "scintillator", fcryostat_logical, false, 0);
  
  // FR4 pixelboard 
  pixelboard = new G4Tubs("pixelboard", 0*cm, 2.2*cm, 1*mm, 0.*deg, 360.*deg);
  pixelboard_logical = new G4LogicalVolume(pixelboard, G4Material::GetMaterial("FR4"), "pixelboard_logical", 0, 0, 0);
  
  G4double pixelboard_z = fBigQL ? -2.002 * mm : -1.802 * mm;
  new G4PVPlacement(0, G4ThreeVector(0, 0, pixelboard_z), pixelboard_logical, "pixelboard", fScint_logical, false, 0);

  
  G4UserLimits* stepLimit = new G4UserLimits(5.0 * mm);  
  fScint_logical->SetUserLimits(stepLimit);

  // PIXEL DETECTORS 
  
  G4double sensor_x = fBigQL ? 1.0 * mm : 0.75 * mm;
  G4double sensor_y = fBigQL ? 1.99 * mm : 1.0 * mm;
  G4double aSeSurface_x = fBigQL ? 0.999 * mm : 0.749 * mm;
  G4double aSeSurface_y = fBigQL ? 1.989 * mm : 0.999 * mm;

  auto sensor_solid = new G4Box("aSeSensor_solid", sensor_x, sensor_y, 0.01 * mm);
  auto fASeSurface_solid = new G4Box("aSeSurface_solid", aSeSurface_x, aSeSurface_y, 0.005 * mm);

  fSensor_logical = new G4LogicalVolume(sensor_solid, G4Material::GetMaterial("Glass"), "aSeSensor_log");
  fASeSurface_logical = new G4LogicalVolume(fASeSurface_solid, G4Material::GetMaterial("Al_6061"), "aSeSurface_logical");

  new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fASeSurface_logical, "aSeSurface", fSensor_logical, false, 0);
  
  
  G4double z;
  z = 0;
  
  
  const char* lid_file = fBigQL ? "./lidBig.obj" : "./lidLittle.obj";
  const char* anode_file = fBigQL ? "./anodeBig.obj" : "./anodeLittle.obj";
  const char* spacer_file = fBigQL ? "./spacerBig.obj" : "./spacerLittle.obj";

  // lid
  auto flid_mesh = CADMesh::TessellatedMesh::FromOBJ(lid_file);
  flid_mesh->SetScale(10.0);
  flid_logical = new G4LogicalVolume(flid_mesh->GetSolid(), G4Material::GetMaterial("FR4"), "logical", 0, 0, 0);

  // QPin
  auto fpin_mesh = CADMesh::TessellatedMesh::FromOBJ(anode_file);
  fpin_mesh->SetScale(10.0);
  fpin_logical = new G4LogicalVolume(fpin_mesh->GetSolid(), G4Material::GetMaterial("G4_Au"), "logical", 0, 0, 0);

  // FR4 Spacer
  auto spacer = CADMesh::TessellatedMesh::FromOBJ(spacer_file);
  spacer->SetScale(10.0);
  spacer_logical = new G4LogicalVolume(spacer->GetSolid(), G4Material::GetMaterial("FR4"), "spacer_logical", 0, 0, 0);

// Place the lid and anode
G4double pitch = 10.2 * mm;
int id = 0;

for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
        G4double x = (col - 1) * pitch;
        G4double y = (1 - row) * pitch;
        
        
        new G4PVPlacement(nullptr, G4ThreeVector(x, y, 0), flid_logical, "lid", fScint_logical, false, id);
        new G4PVPlacement(nullptr, G4ThreeVector(x, y, 0.001 * mm), fpin_logical, "pin", fScint_logical, false, id);
        
        G4double spacer_z = fBigQL ? -1.101 * mm : -0.801 * mm;
        new G4PVPlacement(nullptr, G4ThreeVector(x, y, spacer_z), spacer_logical, "spacer", fScint_logical, false, id);


        ++id;
    }
}

  PlacePixels(fSensor_logical, z);
  VisAttributes();
  SurfaceProperties();
  SetLogicalVolume(fcryostat_logical);
}

void MainVolume::PlacePixels(G4LogicalVolume* sensor_log, G4double& z)
{
    G4double pitch = 10.2 * mm;
    
    G4double dx = fBigQL ? -0.05 * mm : -0.02 * mm;
    G4double dy = fBigQL ? 1.51 * mm : 0.56 * mm;
    
  
    int id = 0;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            G4double x = (col - 1) * pitch + dx;
            G4double y = (1 - row) * pitch + dy;

            G4ThreeVector position(x, y, z);

            new G4PVPlacement(nullptr, position, sensor_log, "aSeSensor", fScint_logical, false, id);

            fPixelPositions.push_back(position);

            ++id;
        }
    }
}


void MainVolume::VisAttributes()
{

  G4VisAttributes* cryostat_ = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8,0.0));
  fcryostat_logical->SetVisAttributes(cryostat_);
  
  G4VisAttributes* Ar_ = new G4VisAttributes(G4Colour(137, 0, 255, 0));
  fScint_logical->SetVisAttributes(Ar_);
  
  G4VisAttributes* pixelboard_ = new G4VisAttributes(G4Colour(0.206, 0.351, 0.186, 1));
  pixelboard_logical->SetVisAttributes(pixelboard_);

  G4VisAttributes* spacer_ = new G4VisAttributes(G4Colour(0.96, 0.87, 0.70, 0.1));
  spacer_logical->SetVisAttributes(spacer_);

  G4VisAttributes* sensorVis = new G4VisAttributes(G4Colour(0.5, 0.05, 0.2, 1));
  fSensor_logical->SetVisAttributes(sensorVis);
  
  G4VisAttributes* aSeSurfaceVis = new G4VisAttributes(G4Colour(.545,.827,.984, 1));
  fASeSurface_logical->SetVisAttributes(aSeSurfaceVis);
  
  G4VisAttributes* flid_ = new G4VisAttributes(G4Colour(0.206, 0.351, 0.186, 1));
  flid_logical->SetVisAttributes(flid_);
  
  G4VisAttributes* fpin_ = new G4VisAttributes(G4Colour(0.83, 0.69, 0.22, 1));
  fpin_logical->SetVisAttributes(fpin_);

  //G4VisAttributes* fieldrings_ = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75, 0.0));
  //fieldrings_logical->SetVisAttributes(fieldrings_); 

  //G4VisAttributes* Cathode_ = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75, 0));
  //Cathode_logical->SetVisAttributes(Cathode_); 

  //G4VisAttributes* Cube_ = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75,0.));
  //Cube_logical->SetVisAttributes(Cube_); 

  // HDPE SHORT 
  //G4VisAttributes* HDPE_short_ = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.2));
  //HDPE_short_logical->SetVisAttributes(HDPE_short_);

  //G4VisAttributes* HDPE_lid_ = new G4VisAttributes(G4Colour(0.0,1.0,0.0, 0.2));
  //HDPE_lid_logical->SetVisAttributes(HDPE_lid_);

  //G4VisAttributes* TPB_tube_ = new G4VisAttributes(G4Colour(.545,.827,.984, 0.2));
  //TPB_tube_logical->SetVisAttributes(TPB_tube_);

  //G4VisAttributes* TPB_lid_ = new G4VisAttributes(G4Colour(.545,.827,.984, 0.2));
  //TPB_lid_logical->SetVisAttributes(TPB_lid_);

  
  
}

void MainVolume::SurfaceProperties()
{
  std::vector<G4double> ephoton = { 2 * eV, 11 * eV };

  // CRYOSTAT PROPERTIES
  std::vector<G4double> steel_energy = {2.13*eV, 2.32*eV, 2.54*eV, 2.8*eV, 3.12*eV, 3.5*eV, 3.94*eV, 4.54*eV, 5.72*eV, 10.83*eV, 11.5*eV};
  std::vector<G4double> steel_reflectivity = {0.64, 0.63, 0.62, 0.61, 0.6, 0.58, 0.55, 0.51, 0.45, 0.31, 0.2};
  
  std::vector<G4double> aluminum_energy = {2.04*eV, 2.38*eV, 2.83*eV, 3.51*eV, 4.54*eV, 5.63*eV, 6.56*eV, 7.12*eV, 8.17*eV, 10.17*eV};
  std::vector<G4double> aluminum_reflectivity = {0.89, 0.9, 0.9, 0.9, 0.9, 0.87, 0.82, 0.75, 0.54, 0.25};

  std::vector<G4double> g10_energy = {2.12*eV, 2.33*eV, 2.58*eV, 2.98*eV, 3.38*eV, 3.85*eV, 4.35*eV, 5.28*eV, 7.08*eV, 10.08*eV};
  std::vector<G4double> g10_reflectivity = {0.41, 0.41, 0.39, 0.32, 0.23, 0.13, 0.1, 0.07, 0.01, 0.0};

  std::vector<G4double> peek_energy = {2.12*eV, 2.33*eV, 2.58*eV, 2.98*eV, 3.38*eV, 3.85*eV, 4.35*eV, 5.28*eV, 7.08*eV, 10.08*eV};
  std::vector<G4double> peek_reflectivity = {0.41, 0.41, 0.39, 0.32, 0.23, 0.13, 0.1, 0.07, 0.01, 0.0};
  
  std::vector<G4double> pin_energy = {12.41*eV, 11.75*eV, 11.10*eV, 10.53*eV, 10.05*eV, 9.63*eV, 9.18*eV, 8.80*eV, 8.41*eV, 8.07*eV, 6.22*eV, 4.96*eV, 4.13*eV, 3.54*eV, 3.09*eV, 2.75*eV, 2.64*eV, 2.49*eV, 2.37*eV, 2.25*eV};
  std::vector<G4double> pin_reflectivity = {0.11, 0.12, 0.13, 0.13, 0.14, 0.14, 0.15, 0.16, 0.17, 0.17, 0.2, 0.29, 0.34, 0.33, 0.37, 0.37, 0.37, 0.44, 0.64, 0.77};

  std::vector<G4double> pixelboard_efficiency = {1., 1.};
  std::vector<G4double> pixelboard_reflectivity = {0., 0.};  
  
  std::vector<G4double> zero_efficiency(steel_energy.size(), 0.0);

  G4MaterialPropertiesTable* cryostatPT = new G4MaterialPropertiesTable();
  cryostatPT->AddProperty("REFLECTIVITY", steel_energy, steel_reflectivity);
  G4OpticalSurface* Opcryostat = new G4OpticalSurface("CryostatSurface", unified, polished, dielectric_metal);
  Opcryostat->SetMaterialPropertiesTable(cryostatPT);
  
  
  G4MaterialPropertiesTable* pixelboardPT = new G4MaterialPropertiesTable();
  pixelboardPT->AddProperty("REFLECTIVITY", ephoton, pixelboard_reflectivity);
  G4OpticalSurface* Oppixelboard = new G4OpticalSurface("pixelboard", unified, polished, dielectric_metal);
  Oppixelboard->SetMaterialPropertiesTable(pixelboardPT);

  G4MaterialPropertiesTable* spacerPT = new G4MaterialPropertiesTable();
  spacerPT->AddProperty("REFLECTIVITY", ephoton, pixelboard_reflectivity);
  G4OpticalSurface* Opspacer = new G4OpticalSurface("spacer", unified, polished, dielectric_metal);
  Opspacer->SetMaterialPropertiesTable(spacerPT);
  
  G4MaterialPropertiesTable* lidPT = new G4MaterialPropertiesTable();
  lidPT->AddProperty("REFLECTIVITY", ephoton, pixelboard_reflectivity);
  G4OpticalSurface* Oplid = new G4OpticalSurface("lid", unified, polished, dielectric_metal);
  Oplid->SetMaterialPropertiesTable(lidPT);

  G4MaterialPropertiesTable* pinPT  = new G4MaterialPropertiesTable();
  pinPT->AddProperty("REFLECTIVITY", pin_energy, pin_reflectivity);
  G4OpticalSurface* Oppin = new G4OpticalSurface("pinSurface", unified, polished, dielectric_metal);
  Oppin->SetMaterialPropertiesTable(pinPT);
  
  //G4MaterialPropertiesTable* CathodePT  = new G4MaterialPropertiesTable();
  //CathodePT->AddProperty("REFLECTIVITY", steel_energy, steel_reflectivity);
  //G4OpticalSurface* OpCathodeSurface = new G4OpticalSurface("CathodeSurface", unified, polished, dielectric_metal);
  //OpCathodeSurface->SetMaterialPropertiesTable(CathodePT);

  // Field Rings
  //G4MaterialPropertiesTable* fieldringsPT  = new G4MaterialPropertiesTable();
  //fieldringsPT->AddProperty("REFLECTIVITY", steel_energy, steel_reflectivity);
  //fieldringsPT->AddProperty("EFFICIENCY", steel_energy, zero_efficiency);
  //G4OpticalSurface* OpfieldringsSurface = new G4OpticalSurface("fieldringsSurface", unified, polished, dielectric_metal);
  //OpfieldringsSurface->SetMaterialPropertiesTable(fieldringsPT);
  //new G4LogicalSkinSurface("fieldrings_surface", fieldrings_logical, OpfieldringsSurface);
  
  // Cube
  //G4MaterialPropertiesTable* CubePT  = new G4MaterialPropertiesTable();
  //CubePT->AddProperty("REFLECTIVITY", steel_energy, steel_reflectivity);
  //CubePT->AddProperty("EFFICIENCY", steel_energy, zero_efficiency);
  //G4OpticalSurface* OpCubeSurface = new G4OpticalSurface("CubeSurface", unified, ground, dielectric_metal);
  //OpCubeSurface->SetMaterialPropertiesTable(CubePT);
  //new G4LogicalSkinSurface("Cubesurface", Cube_logical, OpCubeSurface);  
  
  
  // HDPE with TPB
  
  /*
  std::vector<G4double> tpbenergies = {2.1*eV, 2.9*eV, 3.0*eV, 3.1*eV, 3.2*eV};
  std::vector<G4double> tpbreflectivity = {1, 1, 1, 1, 1};
  

  G4MaterialPropertiesTable* HDPE_shortPT  = new G4MaterialPropertiesTable();
  HDPE_shortPT->AddProperty("REFLECTIVITY", tpbenergies, tpbreflectivity);
  G4OpticalSurface* OpHDPE_shortSurface = new G4OpticalSurface("HDPE_shortSurface", unified, ground, dielectric_metal);
  OpHDPE_shortSurface->SetMaterialPropertiesTable(HDPE_shortPT);
  new G4LogicalSkinSurface("HDPE_short_surface", HDPE_short_logical, OpHDPE_shortSurface);
  
  G4MaterialPropertiesTable* HDPE_lidPT  = new G4MaterialPropertiesTable();
  HDPE_lidPT->AddProperty("REFLECTIVITY", tpbenergies, tpbreflectivity);
  G4OpticalSurface* OpHDPE_lidSurface = new G4OpticalSurface("HDPE_lidSurface", unified, ground, dielectric_metal);
  OpHDPE_lidSurface->SetMaterialPropertiesTable(HDPE_lidPT);
  new G4LogicalSkinSurface("HDPE_lid_surface", HDPE_lid_logical, OpHDPE_lidSurface);
  */

  //G4MaterialPropertiesTable* TPB_tubePT  = new G4MaterialPropertiesTable();
  //G4OpticalSurface* OpTPB_tubeSurface = new G4OpticalSurface("TPB_tube", unified, polished, dielectric_dielectric);
  //OpTPB_tubeSurface->SetMaterialPropertiesTable(TPB_tubePT);
  //new G4LogicalSkinSurface("TPB_tube", TPB_tube_logical, OpTPB_tubeSurface);
  
  //G4MaterialPropertiesTable* TPB_lidPT  = new G4MaterialPropertiesTable();
  //G4OpticalSurface* OpTPB_lidSurface = new G4OpticalSurface("TPB_lidSurface", unified, polished, dielectric_dielectric);
  //OpTPB_lidSurface->SetMaterialPropertiesTable(TPB_lidPT);
  //new G4LogicalSkinSurface("TPB_lidsurface", TPB_lid_logical, OpTPB_lidSurface);
  

  // ------------------------------------------------------------------------------- //
  std::vector<G4double> aSeSurface_energy = { 2 * eV, 11 * eV };
  std::vector<G4double> aSeSurface_EFF     = { 1., 1. };
  std::vector<G4double> aSeSurface_REF     = { 0., 0. };
  
  G4MaterialPropertiesTable* aSeSurface_mt = new G4MaterialPropertiesTable();
  aSeSurface_mt->AddProperty("EFFICIENCY", aSeSurface_energy, aSeSurface_EFF);
  aSeSurface_mt->AddProperty("REFLECTIVITY", aSeSurface_energy, aSeSurface_REF);
  
  G4OpticalSurface* aSeSurface_opsurf = new G4OpticalSurface("aSeSurface_opsurf", glisur, polished, dielectric_metal);
  aSeSurface_opsurf->SetMaterialPropertiesTable(aSeSurface_mt);
  new G4LogicalSkinSurface("aSeSurface_surf", fASeSurface_logical, aSeSurface_opsurf);
 
  
  new G4LogicalSkinSurface("cryo_surf", fcryostat_logical, Opcryostat); 
  new G4LogicalSkinSurface("pixelboard_surface", pixelboard_logical, Oppixelboard);
  new G4LogicalSkinSurface("spacer_surface", spacer_logical, Opspacer);
  //new G4LogicalSkinSurface("CathodeSurface", Cathode_logical, OpCathodeSurface);
  new G4LogicalSkinSurface("lid_surface", flid_logical, Oplid);
  new G4LogicalSkinSurface("pin_surface", fpin_logical, Oppin);
    
}



