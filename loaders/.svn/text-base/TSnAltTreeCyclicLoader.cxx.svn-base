#include "TSnAltTreeCyclicLoader.h"

#include <TTree.h>

ClassImp(TSnAltTreeCyclicLoader);

TSnAltTreeCyclicLoader::TSnAltTreeCyclicLoader(const Char_t* altTreeTag,
                                               TTree* altTree,
                                               TEntryList* el,
                                               const Long64_t first,
                                               const Long64_t nalt) :
   TSnAltTreeLoader(altTreeTag, altTree, el),
   fFirst(first), fNumAlt(nalt) {
   
}

Long64_t  TSnAltTreeCyclicLoader::GetAltEntryFor(const Long64_t mainEntry) {
   // return mainEntry mapped to a cyclic range [fFirst, fFirst+fNumAlt)
   
   if (fNumAlt<0) {
      fNumAlt = GetAltTreeEntries();
   }
   
   return GetEntryNumberFor( (mainEntry % fNumAlt) + fFirst );
   
}


