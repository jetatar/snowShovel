#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TChain.h>

#include "TSnSaveCalibDataMod.h"
#include "TSnEvtGuiMainFrame.h"
#include "TSnRawTreeMaker.h"
#endif

#include "loadtree.C"

TSnEvtGuiMainFrame* win(0);
TChain* nt(0);
TFile* pf(0);

void showEvtGui(const Char_t* treefns,
                const Char_t* plotdir,
                //const Char_t* treename=TSnSaveCalibDataMod::kCalEvtTreeNm,
                const Char_t* plottag="WvPlots.") {
   // treefns must match the tree(s) used to make plotfn
   // plotfn is the *directory* containing the output from waveformPlots.C
   
   const Bool_t ok = tryLoadDataTree(treefns, nt);
   
   if (ok) {
      delete pf;
      TString sinfn(treefns);
      TString ifn = sinfn.Data() + sinfn.Last('/') + 1;
      TString plotfn(Form("%s/%s%s",plotdir,plottag,ifn.Data()));
      pf = TFile::Open(plotfn.Data());
   
      delete win;
      win = new TSnEvtGuiMainFrame(nt, pf);
   }   
}
