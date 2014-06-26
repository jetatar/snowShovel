#include "TSnWaveletDaub12.h"

ClassImp(TSnWaveletDaub12);

void TSnWaveletDaub12::BuildWavelet() {
   SetNcof(12u);
   
   SetCC(0,  0.111540743350);
   SetCC(1,  0.494623890398);
   SetCC(2,  0.751133908021);
   SetCC(3,  0.315250351709);
   SetCC(4,  -0.226264693965);
   SetCC(5,  -0.129766867567);
   SetCC(6,  0.097501605587);
   SetCC(7,  0.027522865530);
   SetCC(8,  -0.031582039318);
   SetCC(9,  0.000553842201);
   SetCC(10, 0.004777257511);
   SetCC(11, -0.001077301085);
   
   BuildCRandOffs();
   
}
