#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>

#include "TSnRawTreeMaker.h"
#endif

void makeRawTree(const Char_t* rawfilename,
                 const Char_t* outdir) {
   
   TSnRawTreeMaker maker(rawfilename, outdir);
   
   Printf("Making file [%s].",maker.GetTreeFilename().Data());
   
   maker.ProcFile();
   
   Printf("Wrote: %lld events, %lld power readings, %lld heartbeats.",
          maker.GetNevtsWritten(),
          maker.GetNpwrsWritten(),
          maker.GetNhrtsWritten());
   
}

