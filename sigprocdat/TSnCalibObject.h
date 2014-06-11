#ifndef SNS_TSnCalibObject
#define SNS_TSnCalibObject

#include <TObject.h>

class TSnCalibObject : public TObject {

 public:
   TSnCalibObject() {}
   virtual ~TSnCalibObject() {}
   
   virtual void	Print(Option_t* option = "") const = 0;
   
   virtual Float_t Calibrate(const Float_t x) const = 0;
   virtual Float_t CalibError(const Float_t x, const Float_t ex) const = 0;

   virtual void    CalibrateWithErr(const Float_t x, const Float_t ex,
                                    Float_t& y, Float_t& ey) const {
      // see Calibrate and CalibError
      y  = Calibrate(x);
      ey = CalibError(x, ex);
   }


   ClassDef(TSnCalibObject, 1); // ABC for object storing calibration parameters
};

#endif // SNS_TSnCalibObject
