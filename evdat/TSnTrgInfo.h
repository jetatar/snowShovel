#ifndef SNS_TSnTrgInfo
#define SNS_TSnTrgInfo

#include <TString.h>

class TSnTrgInfo {
 public:
   enum ETrgBit_t {
      kThermal     = BIT(0),  // thermal trigger
      kForced      = BIT(1),  // forced trigger
      kExternal    = BIT(2),  // reserved for external, but impossible to set with current DAQ
      kAdcToMBflag = BIT(7)   // flag events that took too long to get data from dCards to MB
   };
   
 private:
   UShort_t       fTrgBits;   // trigger bit word
   
 public:
   TSnTrgInfo(const UShort_t b=0) : fTrgBits(b) {}
   virtual ~TSnTrgInfo() {}
   
   virtual void Print(Option_t* option="") const;
   void         PrintBits(const Bool_t endLine) const;
   
   static
   TString      GetBitsAsString(const UShort_t b);
   TString      GetBitsAsString() const { return GetBitsAsString(fTrgBits); }

   UShort_t    GetTrgBits() const { return fTrgBits; }
   Bool_t      IsTrig(const UShort_t b) const { return IsTrig(b,fTrgBits); }
   static
   Bool_t      IsTrig(const UShort_t testBits,
                      const UShort_t trigBits) {
      return ( (trigBits & testBits)!=0 );
   }
   
   Bool_t      IsThermal()        const { return IsTrig(kThermal); }
   Bool_t      IsForced()         const { return IsTrig(kForced); }
   Bool_t      IsAdcToMBFlagged() const { return IsTrig(kAdcToMBflag); }
   
   
   void        SetTrgBits(const UShort_t b)   { fTrgBits  =  b; }
   void        EnableTrig(const ETrgBit_t b)  { fTrgBits |=  b; }
   void        ToggleTrig(const ETrgBit_t b)  { fTrgBits ^=  b; }
   void        DisableTrig(const ETrgBit_t b) { fTrgBits &= ~b; }
   
   
   ClassDef(TSnTrgInfo, 1); // trigger bits and masks
};

#endif // SNS_TSnTrgInfo
