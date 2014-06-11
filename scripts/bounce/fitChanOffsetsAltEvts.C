#include <TChain.h>
#include <TString.h>

#include "TAMSelector.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnAltTreeShuffleLoader.h"
#include "TSnTrgInfo.h"
#include "TSnSaveCalibDataMod.h"

void fitChanOffsetsAltEvts() {
    
   TChain* nt[2] = { new TChain(TSnSaveCalibDataMod::kCalEvtTreeNm),
                     new TChain(TSnSaveCalibDataMod::kCalEvtTreeNm) };
   nt[0]->Add("../caltrees/CalTree.RawTree.SnEvtsM0002F7F0C61Ar00035sALL.root");
   nt[1]->Add("../caltrees/CalTree.RawTree.SnEvtsM0002F7F0C61Ar00036sALL.root");
    
   TString alttag = "AltRun";
   
   TAMSelector* sel = new TAMSelector();

   TSnBasicEvtSelMod* bes[2] = { new TSnBasicEvtSelMod("BasicEvtSelMod"),
                                 new TSnBasicEvtSelMod(Form("%sBasicEvtSelMod",
                                                            alttag.Data())) };
   bes[0]->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   bes[0]->SetCheckCRC(true);
   bes[1]->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   bes[1]->SetCheckCRC(true);
   bes[1]->SetHeaderBranchName(Form("%s%s",
            alttag.Data(), bes[1]->GetHeaderBranchName().Data()));
   bes[1]->SetEvtWvfmBranchName(Form("%s%s",
            alttag.Data(), bes[1]->GetEvtWvfmBranchName().Data()));
   
   TSnAltTreeShuffleLoader* altloader = 
      new TSnAltTreeShuffleLoader(alttag.Data(), nt[1]);
   sel->AddLoader(altloader);
   
   sel->AddInput(bes[0]);
   sel->AddInput(bes[1]);
   nt[0]->Process(sel);
   
}

