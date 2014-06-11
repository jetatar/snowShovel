#ifndef SNS_TSnAmpOnSet
#define SNS_TSnAmpOnSet

#include <TObject.h>

#include <vector>

class TSnAmpOnSet : public TObject {
 public:
   typedef std::vector< Bool_t >           AmpOnSet_t;
   
 private:
   AmpOnSet_t     fAmpsOn;
   
   void           DoSizeError(const UInt_t s) const;
   
 public:
   TSnAmpOnSet() {}
   virtual ~TSnAmpOnSet() {}
   
   const AmpOnSet_t& GetAmpsOn() const { return fAmpsOn; }
   Bool_t            IsAmpOn(const UInt_t ch) const
      { return fAmpsOn.at(ch); }
   
   void     SetAmpOn(const UInt_t ch,
                     const Bool_t on=kTRUE);
   
   UInt_t   GetNchans() const { return fAmpsOn.size(); }
   
   Bool_t   IsValidForDb() const;
   
   void     Print(Option_t* option="") const;
   
   template<typename T>
   T        GetAmpOnBitword() const {
      T ret = 0;
      const UInt_t n = fAmpsOn.size();
      if ( n > sizeof(T)*kBitsPerByte ) {
         DoSizeError(sizeof(T)*kBitsPerByte);
      } else {
         for (UInt_t i=0; i<n; i++) {
            if (fAmpsOn.at(i)) {
               ret |= BIT(i);
            }
         }
      }
      return ret;
   }
   
   ClassDef(TSnAmpOnSet, 1); // which amps are on
};

#endif // SNS_TSnAmpOnSet

