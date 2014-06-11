#ifndef SNS_TSnIOClockSetFrame
#define SNS_TSnIOClockSetFrame

#include "TSnIOObject.h"
#include "TSnClockSetInfo.h"

#include <TString.h>

#include <stdexcept>

class TSnIOClockSetFrame : public TSnIOObject {
 public:
   static const UChar_t kIOvers;
   static const UInt_t  kMaxSizeOf;
   
 public:
   TSnIOClockSetFrame() {}
   virtual ~TSnIOClockSetFrame() {}
   
   static
   UInt_t SizeOf(const UChar_t rv) {
      return kMaxSizeOf;
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const TSnClockSetInfo& c,
                const UChar_t Rv=TSnIOClockSetFrame::kIOvers) {
      if (Rv>0) {
         TSnIOObject::WriteTo(b, Rv);
         TSnIOObject::WriteTimeTo(b, c.GetPrevTime());
         TSnIOObject::WriteTimeTo(b, c.GetSetTime());
         TSnIOObject::WriteTimeTo(b, c.GetCurrTime());
         TSnIOObject::WriteTo(b, c.GetUSsinceSet());
      } else {
         throw std::runtime_error(Form("<TSnIOClockSetFrame::WriteTo>: "
            "Unknown i/o version [%hhu]", Rv));
      }
   }
   
   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TSnClockSetInfo& c) {
      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(b);
      if (Rv>0) {
         c.SetPrevTime(TSnIOObject::ReadTimeFrom(b));
         c.SetSetTime(TSnIOObject::ReadTimeFrom(b));
         c.SetCurrTime(TSnIOObject::ReadTimeFrom(b));
         c.SetUSsinceSet(TSnIOObject::ReadFrom<Int_t>(b));
      } else {
         throw std::runtime_error(Form("<TSnIOClockSetFrame::ReadFrom>: "
            "Unknown i/o version [%hhu]", Rv));
      }
      return Rv;
   }

   static
   UInt_t  BytesReadFrom(const TSnIOBuffer& buf,
                         TSnClockSetInfo& c,
                         Int_t& Rv) {
      const TSnIOBuffer::Pos_t op = buf.GetPosVal();
      Rv = ReadFrom(buf, c);
      return (buf.GetPosVal() - op);
   }
   
   static
   TSnIOBuffer GetIOByteArray(const TSnClockSetInfo& c,
                              const UChar_t Rv=TSnIOClockSetFrame::kIOvers) {
      TSnIOBuffer b;
      WriteTo(b, c, Rv);
      return b;
   }
   
   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnClockSetInfo& p);
   static
   UChar_t ReadFromW(const Char_t*& b, TSnClockSetInfo& p);
   static
   UChar_t ReadFromW(FILE*& b, TSnClockSetInfo& p);
   static
   void WriteToW(TSnIOBuffer& b, const TSnClockSetInfo& p,
                 const UChar_t Rv=TSnIOClockSetFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const TSnClockSetInfo& p,
                 const UChar_t Rv=TSnIOClockSetFrame::kIOvers);
   static
   void WriteToW(FILE*& b, const TSnClockSetInfo& p,
                 const UChar_t Rv=TSnIOClockSetFrame::kIOvers);
   
   ClassDef(TSnIOClockSetFrame, 1); // read a clock set info frame
   
   
};

#endif // SNS_TSnIOClockSetFrame
