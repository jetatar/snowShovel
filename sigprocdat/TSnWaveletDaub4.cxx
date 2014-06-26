#include "TSnWaveletDaub4.h"

ClassImp(TSnWaveletDaub4);

void TSnWaveletDaub4::BuildWavelet() {
   SetNcof(4u);
   
   SetCC(0,  0.4829629131445341);
   SetCC(1,  0.8365163037378079);
   SetCC(2,  0.2241438680420134);
   SetCC(3, -0.1294095225512604);

   BuildCRandOffs();
   
}
