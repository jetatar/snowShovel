#ifndef SNS_TSnFPNCalibSet
#define SNS_TSnFPNCalibSet

#include "TSnPerSampCalibSet.h"
#include "NSnConstants.h"

class TSnFPNCalibSet : public TSnPerSampCalibSet {
 public:
   TSnFPNCalibSet() {}
   TSnFPNCalibSet(const Char_t* name) :
      TSnPerSampCalibSet(name, "set of FPN calib values for each sample") {}
   virtual ~TSnFPNCalibSet() {}

   virtual TSnCalibSet* NewCopy() const { return new TSnFPNCalibSet(*this); }

   // see TSnCalibSet for the interface
   
   Float_t       GetFPNError(const Int_t smp) const;
   Float_t       GetRMSError(const Int_t smp) const;
   
   void ChangeCalibOf(const Int_t smp, 
                      const Float_t fpn, const Float_t rms, const Float_t n);
   void AddCalibAt(const Int_t smp, 
                   const Float_t fpn, const Float_t rms, const Float_t n);

   ClassDef(TSnFPNCalibSet, 1); // set of TSnFPNCalibVal's for each sample
};

#endif // SNS_TSnFPNCalibSet
