#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TTimeStamp.h>
#endif

static const UShort_t kNumAdcs     = 16;
static const Float_t  kRcInstrBox  = 0.05;   // Ohms
static const Float_t  kVoltPerAdc  = 10.0 / TMath::Power(2.,16.); // V/chan
static const Float_t  kKperVolt    = 100.; // Kelvin / volt
static const Float_t  kKnotPerVolt = 1000./16.67; // knots / volt

static const Int_t    kNumPeriods = 2;
static const Float_t  kRcBatBox[kNumPeriods]  = { 0.0355, 0.0471 };
static const Float_t  kCurOffset[kNumPeriods] = { -2.90,  1.17   };

enum EADCDef_t {
   kNone0=0,
   kWindP=1,
   kWindN=2,
   kSolP=3,
   kBatBoxP=4,
   kInstrBoxP=5,
   kTemp2=6,
   kTemp1=7,
   kNone8=8,
   kBatVolt=9,
   kSolN=10,
   kBatBoxN=11,
   kInstrBoxN=12,
   kTemp2Gr=13,
   kTemp1Gr=14,
   kAnem=15
};

TChain* nt;
TFile * outf;
TGraph* gWind, * gSol, * gBatBox, * gInstrBox,
      * gBatVolt, * gTemp1, * gTemp2, * gAnem;

void makeGraph(TGraph*& g, const Char_t* name) {
   g = new TGraph;
   g->SetName(name);
}

void writeGraph(TGraph* g) {
   g->Write(g->GetName());
}

Float_t calCur(const UShort_t ap, const UShort_t an,
               const Float_t rc, const Float_t co) {
   if (rc>0.0) {
      const Float_t ad = ap - an;
      return ( ((2.0 / rc) * kVoltPerAdc * ad) + co );
   }
   return 0;
}

Float_t calTemp(const UShort_t adc) {
   static const Float_t kPerAdc = kKperVolt * kVoltPerAdc;
   return ((adc * kPerAdc) - 273.);
}

Float_t calAnem(const UShort_t adc) {
   static const Float_t knotsPerAdc = kKnotPerVolt * kVoltPerAdc;
   return (adc * knotsPerAdc);
}

Float_t calVolt(const UShort_t adc) {
   return (2.0 * adc * kVoltPerAdc);
}

void hkPlot(const Char_t* infn,
            const Char_t* outfn) {
   
   
   nt = new TChain("hk");
   nt->Add(infn);
   
   const Long64_t nents = nt->GetEntries();
   if (nents<1) {
      Error("hkPlot","No entries in chain.");
      return;
   }
   Printf("Entries in tree = %lld",nents);
   
   // open the output file (to store histograms)
   outf = TFile::Open(outfn,"recreate");
   
   // read from tree into...
   UShort_t adc[kNumAdcs];
   UInt_t   utime;
   
   // setup tree, make hists
   nt->SetBranchAddress("unixTime",&utime);
   for (UShort_t a=0; a<kNumAdcs; a++) {
      nt->SetBranchAddress(Form("adc%02hu",a), &(adc[a]));
   }
   makeGraph(gWind,     "gWind");
   makeGraph(gSol,      "gSol");
   makeGraph(gBatBox,   "gBatBox");
   makeGraph(gInstrBox, "gInstrBox");
   makeGraph(gBatVolt,  "gBatVolt");
   makeGraph(gTemp1,    "gTemp1");
   makeGraph(gTemp2,    "gTemp2");
   makeGraph(gAnem,     "gAnem");
   
   
   // process events
   Int_t prd=0;
   Long64_t pts[kNumAdcs] = {0};
   Printf("Processing tree...");
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            e, nents,
            100.*static_cast<Float_t>(e)/static_cast<Float_t>(nents));
      }
      
      prd = (utime<1327637100) ? 0 : 1;
      
      if ( (adc[kWindP]!=0) && (adc[kWindN]!=0) ) {
         gWind->SetPoint(pts[kWindP]++, utime,
            calCur(adc[kWindP], adc[kWindN], kRcBatBox[prd], kCurOffset[prd]));
      }
      
      if ( (adc[kSolP]!=0) && (adc[kSolN]!=0) ) {
         gSol->SetPoint(pts[kSolP]++, utime,
            calCur(adc[kSolP], adc[kSolN],
                   kRcBatBox[prd], kCurOffset[prd]));
      }
      if ( (adc[kBatBoxP]!=0) && (adc[kBatBoxN]!=0) ) {
         gBatBox->SetPoint(pts[kBatBoxP]++, utime,
            calCur(adc[kBatBoxP], adc[kBatBoxN],
                   kRcBatBox[prd], kCurOffset[prd]));
      }
      
      if ( (adc[kInstrBoxP]!=0) && (adc[kInstrBoxN]!=0) ) {
         gInstrBox->SetPoint(pts[kInstrBoxP]++, utime,
            calCur(adc[kInstrBoxP], adc[kInstrBoxN], kRcInstrBox, 0));
      }
      /*
      if (adc[kBatVolt]!=0) {
         gBatVolt->SetPoint(pts[kBatVolt]++, utime,
            calVolt(adc[kBatVolt]));
      }
      */
      if (adc[kWindP]!=0) {
         gBatVolt->SetPoint(pts[kBatVolt]++, utime,
            calVolt(adc[kWindP]));
      }
      
      if (adc[kTemp1]!=0) {
         gTemp1->SetPoint(pts[kTemp1]++, utime, calTemp(adc[kTemp1]));
      }
      if (adc[kTemp2]!=0) {
         gTemp2->SetPoint(pts[kTemp2]++, utime, calTemp(adc[kTemp2]));
      }
      
      if (adc[kAnem]!=0) {
         gAnem->SetPoint(pts[kAnem]++, utime, calAnem(adc[kAnem]));
      }
   }
   
   outf->cd();
   writeGraph(gWind);
   writeGraph(gSol);
   writeGraph(gBatBox);
   writeGraph(gInstrBox);
   writeGraph(gBatVolt);
   writeGraph(gTemp1);
   writeGraph(gTemp2);
   writeGraph(gAnem);
   
}
