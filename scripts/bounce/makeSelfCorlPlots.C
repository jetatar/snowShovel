#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TLine.h>
#include <TGraph.h>
#include <TObjArray.h>
#include <TSpline.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TFile.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TTree.h>

#include "TSnPlotBrowser.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnCalWvData.h"
#include "NSnConstants.h"
#include "TSnRecoChanOffsets.h"
#include "TSnInterp1DSpline3.h"
#include "TSnChanTimeCorlMod.h"
#include "TSnRecoSingleChanOffset.h"
#include "TSnRecoSingleChanOffsetSet.h"
#include "TSnChanTimeCorlAltEvtsMod.h"
#endif

//#define DEBUG

TChain* datt, * offt;
TCanvas* c1, * c2, * c3;
TFile* outf;
TH2F* hShifted[NSnConstants::kNchans];
TH2F* hSubtracted[NSnConstants::kNchans][2];
TProfile* hSubtractedPrf[NSnConstants::kNchans][2];
TProfile* hShiftedPrf[NSnConstants::kNchans];
TProfile* hShiftedPrfSep[NSnConstants::kNchans][2];
TH1F* hAmps[NSnConstants::kNchans];
TH1F* hChShift[NSnConstants::kNchans][NSnConstants::kNchans];

const Int_t chcols[NSnConstants::kNchans] = 
   { kOrange+7, kAzure-2, kViolet-1, kGreen+2 };
const Int_t chmrks[NSnConstants::kNchans] = {7,7,7,7};
const Int_t dtcol = kRed;
const Int_t dtwid = 2;
const Int_t dtsty = 7;
const Int_t shcol = kBlack;

const Int_t   datbins = 100;
const Float_t datmin = -350, datmax = 350;

const Int_t   ampbins = 70;
const Float_t ampmin = 0, ampmax = 350;

const Float_t crlmin = -1.1, crlmax = 1.1;

const Int_t   cdtbins = 100;
const Float_t cdtmin = -5.0, cdtmax = 5.0;

const Int_t tbins = NSnConstants::kNsamps;
const Float_t tmin = -0.5 / NSnConstants::kSampRate;
const Float_t tmax = NSnConstants::kNsamps / NSnConstants::kSampRate;

const Int_t dtbins = 1 + (2*tbins);
const Float_t dtmin = -tmax - tmin;
const Float_t dtmax = tmax + tmin;

const Int_t cwid=1200, chit=900;

// data tree branches
TSnEventMetadata*    dmd(0);
TSnEventHeader*      dhd(0);
TSnCalWvData*        dat(0);
TSnInterp1DSpline3** dfl(0);
TSnCalWvData*        adt(0);
// offset tree branches
TSnEventMetadata*           fmd(0);
TSnEventHeader*             fhd(0);
TSnRecoChanOffsets*         fco(0);
TSnRecoSingleChanOffsetSet* fso(0);

void setupDataTree(const Char_t* infn,
                   const Char_t* treenm,
                   const Char_t* datbr="FilteredData.",
                   const Char_t* alttag=NULL) {
   datt = new TChain(treenm);
   datt->Add(infn);
   
   dmd = new TSnEventMetadata;
   dhd = new TSnEventHeader;
   dat = new TSnCalWvData;
   
   datt->SetBranchAddress("EventMetadata.", &dmd);
   datt->SetBranchAddress("EventHeader.", &dhd);
   datt->SetBranchAddress(datbr, &dat);

   if (alttag!=NULL) {
      adt = new TSnCalWvData;
      datt->SetBranchAddress(Form("%s%s",alttag,datbr), &adt);
   }
   
   TString bn;
   dfl = new TSnInterp1DSpline3*[TSnRecoChanOffsets::kChCmbs];
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      if (alttag==NULL) {
         for (UChar_t xc=0; xc<ch; ++xc) {
            bn = TSnChanTimeCorlMod::GetChanCorlInterpName(ch, xc);
            const Int_t di = TSnRecoChanOffsets::IndexFor(ch, xc);
            dfl[di] = new TSnInterp1DSpline3;
            datt->SetBranchAddress(bn.Data(), &(dfl[di]));
         }
      } else {
         bn = TSnChanTimeCorlAltEvtsMod::GetChanCorlInterpName(ch);
         dfl[ch] = new TSnInterp1DSpline3;
         datt->SetBranchAddress(bn.Data(), &(dfl[ch]));
      }
   }
   
}

void setupOffsetTree(const Char_t* infn,
                     const Char_t* treenm,
                     const Char_t* alttag=NULL) {

   offt = new TChain(treenm);
   offt->Add(infn);
   
   fmd = new TSnEventMetadata;
   fhd = new TSnEventHeader;

   offt->SetBranchAddress("EventMetadata.", &fmd);
   offt->SetBranchAddress("EventHeader.", &fhd);
   if (alttag==NULL) {
      fco = new TSnRecoChanOffsets;
      offt->SetBranchAddress("ChanTimeOffsets.", &fco);
   } else {
      fso = new TSnRecoSingleChanOffsetSet;
      offt->SetBranchAddress("SingleChanTimeOffsets.", &fso);
   }

   offt->BuildIndex("EventMetadata.fRun", "EventHeader.fNum");
   
}

void makeSelfCorlPlots(const Char_t* infn,
                       const Char_t* outfn,
                       const Char_t* datbr="AmpOutDataShifted.",
                       const Char_t* alttag="AltRun",
                       const Long64_t refEvt=10,
                       const Char_t* dattnm="CorlFilt",
                       const Char_t* offtnm="FitOffsets") {
   
   gStyle->SetOptStat(0);
   
   setupDataTree(infn, dattnm, datbr, alttag);
   setupOffsetTree(infn, offtnm, alttag);

   
   const Long64_t n = datt->GetEntries();

   if (n<refEvt) {
      Printf("Ref event [%lld] invalid for tree having [%lld] entries.",
             refEvt, n);
      return;
   }
   datt->GetEntry(refEvt);
   const Long64_t roi = offt->GetEntryNumberWithIndex(dmd->GetRunNum(),
                                                      dhd->GetEvtNum());
   if (roi<0) {
      Printf("Could not get fit offset for ref evt run [%u], evt [%u]",
             dmd->GetRunNum(), dhd->GetEvtNum());
      return;
   }

   outf = TFile::Open(outfn, "recreate");
   TDirectory* wvs   = outf->mkdir("AlignedWvfms");
   TDirectory* wvsJp = outf->mkdir("AlignedWvCh0Jump");
   TDirectory* sbsOk = outf->mkdir("SubdWvfmsCh0ok");
   TDirectory* sbsJp = outf->mkdir("SubdWvfmsCh0Jump");
   outf->cd();

   TString hn;
   TTree* ot = new TTree("RelChanShifts","relative channel shifts");
   ot->Branch("EventMetadata.", &fmd);
   ot->Branch("EventHeader.", &fhd);
   Double_t chOffsets[NSnConstants::kNchans][NSnConstants::kNchans];
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (UChar_t xc=0; xc<ch; ++xc) {
         chOffsets[ch][xc] = 0;
         hn = Form("shiftCh%hhuCh%hhu",ch,xc);
         ot->Branch(hn.Data(),&(chOffsets[ch][xc]),Form("%s/D",hn.Data()));
      }
   }
   ot->SetDirectory(outf);
   
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      hn = Form("hShifted_ch%hhu",ch);
      hShifted[ch] = new TH2F(hn.Data(),
                              Form("aligned waveforms;"
                                   "time (ns);amplitude (mV)"),
                              tbins, tmin, tmax,
                              datbins, datmin, datmax);
      hShifted[ch]->SetDirectory(outf);
      hn = Form("hShiftedPrf_ch%hhu",ch);
      hShiftedPrf[ch] = new TProfile(hn.Data(),
                                     Form("aligned waveforms;"
                                          "time (ns); amplitude (mV)"),
                                     tbins, tmin, tmax);
      hShiftedPrf[ch]->SetMinimum(datmin);
      hShiftedPrf[ch]->SetMaximum(datmax);
      hShiftedPrf[ch]->SetDirectory(outf);

      for (Int_t j=0; j<2; ++j) {

         hn = Form("hShiftedPrfSep_ch%hhu_j%d",ch,j);
         hShiftedPrfSep[ch][j] = new TProfile(hn.Data(),
                                        Form("aligned waveforms (%s);"
                                             "time (ns); amplitude (mV)",
                                            (j==0) ? "ch0 ok" : "ch0 jump"),
                                        tbins, tmin, tmax);
         hShiftedPrfSep[ch][j]->SetMinimum(datmin);
         hShiftedPrfSep[ch][j]->SetMaximum(datmax);
         hShiftedPrfSep[ch][j]->SetDirectory(outf);

         hn = Form("hSubtracted_ch%hhu_j%d",ch,j);
         hSubtracted[ch][j] = new TH2F(hn.Data(),
                                       Form("waveforms - event %lld (%s);"
                                            "time (ns);#Delta (mV)",
                                            refEvt,
                                            (j==0) ? "ch0 ok" : "ch0 jump"),
                                       tbins, tmin, tmax,
                                       datbins, datmin, datmax);
         hSubtracted[ch][j]->SetDirectory(outf);
         hn = Form("hSubtractedPrf_ch%hhu_j%d",ch,j);
         hSubtractedPrf[ch][j] = new TProfile(hn.Data(),
                                        Form("waveforms - event %lld (%s);"
                                             "time (ns); #Delta (mV)",
                                             refEvt,
                                             (j==0) ? "ch0 ok" : "ch0 jump"),
                                        tbins, tmin, tmax);
         hSubtractedPrf[ch][j]->SetMinimum(datmin);
         hSubtractedPrf[ch][j]->SetMaximum(datmax);
         hSubtractedPrf[ch][j]->SetDirectory(outf);
      }
      
      hn = Form("hAmps_ch%hhu",ch);
      hAmps[ch] = new TH1F(hn.Data(),
                           Form("max amplitude of waveform;"
                                "max amplitude (mV);events"),
                           ampbins, ampmin, ampmax);
      hAmps[ch]->SetDirectory(outf);

      for (UChar_t xc=0; xc<ch; ++xc) {
         hn = Form("hChShift_ch%hhu_xc%hhu",ch,xc);
         hChShift[ch][xc] = new TH1F(hn.Data(),
                                     Form("shift of ch%hhu - shift of ch%hhu"
                                          ";ch%hhu - ch%hhu;events",
                                          ch,xc,ch,xc),
                                     dtbins, dtmin, dtmax);
         hChShift[ch][xc]->SetDirectory(outf);
      }
   }

   datt->GetEntry(refEvt);
   offt->GetEntry(roi);
   TSnCalWvData* refDat = new TSnCalWvData(*dat);
   TSnRecoSingleChanOffsetSet* refOffs = new TSnRecoSingleChanOffsetSet(*fso);

   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      TGraph* refWv = refDat->NewGraphForChan(ch, kTRUE, kTRUE,
                                              -(refOffs->GetTimeOffset(ch)),
                                              kTRUE);
      refWv->SetName(Form("gRefWv_e%lld_ch%hhu",refEvt,ch));
      outf->WriteTObject(refWv);
      delete refWv;
   }
   
#ifdef DEBUG
   c2 = new TCanvas("c2","c2 - graphs together",cwid,chit);
   c2->Divide(2,2);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      c2->cd(ch+1);
      TH1* h = new TH1F(Form("h_ch%hhu",ch),
                        "",
                        tbins, tmin, tmax);
      h->SetMinimum(datmin);
      h->SetMaximum(datmax);
      h->Draw("axis");
   }

   for (Long64_t i=0; i<10; ++i) {
#else   
   for (Long64_t i=0; i<n; ++i) {
#endif
      datt->GetEntry(i);
      const Long64_t oi = offt->GetEntryNumberWithIndex(dmd->GetRunNum(),
                                                        dhd->GetEvtNum());
      if (oi>-1) {
         offt->GetEntry(oi);
         
         const Float_t ch0jmp = fso->GetTimeOffset(0) - fso->GetTimeOffset(1);
         if (TMath::Abs(ch0jmp)>1.5) {
            // correlation failed
            //continue;
         }
         const Bool_t ch0ok =  TMath::Abs(ch0jmp) < 0.5;
         const Int_t k = (ch0ok) ? 0 : 1;
         
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            const Float_t shft = -(fso->GetTimeOffset(ch));
            TGraph* shifted = dat->NewGraphForChan(ch, kTRUE, kTRUE,
                                                   shft,
                                                   kTRUE);
            shifted->SetName(Form("gAlignedWvfm_e%lld_ch%hhu",i,ch));
            wvs->WriteTObject(shifted);
            if (ch0ok==kFALSE) {
               wvsJp->WriteTObject(shifted);
            }
            TGraph* subd = dat->NewGraphForChan(ch, kTRUE);
            TGraph* refg = refDat->NewGraphForChan(ch, kTRUE, kTRUE,
                                                   -(refOffs->GetTimeOffset(ch))
                                                   +(fso->GetTimeOffset(ch)),
                                                   kTRUE);
            /*
            TCanvas* c = new TCanvas("c","c",800,1100);
            c->Divide(1,3);
            c->cd(1);
            subd->Draw("apl");
            TGraph* gg = refDat->NewGraphForChan(ch, kTRUE);
            gg->SetLineColor(kGreen+2);
            gg->Draw("pl");
            Printf("ref dt = %g, dat dt = %g",
                   refOffs->GetTimeOffset(ch), fso->GetTimeOffset(ch));
            c->cd(2);
            TGraph* sr = dat->NewGraphForChan(ch, kTRUE, kTRUE,
                                              -(fso->GetTimeOffset(ch)), kTRUE);
            TGraph* rr = refDat->NewGraphForChan(ch, kTRUE, kTRUE,
                                                 -(refOffs->GetTimeOffset(ch)),
                                                 kTRUE);
            sr->Draw("apl");
            rr->SetLineColor(kGreen+2);
            rr->Draw("pl");
            c->cd(3);
            refg->SetLineColor(kGreen+2);
            subd->Draw("apl");
            refg->Draw("pl");
            Printf("subd (%g,%g), refg (%g,%g)",
                   subd->GetX()[0], subd->GetY()[0],
                   refg->GetX()[0], refg->GetY()[0]);
            c->cd();
            c->Update();
            TObject* o(0);
            while ( (o = c->WaitPrimitive())!=0 ) {
               gSystem->ProcessEvents();
            }
            delete c; delete gg; delete sr; delete rr;
            */
            subd->SetName(Form("gSubtracted_e%lld_ch%hhu",i,ch));
            const Int_t np = shifted->GetN();
            const Double_t* x = shifted->GetX();
            const Double_t* y = shifted->GetY();
            const Double_t* r = refg->GetY();
            Double_t* s = subd->GetY() ;
            Double_t maxy(*y);
            for (Int_t j=0; j<np; ++j, ++x, ++y, ++r, ++s) {
               hShifted[ch]->Fill(*x, *y);
               hShiftedPrf[ch]->Fill(*x, *y);
               hShiftedPrfSep[ch][k]->Fill(*x, *y);
               *s -= *r;
               hSubtracted[ch][k]->Fill(*x, *s);
               hSubtractedPrf[ch][k]->Fill(*x, *s);
               if (*y > maxy) {
                  maxy = *y;
               }
            }
            hAmps[ch]->Fill(maxy);
            
            if (ch0ok) {
               sbsOk->WriteTObject(subd);
            } else {
               sbsJp->WriteTObject(subd);
            }

            for (UChar_t xc=0; xc<ch; ++xc) {
               if (fco!=0) {
                  chOffsets[ch][xc] = fco->GetChanOffset(ch, xc);
               } else if (fso!=0) {
                  chOffsets[ch][xc] = fso->GetTimeOffset(ch) -
                                      fso->GetTimeOffset(xc);
               }
               hChShift[ch][xc]->Fill( chOffsets[ch][xc] );
            }

            delete shifted;
            delete subd;
            delete refg;

#ifdef DEBUG            
            c2->cd(ch+1);
            shifted->SetLineColor(i);
            shifted->Draw("pl");
#endif
         }
         ot->Fill();
      }
   }
   
   outf->Write();

#ifdef DEBUG
   c2->cd();
   c2->Update();
#endif
   
   c1 = new TCanvas("c1","c1 - 2d aligned",cwid,chit);
   c1->Divide(2,2);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      c1->cd(ch+1);
      hShifted[ch]->Draw("colz");
   }
   c1->cd();
   c1->Update();
   
   c3 = new TCanvas("c3","c3 - 2d aligned profile",cwid,chit);
   c3->Divide(2,2);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      c3->cd(ch+1);
      hShiftedPrf[ch]->Draw("hist");
   }
   c3->cd();
   c3->Update();
   
   
}
