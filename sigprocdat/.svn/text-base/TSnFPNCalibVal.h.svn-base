#ifndef SNS_TSnFPNCalibVal
#define SNS_TSnFPNCalibVal

#include "TSnCalibObject.h"

class TSnFPNCalibVal : public TSnCalibObject {
 private:
   Float_t   fFpn;  // the FPN value on this sample
   Float_t   fRms;  // the RMS of the FPN on this sample
   Float_t   fN;    // num of evts used to find the FPN,RMS (to calc Gauss error)

 public:
   TSnFPNCalibVal() : fFpn(0), fRms(0), fN(0) {}
   TSnFPNCalibVal(const Float_t fpn, const Float_t rms, const Float_t n) :
      fFpn(fpn), fRms(rms), fN(n) {}
   virtual ~TSnFPNCalibVal() {}
   
   virtual void	   Print(Option_t* option = "") const;
   
   Float_t         GetFPN() const { return fFpn; }
   Float_t         GetRMS() const { return fRms; }
   Float_t         GetN()   const { return fN; }
   
   Float_t         GetFPNError() const;
   Float_t         GetRMSError() const;
   
   virtual Float_t Calibrate(const Float_t x) const;
   virtual Float_t CalibError(const Float_t, const Float_t ex) const;
   
   void            SetFPN(const Float_t n) { fFpn = n; }
   void            SetRMS(const Float_t n) { fRms = n; }
   void            SetN(const Float_t n)   { fN   = n; }
   
   ClassDef(TSnFPNCalibVal, 1); // store the FPN of a sample
};

#endif // SNS_TSnFPNCalibVal
