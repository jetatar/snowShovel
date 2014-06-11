#include "makeCalibTree.C"

void makeCalibTreeFromFpn(const Char_t* infn,
                          const Char_t* calFilen,
                          const Char_t* outdir,
                          const Char_t* calSetNm="DigitizerCalibSet") {
   // see makeCalibTree.
   makeCalibTree(infn, 0, calFilen, outdir, "FPNSet", calSetNm);
}
                    
