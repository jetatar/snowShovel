#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TTree.h>
//#include <TDatime.h>
#include <TString.h>
#include <TGraphErrors.h>
//#include <TGraph.h>
//#include <TMath.h>
#include <TH1.h>
//#include <TH2.h>
#include <TCanvas.h>
#include <TSystem.h>
//#include <TStyle.h>
#include <TFile.h>

#include <NSnConstants.h>
#endif

//#define DEBUG

// 2012 station
static const Int_t   kNchans    = NSnConstants::kNchans;
static const Int_t   kNsamps    = NSnConstants::kNsamps;
static const Int_t   kNextra    = 57;

const Int_t npsbins = 1200,
            nadcbins = 1200;
const Float_t psmin = -600, psmax=600,
              adcmin = 1000, adcmax = 2200;

TChain* nt;
TTree* ot;
TFile* outf, * fpnf;

TGraphErrors* gPed[kNchans], * gNoise[kNchans];

TH1* hMaxSamp[kNchans], * hMaxAdc[kNchans], * hAdc[kNchans],
   * hMinSamp[kNchans], * hMinAdc[kNchans];

void chkTrgChans(const Char_t* infn,
                 const Char_t* outfn,
                 const Char_t* pedfn=0) {
   
   nt = new TChain("runtree");
   nt->Add(infn);
   
   if (pedfn!=0) {
      fpnf = TFile::Open(pedfn);
      if ((fpnf!=0) && (fpnf->IsZombie()==kFALSE)) {
         for (Int_t ch=0; ch<kNchans; ch++) {
            gPed[ch]   = dynamic_cast<TGraphErrors*>(
               fpnf->Get(Form("gExlPed_ch%d",ch)));
            gNoise[ch] = dynamic_cast<TGraphErrors*>(
               fpnf->Get(Form("gExlRms_ch%d",ch)));
            if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
               Printf("Couldn't get FPN/noise graphs from [%s]",pedfn);
            } else {
               Printf("Using pre-calculated FPN for chan %d",ch);
            }
         }
      } else {
         Printf("Couldn't open FPN file [%s]",pedfn);
      }
   }

   // read from tree into...
   UShort_t samples[kNchans][kNsamps];
   Float_t  pedsub[kNchans][kNsamps];
   UShort_t extra[kNextra];
   UInt_t   evnum, utime, utimeus, mbchksum;
   
   memset(extra,0,kNextra*sizeof(UShort_t));
   
   // setup tree, make hists
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("extra",&(extra[0]));
   TString hn;
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
   }
   
   const Long64_t nents = nt->GetEntries();
   outf = TFile::Open(outfn,"recreate");
   Int_t xadc[kNchans], nadc[kNchans];
   ot = new TTree("nt","max sample tree");
   ot->Branch("x0",xadc,"x0/I");
   ot->Branch("x1",xadc+1,"x1/I");
   ot->Branch("x2",xadc+2,"x2/I");
   ot->Branch("x3",xadc+3,"x3/I");
   ot->Branch("n0",nadc,"n0/I");
   ot->Branch("n1",nadc+1,"n1/I");
   ot->Branch("n2",nadc+2,"n2/I");
   ot->Branch("n3",nadc+3,"n3/I");
   ot->Branch("EvtNum",&evnum,"EvtNum/i");
   ot->Branch("unixTime",&utime,"unixTime/i");
   ot->Branch("unixTimeUS",&utimeus,"unixTimeUS/i");
   ot->Branch("extra10",extra+10,"extra10/s");
   ot->Branch("mbChecksum",&mbchksum,"mbChecksum/i");\
   for (Int_t ch=0; ch<kNchans; ch++) {
      hn = Form("ps%02hu",ch);
      ot->Branch(hn.Data(),&(pedsub[ch][0]),
		 Form("%s[%hu]/F",hn.Data(),NSnConstants::kNsamps));

      hn = Form("hMaxSamp_ch%d",ch);
      hMaxSamp[ch] = new TH1F(hn.Data(),
                              Form("max sample of chan %d;sample;N events",ch),
                              kNsamps, -0.5, kNsamps-0.5);
      hMaxSamp[ch]->SetDirectory(outf);
      const Int_t bins = (pedfn!=0) ? npsbins : nadcbins;
      const Float_t min = (pedfn!=0) ? psmin : adcmin,
                    max = (pedfn!=0) ? psmax : adcmax;
      hn = Form("hMaxAdc_ch%d",ch);
      hMaxAdc[ch] = new TH1F(hn.Data(),
                             Form("max adc of chan %d;%s;N events",
                                  ch, (pedfn!=0)?"adc-ped":"adc"),
                             bins, min, max);
      hMaxAdc[ch]->SetDirectory(outf);
      hn = Form("hMinSamp_ch%d",ch);
      hMinSamp[ch] = new TH1F(hn.Data(),
                              Form("min sample of chan %d;sample;N events",ch),
                              kNsamps, -0.5, kNsamps-0.5);
      hMinSamp[ch]->SetDirectory(outf);
      hn = Form("hMinAdc_ch%d",ch);
      hMinAdc[ch] = new TH1F(hn.Data(),
                             Form("min adc of chan %d;%s;N events",
                                  ch, (pedfn!=0)?"adc-ped":"adc"),
                             bins, min, max);
      hMinAdc[ch]->SetDirectory(outf);
      hn = Form("hAdc_ch%d",ch);
      hAdc[ch] = new TH1F(hn.Data(),
                          Form("adc of chan %d;%s;N events",
                               ch, (pedfn!=0)?"adc-ped":"adc"),
                          bins, min, max);
      hAdc[ch]->SetDirectory(outf);
   }
   ot->SetDirectory(outf);
   
   for (Long64_t i=0; i<nents; ++i) {
      if ( (i%500)==0 ) {
         fprintf(stderr, "Processing %lld/%lld   (%2.2f%%) ...          \r",
                 i,nents,
                 100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(i);
      Int_t* x = xadc, * y = nadc;
      for (Int_t ch=0; ch<kNchans; ++ch, ++x, ++y) {
         *x = -1;
         *y = -1;
         Float_t madc(-5e8), minadc(5e8);
         const UShort_t* s = samples[ch];
         Float_t* p = pedsub[ch];
         const Double_t* f = 0;
         if (pedfn!=0) {
            f = gPed[ch]->GetY();
         }
#ifdef DEBUG
         Bool_t verb=kFALSE;
         for (Int_t sm=0; sm<kNsamps; ++sm) {
            if (samples[ch][sm]>2000) {
               verb=kTRUE;
            }
         }
#endif
         for (Int_t sm=0, ss=0; sm<kNsamps; ++sm, ++s, ++p, ++f) {
            if ( (*s)==0 || (*s)==4095 ) {
               continue;
            } else {
               ++ss;
            }
            *p = *s;
#ifdef DEBUG
            if (verb) {
               printf("ch=%d, sm=%d, p=%g, s=%u, f=%g, madc=%g",
                      ch, sm, *p, *s, (pedfn!=0)?*f:0, madc);
            }
#endif
            if (pedfn!=0) {
               *p -= *f;
            }
            if ( (ss==1) || ((*p)>madc) ) {
               *x = sm;
               madc = *p;
            }
            if ( (ss==1) || ((*p)<minadc) ) {
               *y = sm;
               minadc = *p;
            }
#ifdef DEBUG
            if (verb) {
               printf(", x=%d, madc(a)=%g", *x, madc);
               if (*s>2000) {
                  printf("  <<<<");
               }
               printf("\n");
            }
#endif
            hAdc[ch]->Fill(*p);
         }
         hMaxAdc[ch]->Fill(madc);
         hMaxSamp[ch]->Fill(*x);
         hMinAdc[ch]->Fill(minadc);
         hMinSamp[ch]->Fill(*y);
      }
      ot->Fill();
   }
   fprintf(stderr,"\n");
   
   outf->Write();
   
}
