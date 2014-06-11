#include "makeCalibTree.C"

void makeFpnSubTree(const Char_t* infn,
                    const Char_t* fpnFilen,
                    const Char_t* outdir,
                    const Char_t* fpnSetNm="FPNSet") {
   // see makeCalibTree.
   makeCalibTree(infn, fpnFilen, 0, outdir, fpnSetNm);
}
                    
