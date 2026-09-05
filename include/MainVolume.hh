
#ifndef MainVolume_h
#define MainVolume_h 1
#include "DetectorConstruction.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"

class G4Box;
class G4LogicalVolume;
class G4Tubs;

class MainVolume : public G4PVPlacement
{
 public:
  MainVolume(G4RotationMatrix* pRot, const G4ThreeVector& tlate,
            G4LogicalVolume* pMotherLogical, G4bool pMany, G4int pCopyNo,
            G4bool bigQL);

  G4LogicalVolume* GetLogASeSurface() { return fASeSurface_logical; }
  
  std::vector<G4ThreeVector> GetPixelPositions() { return fPixelPositions; }
    


 private:
  void VisAttributes();
  void SurfaceProperties();

  void PlacePixels(G4LogicalVolume* sensorLog, G4double& z);

  
  G4bool fBigQL;

  // Volumes  
  G4Tubs* GAr_scintillator;
  G4Box* cryostat;
  G4Tubs* pixelboard; 
  
  // Logical volumes
  G4LogicalVolume* fScint_logical;
  G4LogicalVolume* fcryostat_logical;
  G4LogicalVolume* fSensor_logical;
  G4LogicalVolume* fASeSurface_logical;
  G4LogicalVolume* fieldrings_logical;
  
  G4LogicalVolume* pixelboard_logical;
  G4LogicalVolume* spacer_logical;
  G4LogicalVolume* flid_logical;
  G4LogicalVolume* fanode_logical;
  
  std::vector<G4ThreeVector> fPixelPositions;

};

#endif
