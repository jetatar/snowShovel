#ifndef SNS_TSnWaveletDaub20
#define SNS_TSnWaveletDaub20

#include "TSnWavelet.h"

class TSnWaveletDaub20 : public TSnWavelet {

 public:
   TSnWaveletDaub20(const Bool_t build=kFALSE) {
      // default must be false because default ctor cannot
      // allocate memory on the heap or memory will be leaked if streamed
      // (i.e. stored in a root file)
      if (build) {
         BuildWavelet();
      }
   }
   virtual ~TSnWaveletDaub20() {}
   
   virtual void BuildWavelet();
   virtual const Char_t* GetLabel() const { return "Daub20"; }

   ClassDef(TSnWaveletDaub20, 1); // Daubechies 20-coeff wavelet 
};

#endif // SNS_TSnWaveletDaub20
