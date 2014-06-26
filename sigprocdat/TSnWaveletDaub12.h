#ifndef SNS_TSnWaveletDaub12
#define SNS_TSnWaveletDaub12

#include "TSnWavelet.h"

class TSnWaveletDaub12 : public TSnWavelet {

 public:
   TSnWaveletDaub12(const Bool_t build=kFALSE) {
      // default must be false because default ctor cannot
      // allocate memory on the heap or memory will be leaked if streamed
      // (i.e. stored in a root file)
      if (build) {
         BuildWavelet();
      }
   }
   virtual ~TSnWaveletDaub12() {}
   
   virtual void BuildWavelet();
   virtual const Char_t* GetLabel() const { return "Daub12"; }

   ClassDef(TSnWaveletDaub12, 1); // Daubechies 12-coeff wavelet 
};

#endif // SNS_TSnWaveletDaub12
