#ifndef SNS_TSnIOEventFrame
#define SNS_TSnIOEventFrame

#include "TSnIOObject.h"
#include "TSnEvent.h"
#include "TSnCompressor.h"
#include "NSnConstants.h"

#include <stdexcept>

class TSnIOEventFrame : public TSnIOObject {
 public:
   static const UChar_t    kIOvers;
   static const UInt_t     kMaxSizeOf;

 private:
   template<typename T>
   static
   void UnpackData(const Char_t*& buf,
                   T* const data,
                   const UChar_t loseLSB,
                   const UChar_t loseMSB,
                   const T wvBaseline,
                   const UInt_t dlen) {
      // from the buffer (compressed) to data
      buf = TSnCompressor::UnpackData(buf,data,loseLSB,loseMSB,
                                       wvBaseline,dlen);
   }

   template<typename T>
   static
   void UnpackData(const TSnIOBuffer& buf,
                   T* const data,
                   const UChar_t loseLSB,
                   const UChar_t loseMSB,
                   const T wvBaseline,
                   const UInt_t dlen) {
      // from the buffer (compressed) to data
      const UInt_t sz = 
         TSnCompressor::SizeOfPackedData<T>(loseLSB, loseMSB, dlen);
      if (buf.HasSpaceFor(sz)) {
         const Char_t* b = &(*(buf.GetPos()));
         const Char_t* const ob = b;
         UnpackData(b, data, loseLSB, loseMSB, wvBaseline, dlen);
         buf.IncrementPos<const Char_t* const>(sz, ob, b);
      } else {
         throw std::runtime_error("<TSnIOEventFrame::UnpackData>: "
                                  "Buffer does not contain compressed event.");
      }
   }

   template<typename T>
   static
   void PackData(Char_t*& buf, const T* const data,
                 const UChar_t loseLSB, const UChar_t loseMSB,
                 const T wvBaseline, const UInt_t dlen) {
      // from the data to the buffer (compressed)
      buf = TSnCompressor::PackData(buf, data, loseLSB, loseMSB,
                                    wvBaseline, dlen);
   }

   template<typename T>
   static
   void PackData(TSnIOBuffer& buf,
                 const T* const data,
                 const UChar_t loseLSB,
                 const UChar_t loseMSB,
                 const T wvBaseline,
                 const UInt_t dlen) {
      // from the data to the buffer (compressed)
      const UInt_t sz = 
         TSnCompressor::SizeOfPackedData<T>(loseLSB, loseMSB, dlen);
      buf.PrepForAdditionOf(sz);
      Char_t* b = &(*(buf.GetPos()));
      const Char_t* const ob = b;
      PackData(b, data, loseLSB, loseMSB, wvBaseline, dlen);
      buf.IncrementPos<const Char_t* const>(sz, ob, b);
   }
   
 public:
   TSnIOEventFrame() {}
   virtual ~TSnIOEventFrame() {}
   
   static
   UInt_t         GetMaxSizeOf() { return kMaxSizeOf; }

   template<typename T>
   static
   UChar_t ReadFrom(T& buf,
                    TSnEvent& evt,
                    const UChar_t loseLSB,
                    const UChar_t loseMSB,
                    const UShort_t wvBaseline) {
      // read the (compressed) event from the buffer
      //
      // note that this will only work for byte arrays and TSnIOBuffer objects

      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(buf);
      
      if (Rv>0) {
	 evt.SetTime(TSnIOObject::ReadTimeFrom(buf));
	 evt.SetEvtNum(TSnIOObject::ReadFrom<UInt_t>(buf));
	 evt.SetDTms(TSnIOObject::ReadFrom<Int_t>(buf));
	 evt.SetTrgNum(TSnIOObject::ReadFrom<UInt_t>(buf));
	 evt.SetTrgInfo(TSnIOObject::ReadFrom<UShort_t>(buf));
         UnpackData(buf, TSnEventWhisperer::GetWvData(evt),
                    loseLSB, loseMSB, wvBaseline,
                    NSnConstants::kTotSamps);
	 evt.SetStationCRC(TSnIOObject::ReadFrom<UInt_t>(buf));
	 evt.CalcCRC();
	 
      } else {
         throw std::runtime_error(Form("<TSnIOEventFrame::ReadFrom>: "
                                       "Unknown I/O version [%hhu]",Rv));
      }
      return Rv;
   }

   static
   TSnIOBuffer  GetIOByteArray(const TSnEvent& evt,
                               const UChar_t loseLSB,
                               const UChar_t loseMSB,
                               const UShort_t wvBaseline,
                               const UChar_t Rv=TSnIOEventFrame::kIOvers) {
      // make an IO buffer and fill it
      TSnIOBuffer b;
      WriteTo(b, evt, loseLSB, loseMSB, wvBaseline, Rv);
      return b;
   }

   static
   UInt_t         BytesReadFrom(const TSnIOBuffer& b,
				TSnEvent& evt,
				const UChar_t loseLSB,
				const UChar_t loseMSB,
				const UShort_t wvBaseline,
                                Int_t& Rv) {
      const TSnIOBuffer::Pos_t op = b.GetPosVal();
      Rv = ReadFrom(b, evt, loseLSB, loseMSB, wvBaseline);
      return b.GetPosVal()-op;
   }
   
   template<typename T>
   static
   void WriteTo(T& buf,
                const TSnEvent& evt,
                const UChar_t loseLSB,
                const UChar_t loseMSB,
                const UShort_t wvBaseline,
                const UChar_t Rv=TSnIOEventFrame::kIOvers) {
      if (Rv>0) {
         TSnIOObject::WriteTo(buf, Rv);
         TSnIOObject::WriteTimeTo(buf, evt.GetTime());
         TSnIOObject::WriteTo(buf, evt.GetEvtNum());
         TSnIOObject::WriteTo(buf, evt.GetDTms());
         TSnIOObject::WriteTo(buf, evt.GetTrgNum());
         TSnIOObject::WriteTo(buf, evt.GetTrgInfo().GetTrgBits());
         PackData(buf, TSnEventWhisperer::GetWvData(evt),
                  loseLSB, loseMSB, wvBaseline,
                  NSnConstants::kTotSamps);
         evt.CalcCRC();
         TSnIOObject::WriteTo(buf, evt.GetCRC());
      } else {
         throw std::runtime_error(Form("<TSnIOEventFrame::WriteTo>: "
                                       "Unknown I/O version [%hhu]",Rv));
      }

   }
   
   static
   UInt_t SizeOf(const UChar_t rv, 
                 const UChar_t loseLSB, const UChar_t loseMSB);
   

   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const Char_t*& b, TSnEvent& evt, 
                     const UChar_t loseLSB, const UChar_t loseMSB,
                     const UShort_t wvBaseline);
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnEvent& evt, 
                     const UChar_t loseLSB, const UChar_t loseMSB,
                     const UShort_t wvBaseline);
   static
   void WriteToW(Char_t*& b, const TSnEvent& evt, 
                 const UChar_t loseLSB, const UChar_t loseMSB,
                 const UShort_t wvBaseline,
                 const UChar_t Rv=TSnIOEventFrame::kIOvers);
   static
   void WriteToW(TSnIOBuffer& b, const TSnEvent& evt, 
                 const UChar_t loseLSB, const UChar_t loseMSB,
                 const UShort_t wvBaseline,
                 const UChar_t Rv=TSnIOEventFrame::kIOvers);
   
   ClassDef(TSnIOEventFrame, 1); // read/write an event frame from/to a byte steram
};

#endif // SNS_TSnIOEventFrame
