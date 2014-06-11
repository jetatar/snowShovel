#ifndef SNS_TSnPerCardCalibSet
#define SNS_TSnPerCardCalibSet

#include "NSnConstants.h"
#include "TSnCalibSet.h"

class TSnPerCardCalibSet : public TSnCalibSet {
 private:
   virtual Int_t GetCalibIndex(const Int_t smp) const {
      // map from sample number to channel number
      // (one calib obj per channel)
      return (static_cast<Int_t>(smp /
                                 static_cast<Int_t>(NSnConstants::kNsamps)) );
   }

 public:
   TSnPerCardCalibSet() {}
   TSnPerCardCalibSet(const Char_t* name,
                      const Char_t* title) :
      TSnCalibSet(name, title,
                  NSnConstants::kNchans) {
   }
   virtual ~TSnPerCardCalibSet() {}

   virtual TSnCalibSet* NewCopy() const { return new TSnPerCardCalibSet(*this); }
   
   virtual void Print(Option_t* option = "") const;
   
   // see TSnCalibSet for the interface
   
   ClassDef(TSnPerCardCalibSet, 1); // base class storing calib objects on a per card basis
};

#endif // SNS_TSnPerCardCalibSet
