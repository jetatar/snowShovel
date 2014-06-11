#ifndef SNS_TSnComponent
#define SNS_TSnComponent

#include <TObject.h>
class TTimeStamp;

class TSnComponent : public TObject {
 private:
   TTimeStamp*    fInstallDate;  // the install date (can be null)
   TTimeStamp*    fRemovedDate;  // the removed date (can be null)
   
 public:
   TSnComponent() : fInstallDate(0), fRemovedDate(0) {}
   TSnComponent(const TTimeStamp& inst, const TTimeStamp& remv);
   virtual ~TSnComponent();
   
   virtual Bool_t IsValidForDb() const=0;
   
   const TTimeStamp*     GetInstallDate() const { return fInstallDate; }
   const TTimeStamp*     GetRemovedDate() const { return fRemovedDate; }
   
   Bool_t         HasInstallDate() const { return fInstallDate!=0; }
   Bool_t         HasRemovedDate() const { return fRemovedDate!=0; }

   void           SetInstallDate(const TTimeStamp& inst);
   void           SetRemovedDate(const TTimeStamp& remv);
   void           SetInstallDate(const UInt_t tloc,
                                 const Bool_t isUTC=kTRUE,
                                 const Int_t secOffset=0,
                                 const Bool_t dosDate=kFALSE);
   void           SetRemovedDate(const UInt_t tloc,
                                 const Bool_t isUTC=kTRUE,
                                 const Int_t secOffset=0,
                                 const Bool_t dosDate=kFALSE);
   

   virtual const Char_t* GetCompType() const=0; // the 'label' of the comp_type
   
   ClassDef(TSnComponent, 1); // ABC for a hardware component
};

#endif // SNS_TSnComponent
