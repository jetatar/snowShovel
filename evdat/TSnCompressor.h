#ifndef SNS_TSnCompressor
#define SNS_TSnCompressor

#include <Rtypes.h>

#include <cstring>

class TSnCompressor {
 public:
   typedef UChar_t Byte_t;
   
   TSnCompressor() {}
   virtual ~TSnCompressor() {}
   
   template<typename T>
   static
   UInt_t SizeOfPackedData(const UChar_t loseLSB,
                           const UChar_t loseMSB,
                           const UInt_t  dlen) {
      // dlen = number of elements in the data array (NOT number of bytes)
      //
      // returns number of bytes (whatever their size in bits) of packed data
      const UChar_t p = (sizeof(T)*kBitsPerByte)-loseLSB-loseMSB;
      return ((p*dlen)/kBitsPerByte) + (((p*dlen)%kBitsPerByte)!=0 ? 1u : 0u);
   }
   
   template<typename T>
   static
   const Char_t* UnpackData(const Char_t* const buf,
                            T* const data,
                            const UChar_t loseLSB,
                            const UChar_t loseMSB,
                            const T wvBaseline,
                            const UInt_t dlen) {
      // type punt via union from Char_t* to Byte_t*
      // see UnpackData for more info
      union {
         const Char_t* c;
         const Byte_t* b;
      } t;
      t.c = buf;
      t.b = UnpackData(t.b, data, loseLSB, loseMSB, wvBaseline, dlen);
      return t.c;
   }

   template<typename T>
   static
   const Byte_t* UnpackData(const Byte_t* const buf,
                            T* const data,
                            const UChar_t loseLSB,
                            const UChar_t loseMSB,
                            const T wvBaseline,
                            const UInt_t dlen) {
      // Uncompress the data. See PackData for more info.
      //
      // buf = the byte array into which the data should be packed
      // data = the data to pack
      // loseLSB = number of least significant bits to throw away
      // loseMSB = number of most significant bits to throw away
      // wvBaseline = baseline to subtract to from ADC before packing
      // dlen = number of elements in the data array (NOT number of bytes)
      //
      // returns the position in the buffer after the data extraction
      
      if (loseLSB==0 && loseMSB==0 && wvBaseline==0) {
         memcpy(data, buf, dlen*sizeof(T));
      } else {
         
         const UChar_t bitsPerElem = sizeof(T)*kBitsPerByte;
         const UChar_t packSmpBits = bitsPerElem-loseLSB-loseMSB;
    
         // make sure data is all 0's to start
         memset(data, 0, dlen*sizeof(T));

         const Byte_t* b = buf;
         T* dev = data;
         T dum;
         Char_t sbit=0;
         for (UInt_t i=0; i<dlen; i++, dev++) {
            dum  = (*b) << kBitsPerByte;
            dum |= *(b+1);
            if (sbit<=0) {
               dum >>= (-sbit+loseLSB+loseMSB);
               dum <<= loseLSB;
               *dev |= dum;
               // add baseline and move to next number (dev++ in the for loop)
               *dev += wvBaseline;
               // but stay in this short of the buf (do not increment b)
               // move starting bit up
               sbit += packSmpBits;
            } else {
               dum <<= sbit;
               dum >>= loseMSB+loseLSB;
               dum <<= loseLSB;
               *dev = dum;
               if ( (sbit+packSmpBits) >= bitsPerElem ) {
                  b+=2;       // move to next short in the buf
                  i--; dev--; // but stay on this number
                  // move starting bit back into the short we just read from
                  sbit -= bitsPerElem;
               } else {
                  // add baseline and move to next number (dev++ in the for loop)
                  *dev += wvBaseline;
                  sbit += packSmpBits;
               }
            } 
         }
      }
    
      return buf+SizeOfPackedData<T>(loseLSB, loseMSB, dlen);
   }

   template<typename T>
   static
   Char_t* PackData(Char_t* const buf, const T* const data,
                    const UChar_t loseLSB, const UChar_t loseMSB,
                    const T wvBaseline, const UInt_t dlen) {
      // type punt via union from Char_t* to Byte_t*
      // see PackData for more info
      union {
         Char_t* c;
         Byte_t* b;
      } t;
      t.c = buf;
      t.b = PackData(t.b, data, loseLSB, loseMSB, wvBaseline, dlen);
      return t.c;
   }
   
   template<typename T>
   static
   Byte_t* PackData(Byte_t* const buf, const T* const data,
                    const UChar_t loseLSB, const UChar_t loseMSB,
                    const T wvBaseline, const UInt_t dlen) {
      // Compress the data. This is potentially LOSSY; it depends
      // on the dynamic range and on the options.
      // See SnConfigFrame::fWvLoseLSB and SnConfigFrame::fWvLoseMSB.
      // If the number of least signficant bits to lose is not 0, the
      // compression will be lossy (decreased resolution -- this is ok
      // if the noise of the signal is much greater than the resolution).
      // Losing the most significant bits will only be lossy if the
      // signal-SnConfigFrame::fWvBaseline cannot fit in the reduced
      // dynamic range (each MSB bit reducing the DR by a factor of 2).
      //
      // Note that the mbed uses little endian. Behavior should be the
      // same for read/writes on big endian, but this has not been tested.
      // A write with little endian and a read with big endian, or vice
      // versa, will almost certainly not work (but again not tested).
      //
      // Use an unsigned buffer to prevent bits being changed during
      // an implicit unsigned -> signed cast.
      //
      // buf = the byte array into which the data should be packed
      // data = the data to pack
      // loseLSB = number of least significant bits to throw away
      // loseMSB = number of most significant bits to throw away
      // wvBaseline = baseline to subtract to from ADC before packing
      // dlen = number of elements in the data array (NOT number of bytes)
      //
      // returns the position in the buffer after the packed data

      const UInt_t  blen = SizeOfPackedData<T>(loseLSB, loseMSB, dlen);
      const UChar_t bitsPerElem = sizeof(T)*kBitsPerByte;
      const UChar_t packSmpBits = bitsPerElem-loseLSB-loseMSB;

      // make sure this buffer space is all 0's to start
      memset(buf, 0, blen);
    
      const T clipHi = T( T( T(0xFFFFu >> loseLSB) << (loseLSB+loseMSB))
                          >> loseMSB);
    
      Byte_t* b = buf;
      const T* dev = data;
      T dum;
      Char_t sbit=0;
      for (UInt_t i=0; i<dlen; i++, dev++) {
         // dump the bits we don't want
         dum = (*dev) - wvBaseline;
         if (dum<clipHi) {
            dum >>= loseLSB;
            dum <<= (loseLSB+loseMSB);
         } else {
            dum = clipHi << loseMSB;
         }
         if (sbit<=0) {
            // lose MSB's put in previous short (or none if sbit==0)
            dum   <<= -sbit;
            *b     |= dum >> kBitsPerByte;   // "first" byte of the short
            dum   <<= kBitsPerByte;
            *(b+1) |= dum >> kBitsPerByte;   // "second"
            // move to next number (dev++ in the for loop)
            // move starting bit up
            // but stay in this byte of the buf (do not increment b)
            // since kPackSmpBits <= kBitsInShort, sbit can't
            // move past the end of the current two bytes
            sbit += packSmpBits;
         } else {
            // first few bits towards the end of this short
            dum   >>= sbit;
            *b     |= dum >> kBitsPerByte;   // "first" byte of the short
            dum   <<= kBitsPerByte;
            *(b+1) |= dum >> kBitsPerByte;   // "second"
            if ( (sbit+packSmpBits) >= bitsPerElem ) {
               b+=2;       // move to next short in the buf
               i--; dev--; // but stay on this number
               // move starting bit back into the short we just filled
               sbit -= bitsPerElem;
            } else {
               // stay in this buffer and move to the next number
               // move starting bit up
               sbit += packSmpBits;
            }
         }
      }
      
      return buf+blen;
   }
   
   ClassDef(TSnCompressor, 1); // compression routines
};

#endif // SNS_TSnCompressor
