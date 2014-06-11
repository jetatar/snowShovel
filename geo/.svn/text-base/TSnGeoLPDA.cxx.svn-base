#include "TSnGeoLPDA.h"

#include <TGeoManager.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>

#include <stdexcept>

ClassImp(TSnGeoLPDA);

void TSnGeoLPDA::Build() {
   
   TGeoManager* man = GetGeoManager();
   if (man) {
      
      // TODO: this will be updated to be a bunch of TGeoTubes for the tines
      // and a TGeoBox (maybe 3) for the spine
      //
      // for now...
      TGeoVolume* lpda = man->MakeTrd1("LPDA", man->GetMedium("AluminumMed"),
                                       0.0, // half width at bottom
                                       0.5, // half width at top
                                       0.01, // depth
                                       0.75 // half height
         );
      lpda->SetLineColor(kAzure-6);
      AddNode(lpda, 1);
      
   } else {
      throw std::runtime_error("<TSnGeoLPDA::Build>: "
                               "Could not get GeoManager.");
   }
   
}
