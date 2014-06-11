#ifndef SNS_TSnIOObject
#define SNS_TSnIOObject

#include "TSnIOBuffer.h"

#include <TObject.h>
#include <TString.h>
#include <TTimeStamp.h>

#include <cstring>
#include <stdexcept>

class TSnIOObject : public TObject {
 private:
   
 public:
   
   //
   // memory safe I/O using TSnIOBuffer - use these!
   //

   template<typename T>
   static
   T ReadFrom(const TSnIOBuffer& buffer) {
      // read the object from the buffer if there is enough
      // room to do so.
      T x;
      const size_t s = sizeof(T);
      if (buffer.HasSpaceFor(s)) {
         std::vector<Char_t>::const_iterator buf = buffer.GetPos();
         const Char_t* b = &(*buf);
         const Char_t* const ob = b;
         x = ReadFrom<T>(b);
         buffer.IncrementPos(s, ob, b);
      } else {
         throw std::out_of_range("Not enough room in buffer to read data");
      }
      return x;
   }

   template<typename T>
   static
   void WriteTo(TSnIOBuffer& buffer,
                const T x) {
      // expand the buffer and write the object into it

      const size_t s = sizeof(T);
      buffer.PrepForAdditionOf(s);
      Char_t* b = &(*(buffer.GetPos()));
      const Char_t* const ob = b;
      WriteTo<T>(b, x);
      buffer.IncrementPos<const Char_t* const>(s, ob, b);
   }
   
   static
   TString ReadStrFrom(const TSnIOBuffer& buffer,
                       const UInt_t slen) {
      // read the string from the buffer if there is enough
      // room to do so.
      TString r;
      if (buffer.HasSpaceFor(slen)) {
         const Char_t* b = &(*(buffer.GetPos()));
         const Char_t* const ob = b;
         r = ReadStrFrom(b, slen);
         buffer.IncrementPos(slen, ob, b);
      } else {
         throw std::out_of_range("Not enough room in buffer to read string");
      }
      return r;
   }
   
   static
   void WriteBytesTo(TSnIOBuffer& buffer,
                     const Char_t* const str,
                     const UInt_t slen) {
      // expand the buffer and copy the byte str to it, with the specified slen
      
      buffer.PrepForAdditionOf(slen);
      Char_t* b = &(*(buffer.GetPos()));
      const Char_t* const ob = b;
      WriteBytesTo(b, str, slen);
      buffer.IncrementPos<const Char_t* const>(slen, ob, b);
   }
   
   template<typename T>
   static
   T PeekAt(const TSnIOBuffer& buffer) {
      // peek at the object/value from the buffer if there is enough
      // room to do so.
      const size_t s = sizeof(T);
      T x = ReadFrom<T>(buffer);
      buffer.DecrementPos(s);
      return x;
   }

   //
   // simple byte array I/O
   //

   template<typename T>
   static
   T ReadFrom(const Char_t*& buf) {
      // low level. use TSnIOBuffer methods instead!
      //
      // not a string to int conversion, but a byte copy
      T x;
      memcpy(&x, buf, sizeof(T));
      buf += sizeof(T);
      return x;
   }

   template<typename T>
   static
   void WriteTo(Char_t*& buf, const T x) {
      // low level. use TSnIOBuffer methods instead!
      //
      // not an int to string conversion, but a byte copy
      memcpy(buf, &x, sizeof(T));
      buf += sizeof(T);
   }
   
   
   static
   TString ReadStrFrom(const Char_t*& buf,
                       const UInt_t slen) {
      // low level. use TSnIOBuffer methods instead!
      //
      // just a byte copy. assumes Char_t is 1 byte
      Char_t x[slen];
      memcpy(x, buf, slen);
      buf += slen;
      return TString(x, slen);
   }
   
   static
   void WriteBytesTo(Char_t*& buf, const Char_t* const x,
                     const UInt_t slen) {
      // low level. use TSnIOBuffer methods instead!
      //
      // just a byte copy. assumes Char_t is 1 byte
      memcpy(buf, x, slen);
      buf += slen;
   }

   template<typename T>
   static
   T PeekAt(const Char_t*& buf) {
      // peek at the object/value from the buffer without
      // incrementing the pointer.
      //
      // low level. use TSnIOBuffer methods instead!
      //
      // not a string to int conversion, but a byte copy
      T x = ReadFrom<T>(buf);
      buf -= sizeof(T);
      return x;
   }
   
   //
   // C FILE I/O
   //
   
   template<typename T>
   static
   T ReadFrom(FILE* inf) {
      // not a string to int conversion, but a byte copy
      T x;
      const size_t cr = fread(&x, sizeof(T), 1u, inf);
      if (cr!=1u) {
         throw std::runtime_error(Form("<TSnIOObject::ReadFrom(FILE*)>: "
            "Read wrong number of elements [%u].", static_cast<UInt_t>(cr)));
      }
      return x;
   }

   template<typename T>
   static
   void WriteTo(FILE* inf, const T x) {
      // not an int to string conversion, but a byte copy
      const size_t cr = fwrite(&x, sizeof(T), 1u, inf);
      if (cr!=1u) {
         throw std::runtime_error(Form("<TSnIOObject::WriteTo(FILE*)>: "
            "Wrote wrong number of elements [%u].", static_cast<UInt_t>(cr)));
      }
   }
   
   static
   TString ReadStrFrom(FILE* inf,
                       const UInt_t slen) {
      Char_t x[slen];
      ReadBytesFrom(inf, x, slen);
      return TString(x, slen);
   }
   
   static
   Char_t* ReadBytesFrom(FILE* inf, Char_t* const x, const UInt_t slen) {
      // put the bytes in x. the array x must already be allocated
      const size_t cr = fread(x, sizeof(Char_t), slen, inf);
      if (cr!=slen) {
         throw std::runtime_error(Form("<TSnIOObject::ReadBytesFrom(FILE*)>: "
            "Read wrong number of characters [%u]. Expected [%u].",
            static_cast<UInt_t> (cr), slen));
      }
      return x+cr;
   }
   
   static
   void WriteBytesTo(FILE*& inf, const Char_t* const x,
                     const UInt_t slen) {
      const size_t cr = fwrite(x, sizeof(Char_t), slen, inf);
      if (cr!=slen) {
         throw std::runtime_error(Form("<TSnIOObject::WriteBytesTo(FILE*)>: "
            "Wrote wrong number of characters [%u]. Tried [%u].",
            static_cast <UInt_t> (cr), slen));
      }
   }

   template<typename T>
   static
   T PeekAt(FILE* inf) {
      // peek at the object/value from the file without
      // incrementing the current position in the file
      //
      // not a string to int conversion, but a byte copy
      const Int_t fpos = ftell(inf);
      T x = ReadFrom<T>(inf);
      fseek(inf, fpos, SEEK_SET);
      return x;
   }
   
   //
   // A catch-all to just call WriteBytesTo.
   //
   template<typename T>
   static
   void WriteStrTo(T& buffer,
                   const Char_t* const str,
                   const UInt_t slen) {
      // write the str to the buffer, with the specified slen
      // (note that no trailing \0 is added)
      // this is currently just a place-holder in case writing
      // a string ever becomes different from writing an
      // array of bytes.
      WriteBytesTo(buffer, str, slen);
   }
      
   //
   // These should be template specializations, but rootcint fails on that
   //
   
   static
   TTimeStamp ReadTimeFrom(const TSnIOBuffer& buf) {
      const UInt_t x = ReadFrom<UInt_t>(buf);
      return TTimeStamp(x);
   }
   
   static
   void WriteTimeTo(TSnIOBuffer& buf, const TTimeStamp& x) {
      WriteTo(buf, static_cast<UInt_t>(x.GetSec()));
   }
   
   static
   TTimeStamp ReadTimeFrom(const Char_t*& buf) {
      const UInt_t x = ReadFrom<UInt_t>(buf);
      return TTimeStamp(x);
   }
   
   static
   void WriteTimeTo(Char_t*& buf, const TTimeStamp& x) {
      WriteTo(buf, static_cast<UInt_t>(x.GetSec()));
   }
   
   static
   TTimeStamp ReadTimeFrom(FILE* inf) {
      const UInt_t x = ReadFrom<UInt_t>(inf);
      return TTimeStamp(x);
   }
   
   static
   void WriteTimeTo(FILE* inf, const TTimeStamp& x) {
      WriteTo(inf, static_cast<UInt_t>(x.GetSec()));
   }

   static
   Int_t TryIntCast(const UInt_t l);
   

   
   TSnIOObject() {}
   virtual ~TSnIOObject() {}

   
   // helper functions for PyROOT
   static
   Char_t    ReadFromChar(const TSnIOBuffer& buffer);
   static
   UChar_t   ReadFromUChar(const TSnIOBuffer& buffer);
   static
   Short_t   ReadFromShort(const TSnIOBuffer& buffer);
   static
   UShort_t  ReadFromUShort(const TSnIOBuffer& buffer);
   static
   Int_t     ReadFromInt(const TSnIOBuffer& buffer);
   static
   UInt_t    ReadFromUInt(const TSnIOBuffer& buffer);
   static
   Long64_t  ReadFromLong64(const TSnIOBuffer& buffer);
   static
   ULong64_t ReadFromULong64(const TSnIOBuffer& buffer);
   static
   Float_t   ReadFromFloat(const TSnIOBuffer& buffer);
   static
   Double_t  ReadFromDouble(const TSnIOBuffer& buffer);
   
   
   ClassDef(TSnIOObject, 1); // base class for objects that read/write from/to byte streams
};

#endif // SNS_TSnIOObject
