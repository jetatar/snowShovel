#ifndef SNS_TSnMakeShiftTree
#define SNS_TSnMakeShiftTree

#include <TString.h>
class TTree;

#include "TAModule.h"

class TSnStopPosnSet;
class TSnCalWvData;
class TSnEventHeader;
class TSnEventMetadata;

class TSnMakeShiftTree : public TAModule {
 private:
   TString        fOutTreeNm;    // name of the output tree
   TString        fOutTreeTitle; // title of the output tree
   TString        fDatBrNm;  // name of the input branch containing the data (fpn sub or amp out)
   TString        fStopNm;   // name of the input stop position object in the event (default: StopPositions)
   TString        fShiftNm;  // name of the output shifted TSnCalWvData (no default!)
   Int_t          fBufSz;        // buffer size for new branch(es)
   Int_t          fSplit;        // split level for new branch(es)

   // event data
   TTree*            fOt;           //  the output tree
   TSnCalWvData*     fDat;          //! the event data
   TSnStopPosnSet*   fStops;        //! the stop positions
   TSnCalWvData*     fShifted;      //! the shifted data
   TSnEventHeader*   fHdr;          //! the event header
   TSnEventMetadata* fMeta;         //! the event metadata

   
   TString GetSplitSafeBrNm(const TString& s) const;
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   virtual void SlaveTerminate();

 public:
   TSnMakeShiftTree() : fBufSz(262144), fSplit(1),
                        fOt(0), fDat(0), fShifted(0), fHdr(0), fMeta(0) {}
   TSnMakeShiftTree(const Char_t* name,
                    const Char_t* treeName,
                    const Char_t* treeTitle,
                    const Char_t* dataNm,
                    const Char_t* shiftNm,
                    const Char_t* stopNm="StopPositions",
                    const Int_t   bufsz=262144,
                    const Int_t   split=1) :
      TAModule(name, "make a tree to store evt data shifted by stop posns"),
      fOutTreeNm(treeName), fOutTreeTitle(treeTitle),
      fDatBrNm(dataNm), fStopNm(stopNm), fShiftNm(shiftNm),
      fBufSz(bufsz), fSplit(split),
      fOt(0), fDat(0), fShifted(0), fHdr(0), fMeta(0) {
      // normal ctor
   }
   virtual ~TSnMakeShiftTree();
                    

   ClassDef(TSnMakeShiftTree, 1); // make a tree storing an event, stop positions and shifted waveforms
};

#endif // SNS_TSnMakeShiftTree
