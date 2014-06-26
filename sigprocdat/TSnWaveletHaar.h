#ifndef SNS_TSnWaveletHaar
#define SNS_TSnWaveletHaar

#include "TSnWavelet.h"

class TSnWaveletHaar : public TSnWavelet {

 public:
   TSnWaveletHaar(const Bool_t build=kFALSE) {
      // default must be false because default ctor cannot
      // allocate memory on the heap or memory will be leaked if streamed
      // (i.e. stored in a root file)
      if (build) {
         BuildWavelet();
      }
   }
   virtual ~TSnWaveletHaar() {}
   
   virtual void BuildWavelet();
   virtual const Char_t* GetLabel() const { return "Haar"; }
   
   ClassDef(TSnWaveletHaar, 1);
};

#endif // SNS_TSnWaveletHaar
