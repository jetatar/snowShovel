#ifndef SNS_TSnAltTreeRandomLoader
#define SNS_TSnAltTreeRandomLoader

class TTree;
class TEntryList;
class TRandom3;

#include "TSnAltTreeLoader.h"

class TSnAltTreeRandomLoader : public TSnAltTreeLoader {
////////////////////////////////////////////////////////////////////////////////
// TSnAltTreeRandomLoader
//
// Load a completely random entry from the alternate tree, using the
// entry list if one has been specified. May load the same alternate
// entry multiple times.
////////////////////////////////////////////////////////////////////////////////

 private:
   UInt_t      fSeed;       // the random number seed to start from
   TRandom3*   fRand;       // random number generator. owned.
   
   Long64_t    fCurEntry;   // cache of the current entry
   Long64_t    fRndEntry;   // cache of the current random entry
   
 public:
   TSnAltTreeRandomLoader() : fSeed(4357), fRand(0),
                              fCurEntry(-1), fRndEntry(-1) {}
   TSnAltTreeRandomLoader(const Char_t* altTreeTag,
                          TTree* altTree,
                          const UInt_t seed,
                          TEntryList* el=0);
   virtual ~TSnAltTreeRandomLoader();

   virtual Long64_t  GetAltEntryFor(const Long64_t mainEntry);

   UInt_t            GetRandSeed() const { return fSeed; }

   void              SetRandSeed(const UInt_t s) { fSeed = s; }

   
   ClassDef(TSnAltTreeRandomLoader, 2); // load a random entry from an alternate tree
};

#endif // SNS_TSnAltTreeRandomLoader
