#ifndef SNS_TSnWaveletDaub4
#define SNS_TSnWaveletDaub4

#include "TSnWavelet.h"

class TSnWaveletDaub4 : public TSnWavelet {

 public:
   TSnWaveletDaub4(const Bool_t build=kFALSE) {
      // default must be false because default ctor cannot
      // allocate memory on the heap or memory will be leaked if streamed
      // (i.e. stored in a root file)
      if (build) {
         BuildWavelet();
      }
   }
   virtual ~TSnWaveletDaub4() {}
   
   virtual void BuildWavelet();
   virtual const Char_t* GetLabel() const { return "Daub4"; }

   ClassDef(TSnWaveletDaub4, 1); // Daubechies 4-coeff wavelet 
};

#endif // SNS_TSnWaveletDaub4
