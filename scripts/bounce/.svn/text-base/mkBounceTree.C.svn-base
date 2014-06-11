#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TSystem.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TGraphErrors.h>

#include <NSnConstants.h>
#endif

static const UChar_t kNextra = 57;

TFile* outf=0, * fpnf=0;
TTree* bt;
TChain* nt, * ns;
TGraphErrors* gPed[NSnConstants::kNchans]={0},
            * gNoise[NSnConstants::kNchans]={0};


void mkBounceTree(const Char_t* rtfn,
                  const Char_t* wvfn,
                  const Char_t* FPNfn,
                  const Char_t* outfn) {
   
   nt = new TChain("runtree");
   nt->Add(rtfn);

   const Long64_t nents = nt->GetEntries();
   if (nents==0) {
      Error("noAveBunceStdy","No events in tree from [%s].",rtfn);
      return;
   }

   ns = new TChain("nShifts");
   ns->Add(wvfn);

   if (nents>ns->GetEntries()) {
      Error("bounceStudy","%lld entries in runtree but "
            "%lld entries in shift tree",nents,ns->GetEntries());
      return;
   }
   
   fpnf = TFile::Open(FPNfn);
   if ( (fpnf==0) || (fpnf->IsZombie()) ) {
      Error("bounceStudy","Could not open FPN file [%s]",FPNfn);
      return;
   }
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      gPed[ch]   = dynamic_cast<TGraphErrors*>(
         fpnf->Get(Form("gExlPed_ch%d",ch)));
      gNoise[ch] = dynamic_cast<TGraphErrors*>(
         fpnf->Get(Form("gExlRms_ch%d",ch)));
      if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
         Error("bounceStudy",
               "Couldn't get FPN/noise graphs from [%s]",FPNfn);
         return;
      }
   }

   // read from tree into...
   Float_t  pedsubs[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  psshift[NSnConstants::kNchans][NSnConstants::kNsamps];
   UShort_t samples[NSnConstants::kNchans][NSnConstants::kNsamps];
   UInt_t   evnum, utime, utimeus, mbchksum;
   UShort_t extra[kNextra];
   Int_t dtms;
   memset(extra,0,kNextra*sizeof(UShort_t));
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("DTms",&dtms);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
   }
   nt->SetBranchAddress("extra",&(extra[0]));

   // shift for stop tree
   Int_t nsent;
   UInt_t nsevn;
   Int_t aveShift(0), aveLen(0);
   Int_t shift[NSnConstants::kNchans];
   Int_t len[NSnConstants::kNchans];
   ns->SetBranchAddress("Ent",&nsent);
   ns->SetBranchAddress("EvtNum",&nsevn);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      ns->SetBranchAddress(Form("shift%02d",ch),&(shift[ch]));
      ns->SetBranchAddress(Form("len%02d",ch),&(len[ch]));
   }
   ns->BuildIndex("EvtNum");

   // output
   TString hn;
   outf = TFile::Open(outfn,"recreate");
   
   bt = new TTree("bt","bounce tree - ped sub, stop shifted data");
   bt->Branch("mbChecksum",&mbchksum,"mbChecksum/i"); 
   bt->Branch("EvtNum",&evnum,"EvEvtNum/i");
   bt->Branch("unixTime",&utime,"unixTime/i");
   bt->Branch("unixTimeUS",&utimeus,"unixTimeUS/i");
   bt->Branch("DTms",&dtms,"DTms/I");
   TString hd;
   for (UShort_t j=0; j<NSnConstants::kNchans; j++) {
      hd = Form("psd%02hu",j);
      bt->Branch(hd.Data(),&(psshift[j][0]),
		 Form("%s[%hu]/F",hd.Data(),NSnConstants::kNsamps));
   }
   bt->Branch("extra",&(extra[0]),Form("extra[%hhu]/s",kNextra));
   bt->Branch("stopShift",&aveShift,"stopShift/I");
   bt->Branch("stopLen",&aveLen,"stopLen/I");
   bt->SetDirectory(outf);
   
   Long64_t ne(0);
   for (Long64_t ev=0; ev<nents; ++ev) {
      if ( (ev%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
                 ev, nents,
                 100.*static_cast<Float_t>(ev)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(ev);
      ne = ns->GetEntryNumberWithIndex(evnum);
      if (ne>-1) {
         ns->GetEntry(ne);
         
         // find the stop shift
         aveShift = aveLen = 0;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            aveShift += shift[ch];
            aveLen   += len[ch];
         }
         aveShift = TMath::Nint( static_cast<Float_t>(aveShift)/
                                 static_cast<Float_t>(NSnConstants::kNchans) );
         aveLen   = TMath::Nint( static_cast<Float_t>(aveLen)/
                                 static_cast<Float_t>(NSnConstants::kNchans) );
         const Int_t shiftStart = NSnConstants::kNsamps-1 - aveShift - aveLen;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            // subtract FPN
            const Double_t* pd = gPed[ch]->GetY();
            const UShort_t* sp = &(samples[ch][0]);
            Float_t*        ps = &(pedsubs[ch][0]);
            Float_t*        ph = &(psshift[ch][ shiftStart ]);
            for (Int_t sm=0; sm<NSnConstants::kNsamps;
                                  ++sm, ++pd, ++sp, ++ps, ++ph) {
               *ps = *sp - *pd;
               while ( (ph-&(psshift[ch][0])) >= NSnConstants::kNsamps) {
                  ph -= NSnConstants::kNsamps;
               }
               *ph = *ps;
            } // samp
         } // chan
         
         bt->Fill();
      }
   } // evt
   
   outf->Write();

}
