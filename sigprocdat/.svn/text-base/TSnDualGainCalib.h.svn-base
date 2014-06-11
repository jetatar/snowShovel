#ifndef SNS_TSnDualGainCalib
#define SNS_TSnDualGainCalib

#include "TSnCalibObject.h"
#include "TSnGainCalibVal.h"

class TSnDualGainCalib : public TSnCalibObject {
 private:
   TSnGainCalibVal fPosCal; // gain calib for non-negative ADCs
   TSnGainCalibVal fNegCal; // gain calib for negative ADCs
   
 public:
   TSnDualGainCalib() {}
   TSnDualGainCalib(const TSnGainCalibVal& pos,
                    const TSnGainCalibVal& neg)
      : fPosCal(pos), fNegCal(neg) {}
   TSnDualGainCalib(const Float_t mPos, const Float_t emPos,
                    const Float_t bPos, const Float_t ebPos,
                    const Float_t mNeg, const Float_t emNeg,
                    const Float_t bNeg, const Float_t ebNeg)
      : fPosCal(mPos, emPos, bPos, ebPos),
        fNegCal(mNeg, emNeg, bNeg, ebNeg) {}
   virtual ~TSnDualGainCalib() {}
   
   virtual void Print(Option_t* option = "") const;
   
   virtual Float_t Calibrate(const Float_t x) const {
      return (x<0) ? fNegCal.Calibrate(x) : fPosCal.Calibrate(x);
   }
   virtual Float_t CalibError(const Float_t x, const Float_t ex) const {
      return (x<0) ? fNegCal.CalibError(x,ex)
                   : fPosCal.CalibError(x,ex);
   }
   
   const TSnGainCalibVal& GetPosGainCalib() const { return fPosCal; }
         TSnGainCalibVal& GetPosGainCalib()       { return fPosCal; }
   const TSnGainCalibVal& GetNegGainCalib() const { return fNegCal; }
         TSnGainCalibVal& GetNegGainCalib()       { return fNegCal; }
   
   // use non-const ref getters to set the gain calibs
   
   ClassDef(TSnDualGainCalib, 1); // has a separate TSnGainCalibVal for pos and neg ADCs
};

#endif // SNS_TSnDualGainCalib
