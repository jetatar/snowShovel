#ifndef SNS_TSnDualGainCalibSet
#define SNS_TSnDualGainCalibSet

#include "TSnPerCardCalibSet.h"
class TSnGainCalibVal;

class TSnDualGainCalibSet : public TSnPerCardCalibSet {
 public:
   TSnDualGainCalibSet() {}
   TSnDualGainCalibSet(const Char_t* name) :
      TSnPerCardCalibSet(name, "set of dual gain calibs per card") {}
   virtual ~TSnDualGainCalibSet() {}

   virtual TSnCalibSet* NewCopy() const 
      { return new TSnDualGainCalibSet(*this); }
   
   // see TSnCalibSet for the interface

   void ChangeCalibOfChan(const Int_t ch,
                          const TSnGainCalibVal& pos,
                          const TSnGainCalibVal& neg) {
      ChangeCalibOf(ch*NSnConstants::kNsamps, pos, neg);
   }
   
   void ChangeCalibOfOfChan(const Int_t ch,
                      const Float_t mPos, const Float_t emPos,
                      const Float_t bPos, const Float_t ebPos,
                      const Float_t mNeg, const Float_t emNeg,
                      const Float_t bNeg, const Float_t ebNeg) {
      ChangeCalibOf(ch*NSnConstants::kNsamps,
                    mPos, emPos, bPos, ebPos,
                    mNeg, emNeg, bNeg, ebNeg);
   }

   void ChangeCalibOf(const Int_t smp,
                      const TSnGainCalibVal& pos,
                      const TSnGainCalibVal& neg);
   void ChangeCalibOf(const Int_t smp,
                      const Float_t mPos, const Float_t emPos,
                      const Float_t bPos, const Float_t ebPos,
                      const Float_t mNeg, const Float_t emNeg,
                      const Float_t bNeg, const Float_t ebNeg);

   void AddCalibAt(const Int_t smp,
                   const TSnGainCalibVal& pos,
                   const TSnGainCalibVal& neg);
   void AddCalibAt(const Int_t smp,
                   const Float_t mPos, const Float_t emPos,
                   const Float_t bPos, const Float_t ebPos,
                   const Float_t mNeg, const Float_t emNeg,
                   const Float_t bNeg, const Float_t ebNeg);

   void AddCalibAtChan(const Int_t ch,
                   const TSnGainCalibVal& pos,
                   const TSnGainCalibVal& neg) {
      AddCalibAt(ch*NSnConstants::kNsamps, pos, neg);
   }
   void AddCalibAtChan(const Int_t ch,
                   const Float_t mPos, const Float_t emPos,
                   const Float_t bPos, const Float_t ebPos,
                   const Float_t mNeg, const Float_t emNeg,
                   const Float_t bNeg, const Float_t ebNeg) {
      AddCalibAt(ch*NSnConstants::kNsamps,
                 mPos, emPos, bPos, ebPos,
                 mNeg, emNeg, bNeg, ebNeg);
   }
   
   ClassDef(TSnDualGainCalibSet, 1); // set of dual gain calib objects on a per card basis
};

#endif // SNS_TSnDualGainCalibSet
