#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TDatime.h>
#include <TString.h>
#include <TGraph.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TMath.h>
#include <TFile.h>
#include <TEventList.h>

#include <algorithm>

#endif

#include <set>

static const Int_t kNextra=57;
static const Int_t kTrgLoc=10;

TChain* nt;
TFile* outf;
TEventList* el;

Double_t* t_p;
UInt_t* en_p;
Long64_t* sidx_p;
Long64_t* odix_p;


void pickHiRate(const Char_t*  infn,
                const Char_t*  outfn,
                const Double_t maxdt,
                const Int_t    trg=-1,
                const UInt_t   enmin=0,
                const UInt_t   enmax=kMaxUInt,
                const Bool_t   verifyChksum=kTRUE,
                const Double_t mindate=1324339200, // Dec 20, 2011 00:00:00
                const Double_t maxdate=1349049600) { // Oct 1, 2011
   // make a TEventList of consecutive event pairs having a deltaT<maxdt
   //
   // use copyChain.C to make a new tree from the TEventList
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   
   Printf("Chaining...");
   
   nt = new TChain("nt");
   nt->Add(infn);
   
   UShort_t extra[kNextra];
   UInt_t utime, utimeus, evtnum, mbchksum;
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("EvtNum",&evtnum);
   nt->SetBranchAddress("extra",&(extra[0]));
   
   nt->GetEntry(0);
   
   const Long64_t nents = nt->GetEntries();
   
   Double_t* const t = new Double_t[nents];
                 t_p = t;
   Double_t*       u = t,
           *       v = t;
   
   UInt_t* const en = new UInt_t[nents];
               en_p = en;
   UInt_t*       ni = en,
         *       nj = en;
   
   outf = TFile::Open(outfn,"recreate");
   if (outf==0 || outf->IsZombie()) {
      Printf("Could not open output file [%s].",outfn);
      return;
   }
   el = new TEventList("pickrate",
      Form("List of event numbers with consec. dt < %g",maxdt));
   el->SetDirectory(outf);
   
   Printf("Looping over %lld entries", nents);
   
   Long64_t selevts = 0;
   for (Long64_t i=0; i<nents; i++) {
      nt->GetEntry(i);
      
      if ( (i%5000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            i, nents,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok = ( (trg==-1) || (extra[kTrgLoc]==trg) );
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
      }
      if (ok) {
         ok &= (evtnum<=enmax) && (evtnum>=enmin);
      }
      if (ok) {
         ok &= (utime>mindate) && (utime<maxdate);
      }
      if ( ok ) {

         *u = static_cast<Double_t>(utime)
               + (static_cast<Double_t>(utimeus)*1.0e-6);
         *ni = evtnum;

         selevts++;
         u++;
         ni++;
      }
   }
   
   if (selevts==0) {
      Printf("No events selected.");
      return;
   }
   
   // sidx = indicies of tims that correspond to the sorted ordering of tims
   Long64_t* const sidx = new Long64_t[selevts];
                 sidx_p = sidx;
   Long64_t*          x = sidx,
           * const   ex = sidx+selevts;
   
   std::set<Long64_t> oevts;
   
   Printf("Sorting by time...");
   
   TMath::Sort(selevts, t, x, kFALSE);
   
   Printf("Calculating time differences...");
   
   Double_t delt;
   ni = nj = en;
   for (x = sidx; x!=ex; x++) {
      u  = t  + (*x);
      ni = en + (*x);
      if (x!=sidx) {
         v  = t  + (*(x-1));
         nj = en + (*(x-1));
         delt = *u - *v;
         if (delt < maxdt) {
            oevts.insert(*ni);
            oevts.insert(*nj);
         }
      }
   }
   
   std::set<Long64_t>::const_iterator ox, oend=oevts.end();
   for (ox=oevts.begin(); ox!=oend; ox++) {
      el->Enter(*ox);
   }
   
   outf->Write();
   
}
