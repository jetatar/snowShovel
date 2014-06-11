#include "TSnAltTreeShuffleLoader.h"

#include "TSnMath.h"

#include <stdexcept>

ClassImp(TSnAltTreeShuffleLoader);

const Long64_t TSnAltTreeShuffleLoader::kMaxEntries = static_cast<Long64_t>(
   TMath::Power(2.0,30.0)
   / sizeof(TSnAltTreeShuffleLoader::Entry_t) ); // restrict to 1GB (gibibyte)


TSnAltTreeShuffleLoader::TSnAltTreeShuffleLoader(const Char_t* altTreeTag,
                                                 TTree* altTree,
                                                 TEntryList* el) :
   TSnAltTreeLoader(altTreeTag, altTree, el),
   fMakeList(kTRUE),
   fCurEntry(-1), fShfEntry(-1) {
   // normal ctor
}

void TSnAltTreeShuffleLoader::MakeList() {

   const Long64_t n = GetAltTreeEntries();
   for (Long64_t i=0, j=0; i<n; ++i) {
      const Long64_t e = GetEntryNumberFor(i);
      if (e>=0) {
         fEntList.push_back(TSnMath::SafeCast<Long64_t, Entry_t>(e));
         ++j;
         if (j==kMaxEntries) {
            throw std::runtime_error(
               Form("<TSnAltTreeShuffleLoader::MakeList>: "
                    "Cannot build available entry list with more than "
                    "[%lld] entries! Will use too much memory. Adjust "
                    "TSnAltTreeShuffleLoader::kMaxEntries if this is "
                    "absolutely necessary (and possible).",
                    kMaxEntries));
         }
      }
   }
   std::random_shuffle(fEntList.begin(), fEntList.end());
   fMakeList=kFALSE;
}

Long64_t TSnAltTreeShuffleLoader::GetAltEntryFor(const Long64_t mainEntry) {

   if (mainEntry!=fCurEntry) {
      if (fMakeList) {
         MakeList();
      }
      if (fEntList.empty()) {
         throw std::runtime_error(
            Form("<TSnAltTreeShuffleLoader::GetAltEntryFor>: "
                 "Cannot load alt tree entry for [%lld]; no more unused "
                 "entries in the alternate tree remain.", mainEntry));
      }
      
      const Long64_t e = GetEntryNumberFor( fEntList.back() );
      fEntList.pop_back();
      fShfEntry = e;
      fCurEntry = mainEntry;
   }
   return fShfEntry;
}

