#ifndef SNS_TSnIOHeartbeatFrame
#define SNS_TSnIOHeartbeatFrame

#include "TSnIOObject.h"

#include <TString.h>
#include <stdexcept>

class TSnIOHeartbeatFrame : public TSnIOObject {
 public:
   static const UChar_t kIOvers;
   static const UInt_t  kMaxSizeOf;
   
   TSnIOHeartbeatFrame() {}
   virtual ~TSnIOHeartbeatFrame() {}
   
   static
   UInt_t         GetMaxSizeOf() { return kMaxSizeOf; }
   
   static
   UInt_t         SizeOf(const UChar_t rv) {
      // get the size of this version of the heartbeat frame
      //
      // NOTE: the size of the heartbeat frame should never become
      // the same as the size of V1 of this frame (8 bytes).
      // this is because V1 of the frame did not write out the
      // i/o version. so the size is the only handle to indicate
      // that V1 should be read. this is exploited by TSnRawTreeMaker.
      if (rv>1) {
         return kMaxSizeOf;
      } else {
         return kMaxSizeOf-sizeof(UChar_t);
      }
   }
   
   template<typename T>
   static
   UChar_t ReadFromV1(T& b,
                      TTimeStamp& time,
                      UInt_t&  num) {
      // V1 of this class did not write out the I/O
      // version. so this function must be called
      // explicitly when it is known that the file/buffer
      // contains V1. Sad face.
      time = TSnIOObject::ReadTimeFrom(b);
      num  = TSnIOObject::ReadFrom<UInt_t>(b);
      return 1;
   }

   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TTimeStamp& time,
                    UInt_t&  num) {
      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(b);
      if (Rv>1) {
         time = TSnIOObject::ReadTimeFrom(b);
         num  = TSnIOObject::ReadFrom<UInt_t>(b);
      } else {
         throw std::runtime_error(Form("<TSnIOHeartbeatFrame::ReadFrom>: "
                                       "Cannot read version [%hhu]. "
                                       "Use ReadFromV1?",
                                       Rv));
      }
      return Rv;
   }
   
   static
   UInt_t PyReadFrom(const TSnIOBuffer& buf,
                     TTimeStamp& time,
                     Int_t& num,
                     Int_t& Rv) {
      // for use in python, since UInt_t& binding does not work
      UInt_t n;
      const TSnIOBuffer::Pos_t op = buf.GetPosVal();
      Rv = ReadFrom(buf, time, n);
      num  = TryIntCast(n);
      return buf.GetPosVal() - op;
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const TTimeStamp& time,
                const UInt_t      num,
                const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers) {
      WriteTo(b, time.GetSec(), num, Rv);
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const UInt_t time,
                const UInt_t num,
                const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers) {
      if (Rv>1) {
         TSnIOObject::WriteTo(b, Rv);
         TSnIOObject::WriteTo(b, time);
         TSnIOObject::WriteTo(b, num);
      } else {
         throw std::runtime_error(Form("<TSnIOHeartbeatFrame::WriteTo>: "
                                       "Cannot read version [%hhu].",
                                       Rv));
      }
   }

   static
   TSnIOBuffer GetIOByteArray(const UInt_t time,
                              const UInt_t num,
                              const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers) {
      // make an IO buffer and fill it
      TSnIOBuffer b;
      WriteTo(b, time, num, Rv);
      return b;
   }

   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TTimeStamp& time, UInt_t& num);
   static
   UChar_t ReadFromW(const Char_t*& b, TTimeStamp& time, UInt_t& num);
   static
   UChar_t ReadFromW(FILE*& b, TTimeStamp& time, UInt_t& num);
   static
   void WriteToW(TSnIOBuffer& b, const TTimeStamp& time, const UInt_t num,
                 const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const TTimeStamp& time, const UInt_t num,
                 const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers);
   static
   void WriteToW(FILE*& b, const TTimeStamp& time, const UInt_t num,
                 const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers);
   static
   void WriteToW(TSnIOBuffer& b, const UInt_t time, const UInt_t num,
                 const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const UInt_t time, const UInt_t num,
                 const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers);
   static
   void WriteToW(FILE*& b, const UInt_t time, const UInt_t num,
                 const UChar_t Rv=TSnIOHeartbeatFrame::kIOvers);
   
   ClassDef(TSnIOHeartbeatFrame, 1); // read/write a heartbeat frame from/to a byte stream
};

#endif // SNS_TSnIOHeartbeatFrame
