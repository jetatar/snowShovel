#ifndef SNS_TSnAltTreeShuffleLoader
#define SNS_TSnAltTreeShuffleLoader

class TTree;
class TEntryList;

#include "TSnAltTreeLoader.h"

#include <vector>

class TSnAltTreeShuffleLoader : public TSnAltTreeLoader {
////////////////////////////////////////////////////////////////////////////////
// TSnAltTreeShuffleLoader
//
// Load entries at random from a list of available entries, without
// repeating. An exception will be thrown if all possible entries are
// exhausted.
//
// Note that for a large number of entries, this loader can use a lot
// of memory! A maximum number of entries has been set to prevent the
// use of more than 1GB of memory.
////////////////////////////////////////////////////////////////////////////////

 public:
   typedef      UInt_t   Entry_t;
   static const Long64_t kMaxEntries; // max number of entries possible

 private:
   std::vector<Entry_t>  fEntList;   // list of possible entries
   Bool_t                fMakeList;  // whether the list needs to be made
   
   Long64_t              fCurEntry;  // cache the current entry
   Long64_t              fShfEntry;  // cache the shuffled entry in case it needs to be sent to more than one branch
   
   void MakeList();

 public:
   TSnAltTreeShuffleLoader() : fMakeList(kTRUE),
                               fCurEntry(-1), fShfEntry(-1) {}
   TSnAltTreeShuffleLoader(const Char_t* altTreeTag,
                           TTree* altTree,
                           TEntryList* el=0);
   virtual ~TSnAltTreeShuffleLoader() {}
   
   const std::vector<Entry_t>& GetAvailEntryList() const { return fEntList; }
   // intentionally no non-const getter or setters for the entry list
   
   virtual Long64_t  GetAltEntryFor(const Long64_t mainEntry);
   

   ClassDef(TSnAltTreeShuffleLoader, 1); // load entries from a shuffled list of available entries
};

#endif // SNS_TSnAltTreeShuffleLoader
