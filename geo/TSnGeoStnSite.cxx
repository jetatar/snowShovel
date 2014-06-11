#include "TSnGeoStnSite.h"

#include <TMath.h>
#include <TString.h>
#include <TGeoManager.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TVector3.h>

#include "TSnGeoDAQBox.h"
#include "TSnGeoLPDA.h"

#include <stdexcept>

ClassImp(TSnGeoStnSite);

void TSnGeoStnSite::Build() {
   TGeoManager* man = GetGeoManager();
   if (man) {
      
      TSnGeoDAQBox* daq = new TSnGeoDAQBox(Form("%s_DAQBox",GetName()));
      TSnGeoLPDA*  lpda = new TSnGeoLPDA(Form("%s_LPDA",GetName()));
      AddNode(daq, 1);
      // antennas are placed in their ideal positions (for now)
      // alignemt adjustments can be made later via TGeoPhysicalNode's
      //
      // TODO: LPDA rotations will be backwards for some channels if the
      // LPDA objects become not symmetric (i.e. with a spine on one side)
      TGeoTranslation ch0(Form("%s_Ch0Trans",GetName()), 0.0,  3.0, -0.5);
      TGeoTranslation ch1(Form("%s_Ch1Trans",GetName()), 3.0,  0.0, -0.5);
      TGeoTranslation ch2(Form("%s_Ch2Trans",GetName()), 0.0, -3.0, -0.5);
      TGeoTranslation ch3(Form("%s_Ch3Trans",GetName()),-3.0,  0.0, -0.5);
      TGeoRotation ch1r(Form("%s_Ch1Rot",GetName()), 90.0, 0.0, 0.0);
      AddNode(lpda, 1, new TGeoCombiTrans(ch0, *gGeoIdentity));
      AddNode(lpda, 2, new TGeoCombiTrans(ch1, ch1r));
      AddNode(lpda, 3, new TGeoCombiTrans(ch2, *gGeoIdentity));
      AddNode(lpda, 4, new TGeoCombiTrans(ch3, ch1r));
      TGeoRotation hbr(Form("%s_HtbtRot",GetName()),
                       -45.0, -90.0, 0.0);
      TGeoTranslation hbt(Form("%s_HtbtCmbTrn",GetName()),
                          18.0 * TMath::Sqrt(2.0),
                          18.0 * TMath::Sqrt(2.0),
                          -0.2);
      AddNode(lpda, 5, new TGeoCombiTrans(hbt, hbr));

   } else {
      throw std::runtime_error("<TSnGeoStnSite::Build>: "
                               "Could not get GeoManager.");
   }
   
}

const TGeoNode* TSnGeoStnSite::GetLPDANode(const UChar_t ch) const {
   // find the node.
   // ch == NSnConstants::kNchans+1 will get the heartbeat LPDA
   return FindNode(Form("%s_LPDA_%d",GetName(),ch+1));
}
   
void TSnGeoStnSite::SetLPDAPosition(const UChar_t ch, TVector3& pos) const {
   // set the LPDA position for the specified channel into the vector 'pos'.
   // see GetLPDANode for channel indexing notes
   const TGeoNode* nch = GetLPDANode(ch);
   if (nch!=0) {
      const Double_t o[3] = {0,0,0}; // origin in the local frame
      Double_t x[3]       = {0,0,0}; // location in the global frame
      nch->LocalToMaster(o, x);
      pos.SetXYZ(x[0],x[1],x[2]);
   } else {
      throw std::runtime_error(Form("<TSnGeoStnSite::SetLPDAPosition>: "
                                    "No LPDA node for ch=%hhu", ch));
   }
}
