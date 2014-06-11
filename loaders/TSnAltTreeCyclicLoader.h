#ifndef SNS_TSnAltTreeCyclicLoader
#define SNS_TSnAltTreeCyclicLoader

class TTree;
class TEntryList;

#include "TSnAltTreeLoader.h"

class TSnAltTreeCyclicLoader : public TSnAltTreeLoader {
////////////////////////////////////////////////////////////////////////////////
// TSnAltTreeCyclicLoader
//
// Load entries from the other tree in order, cycling back through if
// the requests outnumber available entries
////////////////////////////////////////////////////////////////////////////////

 private:
   Long64_t   fFirst;   // the first alt tree entry to try to load (default: 0)
   Long64_t   fNumAlt;  // number of events to use from the alternate tree (i.e. use events fFirst to (fFirst+fNumAlt-1) ) (default: number of events in alternate tree)

 public:
   TSnAltTreeCyclicLoader() : fFirst(0), fNumAlt(-1) {}
   TSnAltTreeCyclicLoader(const Char_t* altTreeTag,
                          TTree* altTree,
                          TEntryList* el=0,
                          const Long64_t first=0,
                          const Long64_t nalt=-1);
   virtual ~TSnAltTreeCyclicLoader() {}

   virtual Long64_t  GetAltEntryFor(const Long64_t mainEntry);

   Long64_t    GetBeginEntry() const { return fFirst; }
   Long64_t    GetNumAltEntriesToUse() const { return fNumAlt; }
   
   void        SetBeginEntry(const Long64_t i) { fFirst = i; }
   void        SetNumAltEntriesToUse(const Long64_t n) { fNumAlt = n; }
   
   ClassDef(TSnAltTreeCyclicLoader, 1); // load entries from the other tree in order, cycling back through if the requests outnumber available entries
};


#endif // SNS_TSnAltTreeCyclicLoader
