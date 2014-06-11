#ifndef SNS_TSnPerSampCalibSet
#define SNS_TSnPerSampCalibSet

#include "TSnCalibSet.h"
#include "NSnConstants.h"

class TSnPerSampCalibSet : public TSnCalibSet {
 private:
   // 1 to 1 mapping
   virtual Int_t GetCalibIndex(const Int_t smp) const { return smp; }

 public:
   TSnPerSampCalibSet() {}
   TSnPerSampCalibSet(const Char_t* name,
                      const Char_t* title) :
      TSnCalibSet(name, title, NSnConstants::kTotSamps) {}
   virtual ~TSnPerSampCalibSet() {}

   virtual TSnCalibSet* NewCopy() const { return new TSnPerSampCalibSet(*this); }
   
   // see TSnCalibSet for interface
   
   ClassDef(TSnPerSampCalibSet, 1); // base class storing calib objs on a per sample basis
};

#endif // SNS_TSnPerSampCalibSet
