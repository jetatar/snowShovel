#include "TSnAltTreeRandomLoader.h"

#include <TString.h>
#include <TRandom3.h>

#include <stdexcept>

ClassImp(TSnAltTreeRandomLoader);

TSnAltTreeRandomLoader::TSnAltTreeRandomLoader(const Char_t* altTreeTag,
                                               TTree* altTree,
                                               const UInt_t seed,
                                               TEntryList* el) :
   TSnAltTreeLoader(altTreeTag, altTree, el),
   fSeed(seed),
   fRand(new TRandom3(seed)),
   fCurEntry(-1), fRndEntry(-1) {
   // normal ctor
   //
   // setting the seed allows multiple branch loaders on a single tree
   // to each load data from the same (random) entry

}

TSnAltTreeRandomLoader::~TSnAltTreeRandomLoader() {
   delete fRand;
}

Long64_t TSnAltTreeRandomLoader::GetAltEntryFor(const Long64_t mainEntry) {
   // get a random entry number for the alternate tree. try to ensure
   // that multiple branch loaders will get the same event number by
   // only generating a random number if "mainEntry" is different from
   // the last time this function was called.
   
   if (GetAltTreeEntries()>0) {
      if (mainEntry!=fCurEntry) {
         fRndEntry = static_cast<Long64_t>(GetAltTreeEntries() * fRand->Rndm());
         fCurEntry = mainEntry;
      }
   } else {
      throw std::runtime_error(Form("<TSnAltTreeRandomLoader::GetAltEntryFor>: "
                                    "Cannot load alternate entry to "
                                    "mainEntry [%lld] when the alternate tree "
                                    "has [%lld] entries.",
                                    mainEntry, GetAltTreeEntries()));
   }
   return GetEntryNumberFor(fRndEntry);
}


