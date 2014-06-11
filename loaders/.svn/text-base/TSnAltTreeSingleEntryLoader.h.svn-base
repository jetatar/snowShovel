#ifndef SNS_TSnAltTreeSingleEntryLoader
#define SNS_TSnAltTreeSingleEntryLoader

#include "TSnAltTreeLoader.h"

class TSnAltTreeSingleEntryLoader : public TSnAltTreeLoader {
 private:
   Long64_t    fAltEntry; // the entry to load from the alternate tree

 public:
   TSnAltTreeSingleEntryLoader() : fAltEntry(0) {}
   TSnAltTreeSingleEntryLoader(const Char_t* altTreeTag,
                               TTree* altTree,
                               const Long64_t altEntry,
                               TEntryList* el=0) :
      TSnAltTreeLoader(altTreeTag, altTree, el),
      fAltEntry(altEntry) {
      // normal ctor
   }
   
   Long64_t GetAltEntryFor(const Long64_t mainEntry) {
      return GetEntryNumberFor(fAltEntry);
   }
   
   Long64_t GetAltTreeEntryNum() const { return fAltEntry; }
   
   void     SetAltTreeEntryNum(const Long64_t n) { fAltEntry = n; }
   
   ClassDef(TSnAltTreeSingleEntryLoader, 1); // load one entry (the same one) from the alternate tree for every entry of the main tree
};

#endif // SNS_TSnAltTreeSingleEntryLoader
