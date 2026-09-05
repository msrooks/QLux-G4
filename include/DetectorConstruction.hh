#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4VUserDetectorConstruction.hh"

class MainVolume;
class aSeSD;

class G4Element;
class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
class G4MaterialPropertiesTable;
class G4Tubs;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
 public:
  DetectorConstruction();
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;
  void SetGeometry(const G4String& geometry);
   
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
   
  G4bool fBigQL;
  G4GenericMessenger* fMessenger;

  MainVolume* fMainVolume;
  G4MaterialPropertiesTable* fGAr_mt;
  
  // Sensitive Detectors
  G4Cache<aSeSD*> fASeSD;
};

#endif
