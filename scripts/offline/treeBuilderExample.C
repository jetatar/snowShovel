#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnLoadFromFileMod.h"
#include "TSnFPNSubMod.h"
#include "TSnTreeBuilderMod.h"
// need to include the template parameters too!
#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnCalWvData.h"
#endif


void treeBuilderExample(const Char_t* rawTreeFilen,
                        const Char_t* fpnFilen,
                        const Char_t* outfilen) {
   
   TFile inf(rawTreeFilen);
   TTree* rawt = dynamic_cast<TTree*>(inf.Get(TSnRawTreeMaker::kETname));
   if (rawt!=0) {
      
      TAMSelector sel;
      sel.SetVerbosity(10);
      
      TSnLoadFromFileMod loadfpn("LoadFPN", fpnFilen, "FPNSet");
      TSnFPNSubMod fpnsub("FPNSubMod", "FPNSet");
      TSnTreeBuilderMod tbmod("TreeBuilder",
                              "nt","example fpn sub tree");
      tbmod.CopyBranchFromTree<TSnEventMetadata>(TSnRawTreeMaker::kEMtBrNm);
      tbmod.CopyBranchFromTree<TSnEventHeader>(TSnRawTreeMaker::kEHdBrNm);
      tbmod.AddBranchForEvtObj<TSnCalWvData>( fpnsub.GetCalDatName().Data(),
                                              "MyFpnSubBranch." );
      
      loadfpn.Add(&fpnsub);
      fpnsub.Add(&tbmod);
      
      sel.AddInput(&loadfpn);
      
      rawt->Process(&sel, "");
      
      TAMOutput* output = sel.GetModOutput();
      if (output!=0) {
         
         TFile outf(outfilen, "recreate");
         outf.cd();
         output->Write();
         
      } else {
         Printf("Could not get output");
      }
      
   } else {
      Printf("Could not get raw tree from [%s]", inf.GetName());
   }
   
}
