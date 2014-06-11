#include "TSnPlaSet.h"

#include <TString.h>

#include <sstream>
#include <stdexcept>

ClassImp(TSnPlaSet);

const Int_t TSnPlaSet::kPLAPtnLen   = 8; // PLA length (string char length)


void TSnPlaSet::SetPla(const PlaSet_t::size_type pn,
                       const UShort_t pla) {
   if (fPlas.size()<=pn) {
      fPlas.resize(pn+1);
   }
   fPlas.at(pn) = pla;
   
}

void TSnPlaSet::AddPla(const UShort_t pla) {
   fPlas.push_back(pla);
}

Bool_t TSnPlaSet::IsValidForDb() const {
   const PlaSet_t::size_type np = fPlas.size();
   return (np>0) && (np<73);
}

void TSnPlaSet::Print(Option_t* option) const {
   printf("    ");
   PlaSet_t::const_iterator p, pend=fPlas.end();
   for (p=fPlas.begin(); p!=pend;) {
      printf("%hu (%s)", *p, GetPlaAsStr(*p));
      p++;
      if (p!=pend) {
         printf(", ");
      }
   }
   printf("\n");
}

void TSnPlaSet::GetHiLoPlaFromStr(const Char_t* const plastr,
                                  UShort_t& hiPla,
                                  UShort_t& loPla,
                                  const Bool_t r2l) {
   // split the PLA string into 2 bitwords: one for the high threshold
   // and one for the low threshold. "lows" in the string will become
   // "highs" in the low threshold PLA.
   // 
   // see comments in GetHiLoPlas and GetPlaFromStr
   
   const UShort_t pla = GetPlaFromStr(plastr, r2l);
   GetHiLoPlas(pla, hiPla, loPla, r2l);
   
}

void TSnPlaSet::GetHiLoPlas(const UShort_t pla,
                            UShort_t& hiPla,
                            UShort_t& loPla,
                            const bool r2l) {
   // split the PLA bitword into 2: one for the high threshold
   // and one for the low threshold. "lows" in the string will become
   // "highs" in the low threshold PLA.
   // 
   // example 1)
   // PLA string = HLHL....
   // hi thresh  = H.H.....
   // lo thresh  = .H.H....
   //
   // example 2)
   // PLA string = HBL.....
   // hi thresh  = HL......
   // lo thresh  = .LH.....
   //
   // (with . = A here, to make the example more readable)
   //
   // A = 11, B = 00
   // H = 01 or 10, alternating
   // L = 10 or 01, alternating
   // 01 at leftmost bits is H
   // for example:
   // 0x7FFF = 01 11 11 11 11 11 11 11
   // => HAAAAAAA for LEFT TO RIGHT
   // => AAAAAAAH for RIGHT TO LEFT
   // 0x56FF = 01 01 01 10 11 11 11 11
   // => HLHHAAAA for LEFT TO RIGHT
   // => AAAAHHLH for RIGHT TO LEFT
   //
   // so HHHHHHHH is
   // 01 10 01 10 01 10 01 10 always (r2l or l2r)
   //
   // r2l = whether to read bits right to left (true) or not (false)
   // Mahshid liked right to left
   // Liang liked left to right
   // so we allow for either
   
   static const Char_t bitsInShort = sizeof(UShort_t)*kBitsPerByte;
   
   const Char_t start = (r2l) ?           0 : bitsInShort-2;
   const Char_t end   = (r2l) ? bitsInShort : -2;
   const Char_t step  = (r2l) ?           2 : -2;
   
   UChar_t hi= (r2l) ? 0x2 : 0x1;
   UChar_t lo= (r2l) ? 0x1 : 0x2;
   
   // set all bits to 0
   hiPla = 0;
   loPla = 0;
   
   for (Char_t i=start; i!=end; i+=step, hi^=0x3, lo^=0x3) {
      const UChar_t b = (pla & (0x3<<i)) >> i;
      if (b==hi) {
         hiPla |=  hi << i;
         loPla |= 0x3 << i;
      } else if (b==lo) {
         hiPla |= 0x3 << i;
         loPla |=  hi << i;
      } else if (b==0x3) {
         // any
         hiPla |= 0x3 << i;
         loPla |= 0x3 << i;
      } else {
         // no check that b is something else.. should be impossible.
         // between
         hiPla |=  lo << i;
         loPla |=  lo << i;
      }
   }
   
}

const Char_t* TSnPlaSet::GetPlaAsStr(const UShort_t pla,
                                     const Bool_t r2l) {
   // build H/L/A/B string from bits
   // A=always, H=above high, L=below low, B=between (i.e. neither H nor L)
   // A = 11, B = 00
   // H = 01 or 10, alternating
   // L = 10 or 01, alternating
   // 01 at leftmost bits is H
   // for example:
   // 0x7FFF = 01 11 11 11 11 11 11 11
   // => HAAAAAAA for LEFT TO RIGHT
   // => AAAAAAAH for RIGHT TO LEFT
   // 0x56FF = 01 01 01 10 11 11 11 11
   // => HLHHAAAA for LEFT TO RIGHT
   // => AAAAHHLH for RIGHT TO LEFT
   //
   // so HHHHHHHH is
   // 01 10 01 10 01 10 01 10 always (r2l or l2r)
   //
   // r2l = whether to read bits right to left (true) or not (false)
   // Mahshid liked right to left
   // Liang liked left to right
   // so we allow for either
   
   static const Char_t bitsInShort = sizeof(UShort_t)*kBitsPerByte;
   
   const Char_t start = (r2l) ?           0 : bitsInShort-2;
   const Char_t end   = (r2l) ? bitsInShort : -2;
   const Char_t step  = (r2l) ?           2 : -2;
   
   UChar_t hi= (r2l) ? 0x2 : 0x1;
   UChar_t lo= (r2l) ? 0x1 : 0x2;
   
   std::stringstream s;
   for (Char_t i=start; i!=end; i+=step, hi^=0x3, lo^=0x3) {
      const UChar_t b = (pla & (0x3<<i)) >> i;
      if (b==hi) {
         s << 'H';
      } else if (b==lo) {
         s << 'L';
      } else if (b==0x3) {
         s << 'A';
      } else {
         // no check that b is something else.. should be impossible.
         s << 'B';
      }
   }
   return s.str().c_str();
}

UShort_t TSnPlaSet::GetPlaFromStr(const Char_t* const ps,
                                  const Bool_t r2l) {
   // turn H/L/A/B into bits
   // A=always, H=above high, L=below low, B=between (i.e. neither H nor L)
   // A = 11, B = 00
   // H = 01 or 10, alternating
   // L = 10 or 01, alternating
   // 01 at leftmost bits is H
   // for example:
   // HAAAAAAA = 01 11 11 11 11 11 11 11
   // => 0x7FFF for LEFT TO RIGHT (01 11 11 11 11 11 11 11)
   // => 0xFFFE for RIGHT TO LEFT (11 11 11 11 11 11 11 10)
   // HLHHAAAA = 01 01 01 10 11 11 11 11
   // => 0x56FF for LEFT TO RIGHT (01 01 01 10 11 11 11 11)
   // => 0xFF6A for RIGHT TO LEFT (11 11 11 11 01 10 10 10)
   //
   // r2l = whether to read bits right to left (true) or not (false)
   // Mahshid liked right to left
   // Liang liked left to right
   // so we allow for either
   
   const Char_t start = (r2l) ?          0 : kPLAPtnLen-1;
   const Char_t end   = (r2l) ? kPLAPtnLen :           -1;
   const Char_t step  = (r2l) ?          1 :           -1;
   
   const UChar_t hiadd = 1;
   const UChar_t loadd = 0;
   
   UShort_t pla=0x0000; // must be init'd to all 0's here!
   const Char_t* c = ps;
   for (Char_t i=start; i!=end; i+=step, c++) {
      switch (*c) {
         case 'H':
         case 'h':
            pla |= (0x0001<<((i+hiadd)%2)) << (i*2);
            break;
         case 'L':
         case 'l':
            pla |= (0x0001<<((i+loadd)%2)) << (i*2);
            break;
         case 'A':
         case 'a':
            pla |=  0x0003                 << (i*2);
            break;
         case 'N':
         case 'n':
         case 'B':
         case 'b':
            // do nothing; bits already zero
            break;
         default:
            throw std::runtime_error(Form(
               "<TSnPlaSet::GetPlaFromStr>: "
               "Unknown pattern requested:  c=[%c] at i=[%d] of [%s].",
               *c, i, ps));
      };
   }
   
   return pla;
}
