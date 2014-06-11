#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TString.h>
#endif

TChain* nt;

void scanMin(const Char_t* infn) {
   nt = new TChain("tChanDelaysFit");
   nt->Add(infn);
   
   nt->SetBranchStatus("*",0);
   
   Float_t chi2;
   nt->SetBranchAddress("chi2",&chi2);
   nt->SetBranchStatus("chi2",1);
   
   nt->GetEntry(0);
   Float_t minchi2 = chi2;
   Long64_t minent = 0;
   
   const Long64_t nents = nt->GetEntries();
   for (Long64_t i=1; i<nents; ++i) {
      if ( (i%10000000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            i, nents,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      nt->GetEntries();
      if (chi2 < minchi2) {
         minchi2 = chi2;
         minent  = i;
      }
   }
   
   Printf("minchi2=%g at entry %lld",minchi2,minent);
}
