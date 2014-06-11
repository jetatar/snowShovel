#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TGraphErrors.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TLatex.h>

#include <vector>

#endif

//#define DEBUG

// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgBitIdx = 10;
static const Float_t kSmpRate   = 1.92; // GHz

static const Int_t   kHlfSmps   = kNsamps/2;

// read from tree into...
UShort_t samples[kNchans][kNsamps];
UShort_t extra[kNextra];
UInt_t   evnum;
UInt_t   utime;
UInt_t   utimeus;
UInt_t   mbchksum;
   


enum EChBits_t {
   kCh0 = BIT(0),
   kCh1 = BIT(1),
   kCh2 = BIT(2),
   kCh3 = BIT(3)
};

const Int_t  col[4] = {kOrange+7, kAzure-2, kViolet-1, kGreen+2};

const Int_t nchi2bins=200;
const Float_t chi2min=5.5, chi2max=9.5;

TGraph* gMinShiftVsEvt[kNchans][kNchans], * gMinChi2VsEvt[kNchans][kNchans];

TH2* hShiftChi2VsEvt[kNchans][kNchans],
   * hShiftVsChi2[kNchans][kNchans],
   * hShiftVsShift[kNchans][kNchans][kNchans][kNchans],
   * hChi2VsChi2[kNchans][kNchans][kNchans][kNchans];

TH1* hMinShift[kNchans][kNchans],
   * dChi2Space, * dWvfm[kNchans];

TChain* nt=0;
TFile* outf, * pedf;
TCanvas* c1, * c2, * c3, * c4, * c5, * c6,
       * cshft=0, * cchi2=0;

void setStyle(TH1* h, const Int_t typ) {
   if (h!=0) {
      h->SetLineColor(col[typ]);
   }
}

Bool_t isActive(const Int_t ch, const Int_t activeChs) {
   return ( BIT(ch) & activeChs )!=0;
}

void drawChi2(const Int_t ch, const Int_t nc, const Int_t evt) {
   if (nc<ch) {
      if (cchi2==0) {
         cchi2 = new TCanvas("cchi2","cchi2",500,500);
      } else {
         cchi2->Clear();
      }
      cchi2->cd();
      TString hn = Form("dChi2Space_ch%d_nc%d",ch,nc);
      const Int_t bin = hShiftChi2VsEvt[ch][nc]->GetXaxis()->FindBin(evt);
      if ( (bin>1) && (bin<=hShiftChi2VsEvt[ch][nc]->GetNbinsX()) ) {
         dChi2Space = hShiftChi2VsEvt[ch][nc]->ProjectionY(hn.Data(),
            bin, bin);
         dChi2Space->SetTitle(Form("#chi^{2} space (ch%d | ch%d, event %d)",
            ch,nc,evt));
         dChi2Space->SetBit(TH1::kCanDelete);
         dChi2Space->SetDirectory(0);
         dChi2Space->Draw();
      } else {
         Printf("couldn't find event %d",evt);
      }
      cchi2->cd();
      cchi2->Update();
   } else {
      Printf("nc>=ch (%d>=%d). reverse channel order?",nc,ch);
   }
}

void mkShftHist(const Int_t ch, const Int_t nc,
                const Int_t evt, const Int_t shift) {
   TString hn = Form("dWvfm_ch%d_evt%d",ch,evt);
   dWvfm[ch] = new TH1F(hn.Data(),
      Form("waveforms with ch%d shifted by %d (evt %d)"
         ";sample;ADC",
         nc, shift, evt),
      kNsamps, -0.5, kNsamps-0.5);
   dWvfm[ch]->SetDirectory(0);
   dWvfm[ch]->SetBit(TH1::kCanDelete);
   setStyle(dWvfm[ch],ch);
}

void drawShift(const Int_t ch, const Int_t nc,
               const Int_t shift, const Int_t evt) {
   if (nt!=0) {
      if (nt->GetEntry(evt)>0) {
         if (cshft==0) {
            cshft = new TCanvas("cshft","cshft",500,500);
         } else {
            cshft->Clear();
         }
         cshft->cd();
         TString hn;
         UShort_t minadc=0, maxadc=0;
         mkShftHist(ch, nc, evt, shift);
         mkShftHist(nc, nc, evt, shift);
         Bool_t fd=kTRUE;
         for (Int_t cc=0; cc<kNsamps; cc++) {
            if ( (cc==ch) || (cc==nc) ) {
               Bool_t first=kTRUE;
               for (Int_t sm=0; sm<kNsamps; sm++) {
                  Int_t ss = (cc==nc) ? sm-shift : sm;
                  while (ss<0) {
                     ss += kNsamps;
                  }
                  while (ss>=kNsamps) {
                     ss -= kNsamps;
                  }
                  const UShort_t adc = samples[cc][ss];
                  dWvfm[cc]->Fill(sm, adc);
                  if ( first ) {
                     minadc = maxadc = adc;
                     first=kFALSE;
                  } else {
                     if (adc<minadc) {
                        minadc = adc;
                     }
                     if (adc>maxadc) {
                        maxadc = adc;
                     }
                  }
               }
               dWvfm[cc]->Draw( (fd) ? "" : "same" );
               fd=kFALSE;
            }
         }
         dWvfm[nc]->SetMinimum(minadc*0.8);
         dWvfm[nc]->SetMaximum(maxadc*1.2);
         dWvfm[ch]->SetMinimum(minadc*0.8);
         dWvfm[ch]->SetMaximum(maxadc*1.2);
         cshft->cd();
         cshft->Update();
      } else {
         Printf("couldn't find entry %d in tree",evt);
      }
   } else {
      Printf("no ntuple");
   }
}

void shiftWvStdy(const Char_t*  infn,
                 const Char_t*  outfn,
                 const Long64_t minEntry=0,
                 const Long64_t maxEntries=600,
                 const Int_t    activeChs=(kCh0 | kCh1),
                 const Int_t    trg=0,
                 const Bool_t   verifyChksum=kTRUE) {
   // trg==-1 => all triggers
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   //gStyle->SetOptStat(0);
   
   // open the tree
   nt = new TChain("nt");
   nt->Add(infn);
   
   const Long64_t nents = TMath::Min(nt->GetEntries(), maxEntries) - minEntry;
   Printf("Will process %lld events",nents);
   
   // open the output file (to store histograms)
   outf = TFile::Open(outfn,"recreate");
   
   // setup tree, make hists
   TString hn;
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("HdEvtNum",&evnum);
   //nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("extra",&(extra[0]));
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
      
      if (isActive(ch, activeChs)) {
         for (Int_t nc=0; nc<ch; nc++) {
            if (isActive(ch, activeChs)) {
               hn = Form("hShiftChi2VsEvt_ch%d_nc%d",ch,nc);
               hShiftChi2VsEvt[ch][nc] = new TH2F(hn.Data(),
                  Form("#chi^{2} for each shift vs event (ch%d | ch%d)"
                       ";event;shift;#chi^{2}",ch,nc),
                  nents, minEntry-0.5, nents+minEntry-0.5,
                  kNsamps, -kHlfSmps-0.5, kHlfSmps-0.5);

               gMinChi2VsEvt[ch][nc] = new TGraph;
               gMinChi2VsEvt[ch][nc]->SetName(Form("gMinChi2VsEvt_ch%d_nc%d",ch,nc));
               gMinChi2VsEvt[ch][nc]->SetTitle(
                  Form("min #chi^{2} vs event (ch%d | ch%d)"
                       ";event;min #chi^{2}",ch,nc));
               gMinChi2VsEvt[ch][nc]->SetMarkerStyle(7);

               gMinShiftVsEvt[ch][nc] = new TGraph;
               gMinShiftVsEvt[ch][nc]->SetName(
                  Form("gMinShiftVsEvt_ch%d_nc%d",ch,nc));
               gMinShiftVsEvt[ch][nc]->SetTitle(
                  Form("best shift vs event (ch%d | ch%d);event;best shift",ch,nc));
               gMinShiftVsEvt[ch][nc]->SetMarkerStyle(7);

               hn = Form("hMinShift_ch%d_nc%d",ch,nc);
               hMinShift[ch][nc] = new TH1F(hn.Data(),
                  Form("best shift (ch%d | ch%d);best shift",ch,nc),
                  kNsamps, -kHlfSmps-0.5, kHlfSmps-0.5);

               hn = Form("hShiftVsChi2_ch%d_nc%d",ch,nc);
               hShiftVsChi2[ch][nc] = new TH2F(hn.Data(),
                  Form("best shift vs best #chi^{2} (ch%d | ch%d)"
                       ";best shift;log_{10}(#chi^{2})",ch,nc),
                  kNsamps, -kHlfSmps-0.5, kHlfSmps-0.5,
                  nchi2bins, chi2min, chi2max);
               
               for (Int_t p=0; p<kNchans; p++) {
                  if (isActive(p, activeChs)) {
                     for (Int_t q=0; q<kNchans; q++) {
                        if (isActive(q, activeChs)) {
                           hn = Form("hShiftVsShift_ch%d_nc%d_p%d_q%d",
                              ch,nc,q,p);
                           hShiftVsShift[ch][nc][p][q] = new TH2F(hn.Data(),
                              Form("shift (ch%d|ch%d) vs shift (ch%d|ch%d)"
                                 ";shift (ch%d|ch%d);shift (ch%d|ch%d)",
                                 ch,nc,p,q, ch,nc, p,q),
                              kNsamps, -kHlfSmps-0.5, kHlfSmps-0.5,
                              kNsamps, -kHlfSmps-0.5, kHlfSmps-0.5);
                           
                           hn = Form("hChi2VsChi2_ch%d_nc%d_p%d_q%d",
                              ch,nc,q,p);
                           hChi2VsChi2[ch][nc][p][q] = new TH2F(hn.Data(),
                              Form("chi2 (ch%d|ch%d) vs chi2 (ch%d|ch%d)"
                                 ";log_{10}(#chi^{2}) (ch%d|ch%d)"
                                 ";log_{10}(#chi^{2}) (ch%d|ch%d)",
                                 ch,nc,p,q, ch,nc, p,q),
                              nchi2bins, chi2min, chi2max,
                              nchi2bins, chi2min, chi2max);
                        }
                     }
                  }
               }
            }
         }
      }
   }
   
   for (Long64_t ei=0, ent=0; ei<nents; ei++) {
      ent = ei + minEntry;
      nt->GetEntry(ent);
      
      if ( (ei%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            ei, nents,
            100.*static_cast<Float_t>(ei)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok = (trg==-1 || trg==extra[kTrgBitIdx]);
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
         // also check that data is not all zero
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs) && ok) {
               Bool_t allzero=kTRUE;
               for (Int_t sm=0; sm<kNsamps; sm++) {
                  if (samples[ch][sm]!=0) {
                     allzero=kFALSE;
                     break;
                  }
               }
               ok &= !allzero;
            }
         }
      }
      
      if (ok) {
         
         typedef std::pair<Int_t, Int_t> ChNc_t;
         typedef std::pair<Double_t, Int_t> Chi2Shift_t;
         std::vector< std::pair< ChNc_t, Chi2Shift_t > > chi2shifts;
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               const UShort_t* const chdata = samples[ch];
               for (Int_t nc=0; nc<ch; nc++) {
                  if (isActive(nc, activeChs)) {
                     const UShort_t* const ncdata = samples[nc];
                     Double_t minChi2=0, minShift=0;
                     // shift|wrap nc w.r.t. cc by each amt possible
                     for (Int_t sh=0; sh<kNsamps; sh++) {
                        const Int_t shift = sh - kHlfSmps;
                        Double_t chi2=0;
                        for (Int_t x=0; x<kNsamps; x++) {
                           // if stop is at x in ch and x+d in nc,
                           // then offset would be ch-nc = x - (x+d) = -d
                           // to show the shift in the same way, we
                           // want to compare samples x in ch and x+d in
                           // nc, but we also want the shift to be -d!
                           // so, we shift nc by x-shift, with shift=-d.
                           // this is actually more intiutive, since with
                           // this def., a shift of -d really means
                           // "move nc to the left by d relative to ch"
                           Int_t xsh = x-shift;
                           if (xsh>=kNsamps) {
                              xsh -= kNsamps;
                           } else if (xsh<0) {
                              xsh += kNsamps;
                           }
                           const Double_t chi = chdata[x] - ncdata[xsh];
                           chi2 += chi*chi;
                        }
                        hShiftChi2VsEvt[ch][nc]->Fill(ent, shift, chi2);
                        if (sh==0 || (chi2 < minChi2)) {
                           minChi2  = chi2;
                           minShift = shift;
                        }
                     }
                     const Double_t logchi = TMath::Log10(minChi2);
                     gMinChi2VsEvt[ch][nc]->SetPoint(
                        gMinChi2VsEvt[ch][nc]->GetN(), ent, logchi);
                        //minChi2);
                     gMinShiftVsEvt[ch][nc]->SetPoint(
                        gMinShiftVsEvt[ch][nc]->GetN(), ent, minShift);
                     hMinShift[ch][nc]->Fill(minShift);
                     hShiftVsChi2[ch][nc]->Fill(minShift, logchi);
                     chi2shifts.push_back(
                        std::pair<ChNc_t, Chi2Shift_t>(
                           ChNc_t(ch,nc), Chi2Shift_t(logchi, minShift) ) );
                  }
               }
            }
         }
         
         const UInt_t nprs = chi2shifts.size();
         for (UInt_t p=0; p<nprs; p++) {
            for (UInt_t q=0; q<p; q++) {
               const ChNc_t&     chncA = chi2shifts.at(p).first;
               const Chi2Shift_t& cshA = chi2shifts.at(p).second;
               const ChNc_t&     chncB = chi2shifts.at(q).first;
               const Chi2Shift_t& cshB = chi2shifts.at(q).second;
               hShiftVsShift[chncA.first][chncA.second]
                  [chncB.first][chncB.second]->Fill(
                     cshA.second, cshB.second);
               hChi2VsChi2[chncA.first][chncA.second]
                  [chncB.first][chncB.second]->Fill(
                     cshA.first, cshB.first);
            }
         }
         
      }
   }
   
   /*
   c1 = new TCanvas("c1","c1 - shift vs event",750,900);
   c1->Divide(2,3);
   for (Int_t ch=0,q=0; ch<kNchans; ch++) {
      if (isActive(ch, activeChs)) {
         for (Int_t nc=0; nc<ch; nc++) {
            if (isActive(nc, activeChs)) {
               c1->cd(++q);
               hShiftChi2VsEvt[ch][nc]->Draw("colz");
            }
         }
      }
   }
   c1->cd();
   c1->Update();
   */
   
   c2 = new TCanvas("c2","c2 - min chi2,shift vs event",1500,900);
   c2->Divide(4,3);
   for (Int_t ch=0,q=0; ch<kNchans; ch++) {
      if (isActive(ch, activeChs)) {
         for (Int_t nc=0; nc<ch; nc++) {
            if (isActive(nc, activeChs)) {
               c2->cd(++q);
               if (gMinChi2VsEvt[ch][nc]->GetN()>0) {
                  gMinChi2VsEvt[ch][nc]->Draw("ap");
               }
               c2->cd(++q);
               if (gMinShiftVsEvt[ch][nc]->GetN()>0) {
                  gMinShiftVsEvt[ch][nc]->Draw("ap");
               }
            }
         }
      }
   }
   c2->cd();
   c2->Update();
   
   c3 = new TCanvas("c3","c3 - offsets",750,900);
   c3->Divide(2,3);
   for (Int_t ch=0,q=0; ch<kNchans; ch++) {
      if (isActive(ch, activeChs)) {
         for (Int_t nc=0; nc<ch; nc++) {
            if (isActive(nc, activeChs)) {
               c3->cd(++q);
               hMinShift[ch][nc]->Draw();
            }
         }
      }
   }
   c3->cd();
   c3->Update();
   
   c4 = new TCanvas("c4","c4 - chi2 vs shift",750,900);
   c4->Divide(2,3);
   for (Int_t ch=0,q=0; ch<kNchans; ch++) {
      if (isActive(ch, activeChs)) {
         for (Int_t nc=0; nc<ch; nc++) {
            if (isActive(nc, activeChs)) {
               c4->cd(++q);
               hShiftVsChi2[ch][nc]->Draw("colz");
            }
         }
      }
   }
   c4->cd();
   c4->Update();
   
   c5 = new TCanvas("c5","c5 - shift vs shift",900,900);
   c5->Divide(2,3);
   c6 = new TCanvas("c6","c6 - chi2 vs chi2",900,900);
   c6->Divide(2,3);
   for (Int_t ch=0, pd=0; ch<kNchans; ch++) {
      if (isActive(ch, activeChs)) {
         for (Int_t nc=0; nc<ch; nc++) {
            if (isActive(nc, activeChs)) {
               for (Int_t p=0; p<kNchans; p++) {
                  if (isActive(p, activeChs)) {
                     for (Int_t q=0; q<kNchans; q++) {
                        if (isActive(q, activeChs)) {
                           if (hShiftVsShift[ch][nc][p][q]->GetEntries()>0) {
                              pd++;
                              c5->cd(pd);
                              hShiftVsShift[ch][nc][p][q]->Draw("colz");
                              c6->cd(pd);
                              hChi2VsChi2[ch][nc][p][q]->Draw("colz");
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   c5->cd();
   c5->Update();
   c6->cd();
   c6->Update();
   
}
