#include "TSnDualGainCalibSet.h"

#include <TObjArray.h>

#include "TSnDualGainCalib.h"
#include "TSnGainCalibVal.h"

ClassImp(TSnDualGainCalibSet);

void TSnDualGainCalibSet::ChangeCalibOf(const Int_t smp,
                                        const TSnGainCalibVal& pos,
                                        const TSnGainCalibVal& neg) {
   TSnDualGainCalib& c = dynamic_cast<TSnDualGainCalib&>(GetCalibFor(smp));
   c.GetPosGainCalib() = pos;
   c.GetNegGainCalib() = neg;
}

void TSnDualGainCalibSet::ChangeCalibOf(const Int_t smp,
                   const Float_t mPos, const Float_t emPos,
                   const Float_t bPos, const Float_t ebPos,
                   const Float_t mNeg, const Float_t emNeg,
                   const Float_t bNeg, const Float_t ebNeg) {
   TSnDualGainCalib& c = dynamic_cast<TSnDualGainCalib&>(GetCalibFor(smp));
   c.GetPosGainCalib().SetParameters(mPos, emPos, bPos, ebPos);
   c.GetNegGainCalib().SetParameters(mNeg, emNeg, bNeg, ebNeg);
}

void TSnDualGainCalibSet::AddCalibAt(const Int_t smp,
                                     const TSnGainCalibVal& pos,
                                     const TSnGainCalibVal& neg) {
   if (HasCalibsFor(smp)) {
      ChangeCalibOf(smp, pos, neg);
   } else {
      PassCalibTo(smp, new TSnDualGainCalib(pos, neg));
   }
}

void TSnDualGainCalibSet::AddCalibAt(const Int_t smp,
                const Float_t mPos, const Float_t emPos,
                const Float_t bPos, const Float_t ebPos,
                const Float_t mNeg, const Float_t emNeg,
                const Float_t bNeg, const Float_t ebNeg) {
   if (HasCalibsFor(smp)) {
      ChangeCalibOf(smp,
                    mPos, emPos,
                    bPos, ebPos,
                    mNeg, emNeg,
                    bNeg, ebNeg);
   } else {
      PassCalibTo(smp, new TSnDualGainCalib(mPos, emPos,
                                            bPos, ebPos,
                                            mNeg, emNeg,
                                            bNeg, ebNeg));
   }
}

