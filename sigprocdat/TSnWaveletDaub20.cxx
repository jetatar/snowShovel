#include "TSnWaveletDaub20.h"

ClassImp(TSnWaveletDaub20);

void TSnWaveletDaub20::BuildWavelet() {
   SetNcof(20u);
   
   SetCC(0,   0.026670057901);
   SetCC(1,   0.188176800078);
   SetCC(2,   0.527201188932);
   SetCC(3,   0.688459039454);
   SetCC(4,   0.281172343661);
   SetCC(5,  -0.249846424327);
   SetCC(6,  -0.195946274377);
   SetCC(7,   0.127369340336);
   SetCC(8,   0.093057364604);
   SetCC(9,  -0.071394147166);
   SetCC(10, -0.029457536822);
   SetCC(11,  0.033212674059);
   SetCC(12,  0.003606553567);
   SetCC(13, -0.010733175483);
   SetCC(14,  0.001395351747);
   SetCC(15,  0.001992405295);
   SetCC(16, -0.000685856695);
   SetCC(17, -0.000116466855);
   SetCC(18,  0.000093588670);
   SetCC(19, -0.000013264203);
   
   BuildCRandOffs();
   
}
