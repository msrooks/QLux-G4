#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4VUserDetectorConstruction.hh"

class MainVolume;
class aSeSD;

class G4Box;
class G4Element;
class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
class G4MaterialPropertiesTable;
class G4Sphere;
class G4Tubs;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
 public:
  DetectorConstruction();
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

  void SetSaveThreshold(G4int);
  void SetGeometry(const G4String& geometry);

  G4int GetSaveThreshold() const { return fSaveThreshold; };
  
  //void SetSphereOn(G4bool);
  //static G4bool GetSphereOn() { return fSphereOn; }

  //void SetHousingReflectivity(G4double);
  //G4double GetHousingReflectivity() const { return fRefl; }

  void SetMainScintYield(G4double);
  void SetWLSScintYield(G4double);
 
 private:
  void DefineMaterials();

  G4Tubs* fworld;
  G4LogicalVolume* fworld_logical;
  G4VPhysicalVolume* fworld_physical;

  // Elements
  
  G4Element* fAl;
  G4Element* fN;
  G4Element* fO;
  G4Element* fMg;
  G4Element* fFe;
  G4Element* fC;
  G4Element* fH;
  G4Element* fSi;
  G4Element* fBr;
  G4Element* fCa;
  G4Element* fNa; 
  G4Element* fTi;

  // Materials
  G4Material* fGAr;
  G4Material* fAir;
  G4Material* fVacuum;
  G4Material* fGlass;
  G4Material* hdpe;
  G4Material* fg10;
  G4Material* fpeek;
  G4Material* ftpb;
  G4Material* fAl_6061;
  G4Material* fSiO2;
  G4Material* fAl2O3;
  G4Material* fFe2O3;
  G4Material* fCaO;
  G4Material* fMgO;
  G4Material* fNa2O;
  G4Material* fTiO2;
  G4Material* ffibrous_glass; 
  G4Material* fepoxy_resin;
  G4Material* fFR4;
 

  //G4Tubs* HDPE_short;
  //G4Tubs* HDPE_long;
  //G4LogicalVolume* HDPE_long_logical;
  //G4LogicalVolume* HDPE_short_logical;
  //G4Tubs* TPB_long;
  //G4Tubs* TPB_short;
  //G4LogicalVolume* TPB_long_logical;
  //G4LogicalVolume* TPB_short_logical;

  
  G4int fSaveThreshold;
  G4bool fBigQL;
  G4GenericMessenger* fMessenger;

  //static G4bool fSphereOn;
  //G4double fRefl;
  
  G4bool fMainVolumeOn;
  
  MainVolume* fMainVolume;
  G4MaterialPropertiesTable* fGAr_mt;
  
  // Sensitive Detectors
  G4Cache<aSeSD*> fASeSD;
};

#endif
