#ifndef SNS_TSnIOPowerFrame
#define SNS_TSnIOPowerFrame

#include <stdexcept>

#include <TString.h>

#include "TSnIOObject.h"
#include "TSnPowerReading.h"

class TSnIOPowerFrame : public TSnIOObject {
 public:
   static const UChar_t kIOvers;
   static const UInt_t  kMaxSizeOf;
   
 public:
   TSnIOPowerFrame() {}
   virtual ~TSnIOPowerFrame() {}
   
   static
   UInt_t   SizeOf(const UChar_t rv) { 
      if (rv==1) {
         return sizeof(UChar_t)+(2u*sizeof(UShort_t));
      } else {
         return kMaxSizeOf;
      }
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const TSnPowerReading& p,
                const UChar_t Rv=TSnIOPowerFrame::kIOvers) {
      TSnIOObject::WriteTo(b, Rv);
      if (Rv==1) {
         TSnIOObject::WriteTo(b, p.GetObsVadc1());
         TSnIOObject::WriteTo(b, p.GetObsVadc2());
      } else if (Rv>1) {
         TSnIOObject::WriteTo(b, p.GetAveV1());
         TSnIOObject::WriteTo(b, p.GetAveV2());
         TSnIOObject::WriteTo(b, p.GetRmsV1());
         TSnIOObject::WriteTo(b, p.GetRmsV2());
         TSnIOObject::WriteTimeTo(b, p.GetTimeStamp());
      } else {
         throw std::runtime_error(Form("<TSnIOPowerFrame::WriteTo>: "
            "Unknown i/o version [%hhu]", Rv));
      }
   }
   
   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TSnPowerReading& p) {
      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(b);
      if (Rv==1) {
         const UShort_t v1 = TSnIOObject::ReadFrom<UShort_t>(b);
         const UShort_t v2 = TSnIOObject::ReadFrom<UShort_t>(b);
         p.Set(v1, v2, 0, 0, 0);
      } else if (Rv>1) {
         p.SetAveV1(TSnIOObject::ReadFrom<Float_t>(b));
         p.SetAveV2(TSnIOObject::ReadFrom<Float_t>(b));
         p.SetRmsV1(TSnIOObject::ReadFrom<Float_t>(b));
         p.SetRmsV2(TSnIOObject::ReadFrom<Float_t>(b));
         p.SetTime(TSnIOObject::ReadTimeFrom(b));
      } else {
         throw std::runtime_error(Form("<TSnIOPowerFrame::ReadFrom>: "
            "Unknown i/o version [%hhu]", Rv));
      }
      return Rv;
   }
   
   static
   UInt_t   BytesReadFrom(const TSnIOBuffer& buf,
                          TSnPowerReading& p,
                          Int_t& Rv) {
      const TSnIOBuffer::Pos_t op = buf.GetPosVal();
      Rv = ReadFrom(buf, p);
      return buf.GetPosVal() - op;
   }

   static
   TSnIOBuffer GetIOByteArray(const TSnPowerReading& p,
                              const UChar_t Rv=TSnIOPowerFrame::kIOvers) {
      TSnIOBuffer b;
      WriteTo(b, p, Rv);
      return b;
   }

   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnPowerReading& p);
   static
   UChar_t ReadFromW(const Char_t*& b, TSnPowerReading& p);
   static
   UChar_t ReadFromW(FILE*& b, TSnPowerReading& p);
   static
   void WriteToW(TSnIOBuffer& b, const TSnPowerReading& p,
                 const UChar_t Rv=TSnIOPowerFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const TSnPowerReading& p,
                 const UChar_t Rv=TSnIOPowerFrame::kIOvers);
   static
   void WriteToW(FILE*& b, const TSnPowerReading& p,
                 const UChar_t Rv=TSnIOPowerFrame::kIOvers);
   
   ClassDef(TSnIOPowerFrame, 1); // read a power frame
};

#endif // SNS_TSnIOPowerFrame
