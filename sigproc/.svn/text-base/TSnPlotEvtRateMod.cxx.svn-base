#include "TSnPlotEvtRateMod.h"

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>

#include "TSnClockSetInfo.h"
#include "TSnRawTreeMaker.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

ClassImp(TSnPlotEvtRateMod);

TSnPlotEvtRateMod::TSnPlotEvtRateMod(const Char_t* name) :
   TAModule(name, "make event rate and dt plots"),
   fTwin(300), fConsDTbw(0.5), fCombDTbw(0.1), fAbsTimeNm("AbsEvtTime"),
   fSecResOk(kTRUE), fNtimeBins(100), fTimeMin(0), fTimeMax(100),
   fEnumVsTnm("gEnumVsT"), fERvsTnm("gERvsT"), fERvsEnumNm("gERvsEnum"),
   fRawDTnm("hRawDT"), fDTnm("hConsDT"), fCombDTnm("hCombDT"),
   fDTvsTnm("hConsDTvsT"), fDTvsTgrNm("gConsDTvsT"),
   fCombDTvsTnm("hCombDTvsT"), fNumEvtsNm("hNumEvts"), fLivetimeNm("hLivetime"),
   fEvtNumsNm("hAveEvtNums"), fRateVsTnm("hRateVsT"),
   fNumEvts(0), fLivetime(0), fEvtNums(0), fRateVsT(0),
   fEnumVsT(0), fERvsT(0), fERvsEnum(0), fRawDT(0),
   fDT(0), fCombDT(0), fDTvsT(0), fDTvsTgr(0), fCombDTvsT(0),
   fHdr(0), fMeta(0), fPrevMeta(new TSnEventMetadata), fStop(0) {
}

TSnPlotEvtRateMod::~TSnPlotEvtRateMod() {
   // delete the output histograms
   delete fNumEvts;
   delete fLivetime;
   delete fEnumVsT;
   delete fERvsT;
   delete fERvsEnum;
   delete fRawDT;
   delete fDT;
   delete fCombDT;
   delete fDTvsT;
   delete fDTvsTgr;
   delete fCombDTvsT;
   delete fPrevMeta;
}

void TSnPlotEvtRateMod::Begin() {
   if (fTwin<=0) {
      SendError(kAbortAnalysis, "Begin",
                "Time window must be larger than 0!");
   }
}

void TSnPlotEvtRateMod::SlaveBegin() {

   // request branch(es)
   ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMeta);
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   ReqBranch(TSnRawTreeMaker::kCSpBrNm, fStop);
   
   // make output histograms/graphs
   fEnumVsT = new TGraph;
   fEnumVsT->SetName(fEnumVsTnm.Data());
   fEnumVsT->SetTitle("Event Num vs Time; Date, time (UTC);Event Num");
   AddOutput(fEnumVsT);
   fERvsT = new TGraphErrors;
   fERvsT->SetName(fERvsTnm.Data());
   fERvsT->SetTitle("Event Rate vs Time; Date, time (UTC);Event Rate (Hz)");
   AddOutput(fERvsT);
   fERvsEnum = new TGraphErrors;
   fERvsEnum->SetName(fERvsEnumNm.Data());
   fERvsEnum->SetTitle("Event Rate vs Event Num;Event Num;Event Rate (Hz)");
   AddOutput(fERvsEnum);
   
   const Int_t   ntotTbins = TMath::CeilNint((fTimeMax-fTimeMin)/fTwin);
   const Double_t    tbmax = fTimeMin+(ntotTbins*fTwin);
   fNumEvts = new TH1F(fNumEvtsNm.Data(),
                       Form("Num Evts vs Time; Date, time (UTC);"
                            "Nevts / %g s",fTwin),
                       ntotTbins, fTimeMin, tbmax);
   fNumEvts->Sumw2();
   AddOutput(fNumEvts);
   fLivetime = new TH1F(fLivetimeNm.Data(),
                        "Livetime vs Time; Date, time (UTC);Livetime s",
                        ntotTbins, fTimeMin, tbmax);
   AddOutput(fLivetime);
   fEvtNums = new TH1F(fEvtNumsNm.Data(),
                       "Event Number vs Time; Date, time (UTC);Event Num",
                       ntotTbins, fTimeMin, tbmax);
   fEvtNums->Sumw2();
   AddOutput(fEvtNums);
   
   const Int_t    nConsDTbins = TMath::Nint(fTwin / fConsDTbw) + 1;
   const Double_t consDTmin = -0.5 * fConsDTbw;
   const Double_t consDTmax = fTwin + (0.5 * fConsDTbw);
   const Int_t    nCombDTbins = TMath::Nint(fTwin / fCombDTbw) + 1;
   const Double_t combDTmin = -0.5 * fCombDTbw;
   const Double_t combDTmax = fTwin + (0.5 * fCombDTbw);
   fRawDT = new TH1F(fRawDTnm.Data(),
                     "Consecutive #Deltat from event;#Deltat from MBED (s)",
                     nConsDTbins, consDTmin, consDTmax);
   AddOutput(fRawDT);
   fDT = new TH1F(fDTnm.Data(),
                  "Consecutive #Deltat dist;t_{i} - t_{i-1} (s)",
                  nConsDTbins, consDTmin, consDTmax);
   AddOutput(fDT);
   fCombDT = new TH1F(fCombDTnm.Data(),
                      "Combinatorial #Deltat dis;"
                      "t_{i} - t_{j} (s), j<i",
                      nCombDTbins, combDTmin, combDTmax);
   AddOutput(fCombDT);
   fDTvsT = new TH2F(fDTvsTnm.Data(),
                     "Consecutive #Deltat dist;Date, time (UTC);"
                     "t_{i} - t_{i-1} (s)",
                     fNtimeBins, fTimeMin, fTimeMax,
                     nConsDTbins, consDTmin, consDTmax);
   AddOutput(fDTvsT);
   fDTvsTgr = new TGraph;
   fDTvsTgr->SetName(fDTvsTgrNm.Data());
   fDTvsTgr->SetTitle("Consecutive #Deltat vs time;Date, time (UTC);"
                      "t_{i} - t_{i-1} (s)");
   AddOutput(fDTvsTgr);
   fCombDTvsT = new TH2F(fCombDTvsTnm.Data(),
                         "Combinatorial #Deltat vs time;Date, time (UTC);"
                         "t_{i} - t_{j} (s), j<i",
                         fNtimeBins, fTimeMin, fTimeMax,
                         nCombDTbins, combDTmin, combDTmax);
   AddOutput(fCombDTvsT);
   
}

void TSnPlotEvtRateMod::Process() {
   // process the event. no event selection is done here (an event selection
   // module should be used for that).
   //
   // simply stores the event time for sorting and processing in ProcessTimes.

   LoadBranch(TSnRawTreeMaker::kEMtBrNm);
   if (fMeta!=0) {
      if (  (fPrevMeta==0) || 
           ((fPrevMeta!=0) && (fPrevMeta->IsEqual(fMeta)==kFALSE)) ) {
         // process times of the old sequence
         ProcessTimes();
         *fPrevMeta = *fMeta;
      }
      
      LoadBranch(TSnRawTreeMaker::kCSpBrNm);
      LoadBranch(TSnRawTreeMaker::kEHdBrNm);
      if (0!=fHdr) {
      
         const TParameter<Double_t>* at = 
            dynamic_cast< const TParameter<Double_t>* >(
               FindObjThisEvt(fAbsTimeNm.Data()) );

         if (  (at!=0) ||
               ((at==0) && fSecResOk) ) {
            // if we have the absolute time, use that. otherwise use evt
            // time (if we've been told it's ok to do so)
            const Double_t et = (at!=0) ? (at->GetVal())
               : static_cast<Double_t>(fHdr->GetTime().GetSec());

            fEnumVsT->SetPoint(fEnumVsT->GetN(), et, fHdr->GetEvtNum());
            fRawDT->Fill(fHdr->GetDTms()/1000.0);

            fNTs.push_back( NumTime_t(fHdr->GetEvtNum(), et) );
         } else {
            SendError(kAbortModule, "Process",
                      "Could not get absolute time parameter [%s] from event",
                      fAbsTimeNm.Data());
         }

      } else {
         SendError(kAbortModule, "Process",
                   "Could not load event header.");
      }
   } else {
      SendError(kAbortModule, "Process",
                "Could not load event metadata.");
   }
}

void TSnPlotEvtRateMod::ProcessTimes() {
   // loop over fNTs (the event numbers and absoulte times) to find
   // event rates and delta-t distributions
   //
   // it is assumed that the events are already in order.  as this
   // function is called each time the seq changs, this needs to be
   // true only for a single file -- which it is for the stations run
   // by the mbed. however, we are assuming that an entire file is
   // processed. this might not be always true if running under Proof.
   // for the old station with the Helios CPU, this may
   // not have been the case. to process such data, the tree/chain
   // should be sorted and a TEventList used such that events would be
   // served in order.  it's doubtful that such data would ever be
   // processed with this module, however.
   //
   // we also assume that there is no dead time between events. that is,
   // that the only dead time is due to the throttle, which will simply limit
   // the maximum observable rate. we do not allow for files to be closed and
   // reopened between the events we process here. this should be the case
   // naturally, since this function gets called from Notify (once per file)

   Bool_t isFirst = kTRUE;
   Double_t prev=0;
   Double_t start=0;
   
   // p = first event of the time window; q=event in combinatorial loop (q<p)
   VectNT_t::const_iterator e, p, q, r, end=fNTs.end();
   for (e=fNTs.begin(), p=fNTs.begin(); e!=end; ++e) {
      const Num_t&     evn = e->first;
      const Double_t&  dat = e->second;
      
      // consecutive dt's
      if (isFirst==kFALSE) {
         const Double_t cdt = dat - prev;
         fDT->Fill(cdt);
         fDTvsT->Fill(dat, cdt);
         fDTvsTgr->SetPoint(fDTvsTgr->GetN(), dat, cdt);
      } else {
         // first event, set up:
         start   = dat;
         prev    = dat;
      }
      
      // event rates
      // Fill searches the time axis every time, which is not optimal,
      // but AddBinContents does not keep track of statistics (for errors)
      // whereas Fill does. So rather than optimize by exploiting that events
      // are in order and keeping track of the current bin, we just use Fill
      fNumEvts->Fill(dat);
      fEvtNums->Fill(dat, evn);
      
      // combinatorial dt's: this event - all prev events in the time window
      // p = first event of this twin, e = current event in the window
      // q = each event in [p, e)
      for (q=p; q!=e; ++q) {
         const Double_t combdt = dat - (q->second);
         fCombDT->Fill(combdt);
         fCombDTvsT->Fill(dat, combdt);
      }
         
      // update for next event
      prev = dat;
      isFirst = kFALSE;
   }
   
   // after last event, see if we can find the end of the file
   // if not, use the time of the last event. it's biased high,
   // but so be it.
   // keeping the time window the same (biasing low) leads to
   // very very biased (just wrong) results when the rates are high
   // and the number of events is much less than evtsPerSeq/rate
   // e.g. for rates~15Hz, one would need 1800 evts/seq for 120sec twin
   const Double_t finish = (fStop!=0) ? fStop->CalcAbsCurrTime().AsDouble() 
      : prev; // 'prev' => bias towards high rate
   Int_t b = fLivetime->FindBin(start);
   const Int_t eb = fLivetime->FindBin(finish);
   if (eb==b) {
      // start and stop in same time window
      fLivetime->AddBinContent(b, finish-start);
   } else {
      fLivetime->AddBinContent(b, 
                               (fLivetime->GetBinLowEdge(b) + fTwin - start));
      fLivetime->AddBinContent(eb, (finish - fLivetime->GetBinLowEdge(eb)));
      for (++b; b<eb; ++b) {
         fLivetime->AddBinContent(b, fTwin);
      }
   }
   
   
   // done. reset the time array
   fNTs.clear();
   fStop    = 0;
   
}

void TSnPlotEvtRateMod::SlaveTerminate() {
   // process the last bit
   ProcessTimes();
}

void TSnPlotEvtRateMod::Terminate() {
   // get the rate histogram and put points in the graphs
   
   // rate hist
   fRateVsT = dynamic_cast<TH1F*>(fNumEvts->Clone());
   fRateVsT->SetName(fRateVsTnm.Data());
   fRateVsT->SetTitle(Form("Event Rate vs Time; Date, time (UTC);"
                            "Rate in %g s bins",fTwin));
   fRateVsT->Reset();
   // for some reason fRateVsT->Divide(fLivetime) gets the errors wrong,
   // so we do it ourselves
   const Int_t nbins = fRateVsT->GetNbinsX();
   Double_t lt;
   for (Int_t i=nbins; i>0; --i) {
      lt = fLivetime->GetBinContent(i);
      if (lt>0) {
         fRateVsT->SetBinContent(i, fNumEvts->GetBinContent(i) / lt);
         fRateVsT->SetBinError(  i, fNumEvts->GetBinError(i) / lt);
      }
   }
   AddOutput(fRateVsT);
   
   // average event numbers
   fEvtNums->Divide(fNumEvts);
   
   // graphs
   const Double_t hdt = fTwin/2.0;
   Int_t n;
   for (Int_t i=0; i<nbins; ++i) {
      const Double_t rate = fRateVsT->GetBinContent(i+1);
      if (rate>0) {
         const Double_t bc = fRateVsT->GetBinCenter(i+1);
         const Double_t re = fRateVsT->GetBinError(i+1);
         n = fERvsT->GetN();
         fERvsT->SetPoint(n, bc, rate);
         fERvsT->SetPointError(n, hdt, re);
         const Double_t ae = fEvtNums->GetBinContent(i+1);
         n = fERvsEnum->GetN();
         fERvsEnum->SetPoint(n, ae, rate);
         fERvsEnum->SetPointError(n, fEvtNums->GetBinWidth(i+1)/2.0, re);
      }
   }
}

