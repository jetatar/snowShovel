#ifndef SNS_TSnIOBuffer
#define SNS_TSnIOBuffer

#include <Rtypes.h>
class TString;

#include <vector>
#include <iterator>
#include <stdexcept>
#include <cstddef>

class TSnCommObject;

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnIOBuffer
//
// A simple class to allow passing a vector<Char_t> along with a number that
// gives the 'current position' in the vector.
//
// The position is not very intelligent at all; it is changed only by certain
// functions (which should be obvious from their names).
//
// The position feature is only intended for behind the scenes use by
// TSnIOObject and TSnIOEventFrame. Other classes can simply call
// TSnIOObject::ReadFrom and WriteTo with the buffer, and all will be well:
// exceptions will be thrown if the buffer doesn't have the data to perform a
// read, and the buffer will be expanded in order to fit a write.
// 
////////////////////////////////////////////////////////////////////////////////


class TSnIOBuffer : public std::vector<Char_t> {
 public:
   typedef std::vector<Char_t>::size_type Pos_t;
   typedef std::vector<Char_t>::iterator  iterator;
   typedef std::vector<Char_t>::const_iterator const_iterator;
   
 private:
   mutable Pos_t fPos;
   
   TSnIOBuffer& AppendQuiet(const Char_t* const start,
                            const Char_t* const stop) {
      // append the array of bytes from start up to but excluding
      // endex
      //
      // does not advance position
      //
      // this renders all previously obtained iterators invalid!
      
      insert(end(), start, stop);
      return *this;
   }
    
 public:
   TSnIOBuffer() : fPos(0) {}
   TSnIOBuffer(const std::vector<Char_t>::size_type n) :
      std::vector<Char_t>(n),
      fPos(0) {
      // initialize a buffer of length n
   }
   TSnIOBuffer(const Char_t* const buf, 
               const std::vector<Char_t>::size_type n) :
      std::vector<Char_t>(buf, buf+n),
      fPos(0) {
      // initialize a buffer of length n and copy the contents
      // of buf into the buffer
   }
   TSnIOBuffer(const TString& buf);
   TSnIOBuffer(const TSnIOBuffer& buf) :
      std::vector<Char_t>(buf.GetPos(), buf.end()),
      fPos(0) {
      // initialize a buffer to buf[current pos, end]
      // does NOT (necessarily) copy the entire buffer 'buf'!
      //
      // use this constructor as a means to "shrink" an existing buffer
   }
   TSnIOBuffer(FILE* f, size_t bytes) :
      fPos(0) {
      // initialize a buffer by reading the first 'bytes' of
      // the file into the buffer
      ReadFrom(f, sizeof(Char_t), bytes);
   }
   TSnIOBuffer(FILE* f) :
      fPos(0) {
      // initialize a buffer by reading the entire file into
      // the buffer
      ReadFrom(f);
   }
   TSnIOBuffer(const TSnCommObject& co);
   virtual ~TSnIOBuffer() {}

   TSnIOBuffer& operator=(const TSnIOBuffer& o) {
      // assign using vector::opertaor= and copy value of fPos.
      // no need for copy and swap here.
      this->std::vector<Char_t>::operator=(o);
      fPos = o.fPos; // safe even if &o == this
      return *this;
   }

   size_t ReadFrom(FILE* f, size_t size, size_t count);
   size_t ReadFrom(FILE* f);
   
   size_t WriteTo(FILE* f, TSnIOBuffer::const_iterator p) const;
   size_t WriteTo(FILE* f) const { return WriteTo(f, GetPos()); }
   size_t WriteTo(const Char_t* fn, TSnIOBuffer::const_iterator p) const;
   size_t WriteTo(const Char_t* fn) const { return WriteTo(fn, GetPos()); }
   
   TSnIOBuffer& AppendQuiet(const TSnIOBuffer& b,
                            const Bool_t fromCurPos=kTRUE) {
      // append the buffer 'b' to this buffer. does not advance position
      //
      // if fromCurPos = true, append only from 'b's current position
      // if fromCurPos = false, append the entire buffer 'b'
      //
      // this renders all previously obtained iterators invalid!
      
      TSnIOBuffer::const_iterator bpos = (fromCurPos) ? b.GetPos() : b.begin();
      return AppendQuiet(&(*bpos), &(*b.end()));
   }
   
   TSnIOBuffer& AppendQuiet(const TSnIOBuffer& b,
                            const TSnIOBuffer::Pos_t start,
                            const TSnIOBuffer::Pos_t length) {
      // append part of the buffer b, starting at 'start' and taking
      // 'length' characters, to the end of this buffer.
      // does not advance position in buffer
      //
      // this renders all previously obtained iterators invalid!
       
      return AppendQuiet(b.begin()+start, length);
   }
   
   TSnIOBuffer& AppendQuiet(TSnIOBuffer::const_iterator s,
                            const TSnIOBuffer::Pos_t len) {
      // append the array of bytes 's' of length 'len' to the buffer.
      // does not advance position
      //
      // this renders all previously obtained iterators invalid!

      return AppendQuiet(&(*s), &(*s)+len);
   }
   
   TSnIOBuffer& AppendQuiet(const TString& s);
   
   std::vector<Char_t>::iterator       GetPos() 
      { return begin() + fPos; }
   std::vector<Char_t>::const_iterator GetPos() const
      { return begin() + fPos; }
   Pos_t                               GetPosVal() const
      { return fPos; }
   
   Pos_t  GetBytesRemaining() const {
      const Pos_t s = size();
      if ( fPos <= s ) {
         return (s - fPos);
      } else {
         throw std::runtime_error("<TSnIOBuffer::GetBytesRemaining>: "
                                  "Current position past end of buffer.");
         return 0;
      }
   }
   
   Bool_t HasSpaceFor(const TSnIOBuffer::Pos_t bytes) const {
      // check if the buffer has 'bytes' remaining beyond
      // the current position
      return ( bytes <= GetBytesRemaining() );
   }

   template<typename T>
   void IncrementPos(const TSnIOBuffer::Pos_t bytes,
                     const T bef,
                     const T aft) const {
      // check that the before and after pointers are 'bytes' apart
      // and increment the position if so. throw an error if not.
      //
      // 'bef' must be before (smaller) than 'aft'
      //
      // this function is used by the TSnIOObject::ReadFrom and
      // WriteTo functions, in which it is not guaranteed that
      // aft-bef = bytes
   
      if ( (aft<bef) || (bytes != static_cast<Pos_t>(aft-bef)) ) {
         throw std::runtime_error("<TSnIOBuffer::IncrementPos>: "
                                  "unexpected num bytes");
      } else {
         IncrementPos(aft-bef);
      }
   }

   template<typename T>
   void DecrementPos(const TSnIOBuffer::Pos_t bytes,
                     const T bef, const T aft) const {
      // check that the before and after pointers are 'bytes' apart
      // and decrement the position if so. throw an error if not.
      //
      // 'bef' must be after (greater) than 'aft'
      
      if ( (bef<aft) || (bytes != static_cast<Pos_t>(bef-aft)) ) {
         throw std::runtime_error("<TSnIOBuffer::Rollback>: "
                                  "unexpected num bytes");
      } else {
         DecrementPos(bytes);
      }
   }

   void IncrementPos(const TSnIOBuffer::Pos_t bytes) const {
      // move the current position up some bytes
      fPos += bytes;
   }

   void DecrementPos(const TSnIOBuffer::Pos_t bytes) const {
      // move the current position back some bytes
      fPos -= bytes;
   }
   
   void PrepForAdditionOf(const TSnIOBuffer::Pos_t bytes) {
      // if the buffer does not have 'bytes' remaining beyond
      // the current position, add expand as necessary
      
      const Pos_t s = size();
      if (s < fPos) {
         // position got out of sync. don't just move on.
         throw std::runtime_error("<TSnIOBuffer::PrepForAdditionOf>: "
                                  "current position is past end of buffer");
      } else {
         const Pos_t newend = fPos + bytes;
         if (newend > s) {
            resize(newend);
         }
      }
   }
   
   virtual void Print(Option_t* option = "") const;
   
   void SetPos(std::vector<Char_t>::const_iterator i)
      { fPos = (i - begin()); }
   void SetPosVal(const TSnIOBuffer::Pos_t p) { fPos = p; }

   // helper function for PyRoot
   const std::vector<Char_t>* AsVector() const { return this; }

   ClassDef(TSnIOBuffer, 1); 
};

#endif // SNS_TSnIOBuffer
