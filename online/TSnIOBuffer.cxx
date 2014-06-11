#include "TSnIOBuffer.h"

#include "TSnCommObject.h"

#include <TString.h>

#include <cstdio>
#include <iostream>

ClassImp(TSnIOBuffer);

TSnIOBuffer::TSnIOBuffer(const TString& buf) :
   std::vector<Char_t>(buf.Data(), buf.Data() + buf.Length()),
   fPos(0) {
   // initialize a buffer to the contents of the string 'buf'
   // this gets called from pything when building a buffer
   // from a python string
}

TSnIOBuffer::TSnIOBuffer(const TSnCommObject& co) :
   fPos(0) {
   // a helper ctor for python, in which copying
   // the result of GetIOByteArray doesn't seem to work
   TSnIOBuffer b(co.GetIOByteArray());
   AppendQuiet(b, kFALSE);
   //SetPosVal(b.GetPosVal());
}

TSnIOBuffer& TSnIOBuffer::AppendQuiet(const TString& s) {
   // append the string to the buffer. for use from PyROOT.
   // does not advance position
   //
   // this renders all previously obtained iterators invalid!

   return AppendQuiet(s.Data(), s.Data() + s.Length());
}


size_t TSnIOBuffer::ReadFrom(FILE* f) {
   // read the whole file into the current position
   
   fseek(f, 0, SEEK_END);
   const Int_t nb = ftell(f);
   rewind(f);
   return ReadFrom(f, nb, 1);
}

size_t TSnIOBuffer::ReadFrom(FILE* f, size_t size, size_t count) {
   // reads from the file into the current position.
   
   PrepForAdditionOf(size*count*sizeof(Char_t));
   const size_t c = fread(&(*(GetPos())), size, count, f);
   if (c!=count) {
      if (feof(f)!=0) {
         TString er("<TSnIOBuffer::ReadFrom>: Reached end of file before [");
         er += count;
         er += "] [";
         er += size;
         er += "]-byte objs could be read. Found [";
         er += c;
         er += "].";
         throw std::runtime_error(er.Data());
      } else if (ferror(f)!=0) {
         throw std::runtime_error("<TSnIOBuffer::ReadFrom>: File error.");
      } else {
         TString er("<TSnIOBuffer::ReadFrom>: Failed to read [");
         er += count;
         er += "] [";
         er += size;
         er += "]-byte objs. Found [";
         er += c;
         er += "]. No ferror and not at EOF.";
         throw std::runtime_error(er.Data());
      }
   }
   return c;
}

size_t TSnIOBuffer::WriteTo(FILE* f, TSnIOBuffer::const_iterator p) const {
   // writes from the specified point of the buffer into the file
   return fwrite(&(*(p)), 1, size(), f);
}

size_t TSnIOBuffer::WriteTo(const Char_t* fn,
                            TSnIOBuffer::const_iterator p) const {
   FILE* f = fopen(fn, "ab");
   size_t s = 0;
   if (f!=0) {
      s = WriteTo(f, p);
   }
   fclose(f);
   return s;
}

void TSnIOBuffer::Print(Option_t* option) const {
   // print the contents of the buffer
   // if option = 'ascii', will print the bytes as ascii
   // otherwise prints as hex
   TString opt(option);
   opt.ToUpper();
   
   TString tp("%02x ");
   if ( opt.Contains("ASCII") ) {
      tp = "%c";
   }
   TSnIOBuffer::const_iterator c, e = end();
   for (c=begin(); c!=e; ++c) {
      printf(tp.Data(), static_cast<UChar_t>(*c));
   }
   std::cout << std::endl
             << "Current position is byte #"
             << GetPosVal() << ": ";
   if (GetPos()!=e) {
      printf(tp.Data(), static_cast<UChar_t>(*(GetPos())));
   } else {
      printf("empty buffer");
   }
   std::cout << std::endl;
   Printf("Size=%u, bytes remaining=%u",
          static_cast<UInt_t>(size()),
          static_cast<UInt_t>(GetBytesRemaining()));
}
