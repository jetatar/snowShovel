#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TEventList.h>
#endif

static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;

TChain* nt;
TTree* ot;
TFile* outf, * elf;
TEventList* el;

void copyChain(const Char_t* infn, 
               const Char_t* elfile,
               const Char_t* elname,
               const Char_t* outfn,
               const Bool_t  isEvtNums) {
   // isEvtNums true => event list stores event numbers
   //          false => event list stores chain entry numbers
   
   
   // open the tree
   nt = new TChain("nt");
   nt->Add(infn);
   
   // open the event list
   elf = TFile::Open(elfile);
   if ( (elf==0) || (elf->IsZombie()) ) {
      Printf("Could not open event list file [%s]",elfile);
      return;
   }
   el = dynamic_cast<TEventList*>(elf->Get(elname));
   if (el==0) {
      Printf("Could not get [%s] from file [%s]",elname,elfile);
      return;
   }
   const Int_t nents = el->GetN();
   Printf("Entries in event list = %d",nents);
   Printf("Entries in   chain    = %lld",nt->GetEntries());
   
   // open the output file
   outf = TFile::Open(outfn,"recreate");
   
   Printf("Setting up branches...");
   UInt_t utime, utimeus, evtnum, mbchksum, hdevtnum;
   UShort_t extra[kNextra];
   UShort_t samples[kNchans][kNsamps];
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("EvtNum",&evtnum);
   nt->SetBranchAddress("HdEvtNum",&hdevtnum);
   nt->SetBranchAddress("extra",&(extra[0]));
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
   }
   
   if (isEvtNums) {
      nt->BuildIndex("EvtNum");
   }
   
   // make a clone of the tree (all the branches, none of the events)
   outf->cd();
   ot = nt->CloneTree(0);
   
   Printf("Copying...");
   for (Int_t i=0; i<nents; i++) {
      
      if ( (i%500)==0 ) {
         fprintf(stderr,"Processing %d / %d  (%02.2f%%)            \r",
            i, nents,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      
      if (isEvtNums) {
         nt->GetEntryWithIndex( el->GetEntry(i) );
      } else {
         nt->GetEntry( el->GetEntry(i) );
      }
      ot->Fill();
   }
   
   outf->Write();
   delete outf; outf=0;
   
}
