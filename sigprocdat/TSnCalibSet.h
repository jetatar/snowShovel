#ifndef SNS_TSnCalibSet
#define SNS_TSnCalibSet

#include <TNamed.h>
class TObjArray;

class TSnCalibObject;

class TSnCalibSet : public TNamed {
 private:
   TObjArray*  fCalibs; //-> a list of TSnCalibObject's. indexing depends on type of calib set
   
   // map from sample number to index in fCalibs
   // (calibs may be on per sample basis or per card basis, or...)
   virtual Int_t GetCalibIndex(const Int_t smp) const = 0;

 protected:
   void PassCalibTo(const Int_t smp, TSnCalibObject* vals);
   
 public:
   TSnCalibSet() : fCalibs(0) {}
   TSnCalibSet(const Char_t* name, const Char_t* title,
               const Int_t n);
   TSnCalibSet(const TSnCalibSet& c);
   virtual ~TSnCalibSet();

   virtual TSnCalibSet* NewCopy() const = 0;
   
   static
   void         Swap(TSnCalibSet& a, TSnCalibSet& b);
   
   TSnCalibSet& operator=(const TSnCalibSet& x) {
      TSnCalibSet* o = x.NewCopy();
      Swap(*this, *o);
      delete o;
      return *this;
   }
   
   virtual void Print(Option_t* option = "") const;
   
   const TSnCalibObject& GetCalibFor(const Int_t smp) const;
         TSnCalibObject& GetCalibFor(const Int_t smp);
   
   Bool_t HasCalibsFor(const Int_t smp) const;
   
   Float_t Calibrate(const Int_t smp, const Float_t x) const;
   Float_t CalibError(const Int_t smp, const Float_t x, const Float_t ex) const;
   void    CalibrateWithErr(const Int_t smp,
                            const Float_t x, const Float_t ex,
                            Float_t& y, Float_t& ey) const;
   
   void ChangeCalibOf(const Int_t smp, const TSnCalibObject& vals);
   void AddCalibAt(const Int_t smp, const TSnCalibObject& vals);
   
   
   ClassDef(TSnCalibSet, 1); // ABC to store a list of calibrations

};

#endif // SNS_TSnCalibSet
