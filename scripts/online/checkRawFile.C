#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TSystem.h>
#include <TString.h>

#include "TSnClockSetInfo.h"
#include "TSnRunInfo.h"
#include "TSnCompMbed.h"
#include "TSnPowerReading.h"
#include "TSnIODataFile.h"
#include "TSnIOHeaderFrame.h"
#include "TSnIOClockSetFrame.h"
#include "TSnMath.h"

#include <cstdio>
#include <stdexcept>
#endif

Bool_t checkRawFile(const Char_t* infn,
                    const Int_t   clockIOversion) {
   
   Bool_t ok(kFALSE);

   FILE* inf = fopen(gSystem->ExpandPathName(infn),"rb");
   if (inf!=0) {
      
      // first check that the end of the file is the clock frame
      if ( (clockIOversion<0) || (clockIOversion>=kMaxUChar) ) {
         Printf("Invalid IO version [%d]",clockIOversion);
         return kFALSE;
      }
      const UInt_t csiz = TSnIOClockSetFrame::SizeOf(
         static_cast<UChar_t>(clockIOversion));
      fseek(inf, -csiz, SEEK_END);
      const UChar_t ciov = TSnIODataFile::PeekIOversion(inf);
      TSnClockSetInfo cend;
      if (clockIOversion==ciov) {
         const UInt_t cb = TSnIODataFile::ReadClockSet(inf, cend);
         if (cb==csiz) {
            //Printf("cend=%s",cend.GetCurrTime().AsString("C"));
            // end is ok. check that the start is ok and a different time.
            fseek(inf, 0, SEEK_SET);
            TSnRunInfo runf;
            TSnCompMbed mbed;
            UShort_t seq;
            TSnPowerReading pwr;
            Int_t vers;
            const UInt_t hb = TSnIODataFile::ReadFileHeader(inf,
                                                            runf, mbed, seq,
                                                            &pwr, vers);
            if (hb>0) {
               UChar_t hcode(0); // block code (see TSnIOHeaderFrame) 
               UInt_t  hlen(0);  // block length
               UInt_t  bytes(0);
               TSnClockSetInfo clkstart;
               do {
                  bytes = TSnIODataFile::ReadBlockHeader(inf, hcode, hlen);
                  if (bytes>0) {
                     if (hcode==TSnIOHeaderFrame::kFileTrgStrtCode) {
                        bytes = TSnIODataFile::ReadClockSet(inf, clkstart);
                        //Printf("cbeg=%s",clkstart.GetCurrTime().AsString("C"));
                        if ((bytes==csiz) &&
                            (clkstart.GetCurrTime().GetSec() != 
                             cend.GetCurrTime().GetSec())) {
                           // ok, it's good.
                           ok = kTRUE;
                        }
                        break; // only want the first one
                     } else {
                        fseek(inf, hlen, SEEK_CUR);
                     }
                  }
               } while ( (feof(inf)==0) && (ferror(inf)==0) && (bytes>0) );

            }
         }
      } else {
         throw std::invalid_argument(Form("Wrong IO version specified [%hhu]!"
                                          "File says [%hhu].",
                                          clockIOversion, ciov));
      }
      
      fclose(inf);
   }
   
   return ok;
}
