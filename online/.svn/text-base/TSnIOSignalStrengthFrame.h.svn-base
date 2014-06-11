#ifndef SNS_TSnIOSignalStrengthFrame
#define SNS_TSnIOSignalStrengthFrame

#include "TSnIOObject.h"
#include "TSnSignalStrengthInfo.h"

#include <TString.h>

#include <stdexcept>

class TSnIOSignalStrengthFrame : public TSnIOObject {
 public:
   static const UChar_t kIOvers;
   static const UInt_t  kMaxSizeOf;
   
 public:
   TSnIOSignalStrengthFrame() {}
   virtual ~TSnIOSignalStrengthFrame() {}
   
   static
   UInt_t SizeOf(const UChar_t rv=TSnIOSignalStrengthFrame::kIOvers) {
      return kMaxSizeOf;
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const TSnSignalStrengthInfo& c,
                const UChar_t Rv=TSnIOSignalStrengthFrame::kIOvers) {
      if (Rv>0) {
         TSnIOObject::WriteTo(b, Rv);
         TSnIOObject::WriteTo(b, c.GetCommType());
         TSnIOObject::WriteTo(b, c.GetSigStr());
         TSnIOObject::WriteTimeTo(b, c.GetSigStrTime());
      } else {
         throw std::runtime_error(Form("<TSnIOSignalStrengthFrame::WriteTo>: "
            "Unknown i/o version [%hhu]", Rv));
      }
   }
   
   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TSnSignalStrengthInfo& c) {
      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(b);
      if (Rv>0) {
         c.SetCommType(TSnIOObject::ReadFrom<UChar_t>(b));
         c.SetSigStr(TSnIOObject::ReadFrom<Float_t>(b));
         c.SetSigStrTime(TSnIOObject::ReadTimeFrom(b));
      } else {
         throw std::runtime_error(Form("<TSnIOSignalStrengthFrame::ReadFrom>: "
            "Unknown i/o version [%hhu]", Rv));
      }
      return Rv;
   }

   static
   UInt_t  BytesReadFrom(const TSnIOBuffer& buf,
                         TSnSignalStrengthInfo& c,
                         Int_t& Rv) {
      const TSnIOBuffer::Pos_t op = buf.GetPosVal();
      Rv = ReadFrom(buf, c);
      return (buf.GetPosVal() - op);
   }
   
   static
   TSnIOBuffer GetIOByteArray(const TSnSignalStrengthInfo& c,
                         const UChar_t Rv=TSnIOSignalStrengthFrame::kIOvers) {
      TSnIOBuffer b;
      WriteTo(b, c, Rv);
      return b;
   }
   
   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnSignalStrengthInfo& p);
   static
   UChar_t ReadFromW(const Char_t*& b, TSnSignalStrengthInfo& p);
   static
   UChar_t ReadFromW(FILE*& b, TSnSignalStrengthInfo& p);
   static
   void WriteToW(TSnIOBuffer& b, const TSnSignalStrengthInfo& p,
                 const UChar_t Rv=TSnIOSignalStrengthFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const TSnSignalStrengthInfo& p,
                 const UChar_t Rv=TSnIOSignalStrengthFrame::kIOvers);
   static
   void WriteToW(FILE*& b, const TSnSignalStrengthInfo& p,
                 const UChar_t Rv=TSnIOSignalStrengthFrame::kIOvers);
   
   ClassDef(TSnIOSignalStrengthFrame, 1); // read a signal strength info frame
   
   
};

#endif // SNS_TSnIOSignalStrengthFrame
