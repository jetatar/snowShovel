#include "TSnGeoDAQBox.h"

#include <TGeoManager.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>

#include <stdexcept>

ClassImp(TSnGeoDAQBox);

void TSnGeoDAQBox::Build() {
   TGeoManager* man = GetGeoManager();
   if (man) {
      // TODO: use real compac material and size
      TGeoVolume* box = man->MakeBox("DAQBox", man->GetMedium("AluminumMed"),
                                     0.25, 0.2, 0.2);
      box->SetLineColor(kOrange-7);
      AddNode(box, 1);
   } else {
      throw std::runtime_error("<TSnGeoDAQBox::Build>: "
                               "Could not get GeoManager.");
   }
   
}
