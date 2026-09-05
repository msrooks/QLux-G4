
#ifndef MainVolume_h
#define MainVolume_h 1
#include "DetectorConstruction.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"

class G4Box;
class G4LogicalVolume;
class G4Sphere;
class G4Tubs;

class MainVolume : public G4PVPlacement
{
 public:
  MainVolume(G4RotationMatrix* pRot, const G4ThreeVector& tlate,
                G4LogicalVolume* pMotherLogical, G4bool pMany, G4int pCopyNo,
                DetectorConstruction* c, G4bool bigQL);

  G4LogicalVolume* GetLogASeSurface() { return fASeSurface_logical; }
  G4LogicalVolume* GetLogScint() { return fScint_logical; }
  G4LogicalVolume* GetLogPin() { return fpin_logical; }
  
  std::vector<G4ThreeVector> GetPixelPositions() { return fPixelPositions; }
    


 private:
  void VisAttributes();
  void SurfaceProperties();

  void PlacePixels(G4LogicalVolume* sensorLog, G4double& z);

  void CopyValues();

  DetectorConstruction* fConstructor;
  G4bool fBigQL;

  // Materials
  G4Material* hdpe;
  G4Material* g10;
  G4Material* peek;

  // Volumes  
  G4Tubs* GAr_scintillator;
  G4Tubs* TubeInteriorSolid;
  G4Box* cryostat;
  G4Tubs* TPB_short;
  G4Tubs* HDPE_short;
  G4Tubs* pixelboard; 
  G4Tubs* QSDPAD; 
  
  G4Tubs* TPB_tube;
  G4Tubs* TPB_lid;
  G4Tubs* HDPE_lid;

  // Logical volumes
  G4LogicalVolume* fScint_logical;
  G4LogicalVolume* fcryostat_logical;
  G4LogicalVolume* fSensor_logical;
  G4LogicalVolume* fASeSurface_logical;
  G4LogicalVolume* fSphere_log;
  G4LogicalVolume* fieldrings_logical;
  G4LogicalVolume* g10legs_logical;
  G4LogicalVolume* steelparts_logical;
  G4LogicalVolume* adapterplate_logical;
  G4LogicalVolume* peek_logical;
  
  G4LogicalVolume* TPB_tube_logical;
  G4LogicalVolume* HDPE_short_logical;
  G4LogicalVolume* TPB_lid_logical;                
  G4LogicalVolume* HDPE_lid_logical;

  G4LogicalVolume* TPB_short_logical;
  G4LogicalVolume* pixelboard_logical;
  G4LogicalVolume* spacer_logical;
  G4LogicalVolume* flid_logical;
  G4LogicalVolume* fpin_logical;
  G4LogicalVolume* Cathode_logical;
  G4LogicalVolume* Cube_logical;
  G4LogicalVolume* QSDPAD_logical;
  G4LogicalVolume* TubeInteriorSolid_logical;
  
  std::vector<G4ThreeVector> fPixelPositions;

};

#endif
