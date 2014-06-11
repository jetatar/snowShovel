#ifndef SNS_TSnIOStatusFrame
#define SNS_TSnIOStatusFrame

#include <TError.h>

#include "TSnIOObject.h"
#include "TSnStatusUpdate.h"
#include "TSnEvent.h"
#include "TSnIOEventFrame.h"
#include "TSnDAQConfig.h"

#include <stdexcept>

class TSnIOStatusFrame : public TSnIOObject {
 public:
   static const UChar_t kIOvers;
   static const UInt_t  kMaxSizeOfV1;
   static const UInt_t  kMaxSizeOfV2;
   static const UInt_t  kMaxSizeOfV3;
   static const UInt_t  kMaxSizeOfV4;
   static const UInt_t  kMaxSizeOfV5;
   static const UInt_t  kMaxSizeOf;

   static const UChar_t k2012to2013IOVers = 3; // i/o vers used 2012-2013
   
 public:
   TSnIOStatusFrame() {}
   virtual ~TSnIOStatusFrame() {}
   
   static
   UInt_t         GetMaxSizeOf(const UChar_t rv);
   
   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TSnStatusUpdate& stat,
                    TSnEvent& evt) {
      return ReadFrom(b, stat, &evt);
   }

   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TSnStatusUpdate& stat,
                    TSnEvent* const evt=0) {
      // read the status from the buffer
      //
      // returns the IO version read from the buffer
      
      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(b);
      if (Rv>0) {
         stat.SetMacAddress(TSnIOObject::ReadFrom<ULong64_t>(b));
         const UInt_t llen = TSnIOObject::ReadFrom<UInt_t>(b);
         stat.SetConfLabel(TSnIOObject::ReadStrFrom(b, llen).Data());
         stat.SetRunNum(TSnIOObject::ReadFrom<UInt_t>(b));
         if (Rv>1) {
            stat.SetSeqNum(TSnIOObject::ReadFrom<UShort_t>(b));
         }
         stat.SetStatusTime(TSnIOObject::ReadTimeFrom(b));
         stat.SetWvLoseLSB(TSnIOObject::ReadFrom<UChar_t>(b));
         stat.SetWvLoseMSB(TSnIOObject::ReadFrom<UChar_t>(b));
         stat.SetWvBaseline(TSnIOObject::ReadFrom<UShort_t>(b));
         if (Rv==1) {
            if (evt!=0) {
               TSnIOEventFrame::ReadFrom(b, *evt, 
                                         stat.GetWvLoseLSB(), 
                                         stat.GetWvLoseMSB(),
                                         stat.GetWvBaseline());
            } else {
               throw std::runtime_error(Form(
                                           "<TSnIOStatusFrame::ReadFrom>: "
                                           "Read version [%hhu] requires "
                                           "event object.",Rv));
            }
         }
         if (Rv>1) {
            stat.SetThmRate(TSnIOObject::ReadFrom<Float_t>(b));
            stat.SetEvtRate(TSnIOObject::ReadFrom<Float_t>(b));
         }
         if (Rv==3) {
            stat.SetObsNfiles(TSnIOObject::ReadFrom<UInt_t>(b));
            stat.SetObsTotKB(TSnIOObject::ReadFrom<Float_t>(b));
         }
	 if (Rv>3) {
            stat.SetFreeMB(TSnIOObject::ReadFrom<Float_t>(b));
         }
         if (Rv>4) {
            stat.SetWDReset(TSnIOObject::ReadFrom<UChar_t>(b));
         }
      } else {
         throw std::runtime_error(Form("<TSnIOStatusFrame::ReadFrom>: "
                                       "Unknown I/O version [%hhu]",Rv));
      }
      return Rv;
   }
   
   static
   UInt_t  BytesReadFrom(const TSnIOBuffer& b,
			 TSnStatusUpdate& stat,
			 TSnEvent& evt,
                         Int_t& Rv) {
      // for use in python
      const TSnIOBuffer::Pos_t op = b.GetPosVal();
      Rv = ReadFrom(b, stat, evt);
      return b.GetPosVal() - op;
   }

   inline
   static
   TSnIOBuffer GetIOByteArray(const TSnStatusUpdate& stat,
                              const UChar_t Rv=TSnIOStatusFrame::kIOvers) {
      // make an IO buffer and fill it.
      TSnIOBuffer b;
      WriteTo(b, stat, Rv);
      return b;
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const TSnStatusUpdate& stat,
                const UChar_t Rv=TSnIOStatusFrame::kIOvers) {
      if (Rv>0) {
         TSnIOObject::WriteTo(b, Rv);
         TSnIOObject::WriteTo(b, stat.GetMacAddress());
         TSnIOObject::WriteTo(b, stat.GetConfLabelLen());
         TSnIOObject::WriteStrTo(b, stat.GetConfLabel(), stat.GetConfLabelLen());
         TSnIOObject::WriteTo(b, stat.GetRunNum());
         if (Rv>1) {
            TSnIOObject::WriteTo(b, stat.GetSeqNum()); // v2
         }
         TSnIOObject::WriteTimeTo(b, stat.GetStatusTime());
         TSnIOObject::WriteTo(b, stat.GetWvLoseLSB());
         TSnIOObject::WriteTo(b, stat.GetWvLoseMSB());
         TSnIOObject::WriteTo(b, stat.GetWvBaseline());
         if (Rv==1) {
            // write an empty event for version 1
            TSnEvent e;
            TSnIOEventFrame::WriteTo(b, e,
                                     stat.GetWvLoseLSB(), 
                                     stat.GetWvLoseMSB(),
                                     stat.GetWvBaseline()); // v1
         }
         if (Rv>1) {
            TSnIOObject::WriteTo(b, stat.GetThmRate()); // v2
            TSnIOObject::WriteTo(b, stat.GetEvtRate()); // v2
         }
         if (Rv==3) {
            TSnIOObject::WriteTo(b, stat.GetObsNfiles()); // v3
            TSnIOObject::WriteTo(b, stat.GetObsTotKB()); // v3
         }
         if (Rv>3) {
            TSnIOObject::WriteTo(b, stat.GetFreeMB()); // v4
         }
         if (Rv>4) {
            TSnIOObject::WriteTo(b, stat.GetWDReset()); // v5
         }
      } else {
         throw std::runtime_error(Form("<TSnIOStatusFrame::WriteTo>: "
                                       "Unknown I/O version [%hhu]",Rv));
      }
   } 
  
   static
   UInt_t         SizeOf(const UChar_t rv,
                         const UInt_t confLblLen,
                         const UChar_t loseLSB,
                         const UChar_t loseMSB);
   
   static
   UInt_t         SizeOf(const UChar_t rv, const TSnDAQConfig& conf);


   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnStatusUpdate& stat,TSnEvent& evt);
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnStatusUpdate& stat,TSnEvent* evt);
   static
   UChar_t ReadFromW(const Char_t*& b, TSnStatusUpdate& stat,TSnEvent& evt);
   static
   UChar_t ReadFromW(const Char_t*& b, TSnStatusUpdate& stat,TSnEvent* evt);
   static
   void WriteToW(TSnIOBuffer& b, const TSnStatusUpdate& stat,
                 const UChar_t Rv=TSnIOStatusFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const TSnStatusUpdate& stat,
                 const UChar_t Rv=TSnIOStatusFrame::kIOvers);

   ClassDef(TSnIOStatusFrame, 1); // read/write a status frame from/to a byte stream
};

#endif // SNS_TSnIOStatusFrame
