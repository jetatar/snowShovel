#include "NSnGeoMaterials.h"

#include <TGeoManager.h>
#include <TGeoElement.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>

#include <stdexcept>

void NSnGeoMaterials::DefineMaterials(TGeoManager& geoman) {
   
   TGeoElementTable* elmtbl = geoman.GetElementTable();
   if (elmtbl!=0) {
      // some default parameters to use
      Double_t param[20];
      param[0] =   0;      // isvol  - Not used (sensitive volume flag)
      param[1] =   0;      // ifield - User defined magnetic field
      param[2] =  10.;     // fieldm - Maximum field value (in kiloGauss)
      param[3] = -20.;     // tmaxfd - Maximum angle due to field deflection
      param[4] =  -0.01;   // stemax - Maximum step allowed
      param[5] =  -0.3;    // deemax - Maximum fractional energy loss, DLS
      param[6] =   0.001;  // epsil  - Tracking precision (cm)
      param[7] =  -0.8;    // stmin  - Min. step due to continuous processes (cm)
      for (Int_t i=8; i<20; i++) param[i] = 0.;
      //  ifield = 0 if no magnetic field
      //  ifield = -1 if user decision in guswim
      //  ifield = 1 if tracking performed with g3rkuta
      //  ifield = 2 if tracking performed with g3helix
      //  ifield = 3 if tracking performed with g3helx3
      
      // pure snow
      TGeoMixture* pureSnow = new TGeoMixture("PureSnowMix", 2);
      pureSnow->AddElement(elmtbl->GetElement(8), 1);
      pureSnow->AddElement(elmtbl->GetElement(1), 2);
      new TGeoMedium("PureSnowMed", kPureSnow, pureSnow, param);
      
      // pure aluminum
      TGeoMixture* pureAl = new TGeoMixture("AluminumMix", 1);
      pureAl->AddElement(elmtbl->GetElement(13), 1);
      new TGeoMedium("AluminumMed", kAluminum, pureAl, param);
      
   } else {
      throw std::runtime_error("<NSnGeoMaterials::DefineMaterials>: "
                               "Could not get element table.");
   }

}

