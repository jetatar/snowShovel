#include "TSnWaveletHaar.h"

#include <TMath.h>

ClassImp(TSnWaveletHaar);

void TSnWaveletHaar::BuildWavelet() {
   SetNcof(2u);
   
   SetCC(0, 1.0 / TMath::Sqrt2());
   SetCC(1, 1.0 / TMath::Sqrt2());
   
   BuildCRandOffs();
}
