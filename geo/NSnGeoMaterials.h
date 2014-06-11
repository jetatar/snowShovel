#ifndef SNS_NSnGeoMaterials
#define SNS_NSnGeoMaterials

class TGeoManager;

namespace NSnGeoMaterials {
   
   enum EAriannMedium_t {
      kPureSnow,
      kAluminum
   };
   
   extern
   void DefineMaterials(TGeoManager& geoman);
   
   
};

#endif // SNS_NSnGeoMaterials
