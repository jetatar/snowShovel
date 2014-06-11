#ifndef SNS_TSnGainCalibVal
#define SNS_TSnGainCalibVal

#include "TSnCalibObject.h"

class TSnGainCalibVal : public TSnCalibObject {
 private:
   Float_t    fM;   // gain slope
   Float_t    fEM;  // error on gain slope
   Float_t    fB;   // offset
   Float_t    fEB;  // error on offset

 public:
   TSnGainCalibVal() :
      fM(0), fEM(0), fB(0), fEB(0) {}
   TSnGainCalibVal(const Float_t m, const Float_t em,
                   const Float_t b, const Float_t eb) :
      fM(m), fEM(em), fB(b), fEB(eb) {}
   virtual ~TSnGainCalibVal() {}
   
   virtual void Print(Option_t* option = "") const;
   
   Float_t GetSlope() const { return fM; }
   Float_t GetSlopeErr() const { return fEM; }
   Float_t GetOffset() const { return fB; }
   Float_t GetOFfsetErr() const { return fEB; }

   virtual Float_t Calibrate(const Float_t x) const { return ((fM*x) + fB); }
   virtual Float_t CalibError(const Float_t x, const Float_t ex) const;
   
   void    SetSlope(const Float_t m) { fM = m; }
   void    SetSlopeErr(const Float_t e) { fEM = e; }
   void    SetSlope(const Float_t m, const Float_t em) { fM = m; fEM = em; }
   void    SetOffset(const Float_t b) { fB = b; }
   void    SetOffsetErr(const Float_t e) { fEB = e; }
   void    SetOffset(const Float_t b, const Float_t eb) { fB = b; fEB = eb; }
   void    SetParameters(const Float_t m, const Float_t em,
                         const Float_t b, const Float_t eb)
      { fM = m; fEM = em; fB = b; fEB = eb; }

   ClassDef(TSnGainCalibVal, 1); // store linear gain calibration parameters (i.e. to calibrate digitizer chip readout or thresholds)
};

#endif // SNS_TSnGainCalibVal
