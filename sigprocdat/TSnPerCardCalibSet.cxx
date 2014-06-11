#include "TSnPerCardCalibSet.h"

#include "TSnCalibObject.h"

ClassImp(TSnPerCardCalibSet);

void TSnPerCardCalibSet::Print(Option_t* option) const {
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      Printf("Chan %d:", ch);
      GetCalibFor(ch * NSnConstants::kNsamps).Print(option);
   }
}
