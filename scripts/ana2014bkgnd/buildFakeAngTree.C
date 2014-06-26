#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TGeoManager.h>
#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TString.h>
#include <TArrayD.h>
#include <TParameter.h>
#include <TMath.h>
#include <TGraph.h>
#include <TSpline.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TVector2.h>
#include <TStopwatch.h>

#include "NSnChanCorl.h"
#include "NSnConstants.h"
#include "TSnRecoChanOffsets.h"
#include "TSnCalWvData.h"
#include "TSnGeoStnSite.h"
#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"

#include <set>

#endif

//#define DEBUG

TGeoManager* gg(0);
const TSnGeoStnSite* site(0);

TFile* inf, * outf, * geof;
TTree* nnt, * ot;

const Float_t kNgTopFirn = 1.33;

void findEangHang(const TVector3& antnorm,
                  const TVector3& sourcedir,
                  Float_t& eang,
                  Float_t& hang) {
   // both should already be unit vectors
   
   // E angle projects onto the plane
   const TVector3 w = (antnorm.Dot(sourcedir)) * antnorm;
   const TVector3 v = sourcedir - w;
   eang = TMath::Abs( TMath::Pi() - v.Theta() );
   
   // H angle projects out of the plane
   hang = TMath::Abs( TMath::Pi() - w.Theta() );

#ifdef DEBUG
   Printf("w:");
   w.Print();
   Printf("v:");
   v.Print();
#endif

}

Float_t findNearestAllowedAngle(std::set<Float_t>& angles,
                                const Float_t ang) {
   // find closest allowed angle
   std::set<Float_t>::const_iterator
      hiang = angles.lower_bound(ang),
      loang = hiang;
   if (loang!=angles.begin()) {
      --loang;
   }
#ifdef DEBUG
      Printf("loang=%p, hiang=%p, end=%p",
             (void*)&(*loang), (void*)&(*hiang), (void*)&(*angles.end()));
#endif
   if ( (loang==angles.end()) || (hiang==angles.end()) ) {
      // too big; take the largest one
      return *(angles.rbegin());
   } else {
      // round to the nearest angle (round up if halfway)
#ifdef DEBUG
      Printf("loang=%g, hiang=%g, ang=%g",
             *loang, *hiang, ang);
#endif
      if ( TMath::Abs(*loang - ang) < TMath::Abs(*hiang - ang) ) {
         return *loang;
      } else {
         return *hiang;
      }
   }
}

void buildFakeAngTree(const Char_t* outtag,
                      const Float_t timereso,     // ns
                      const UInt_t  simevts=1,
                      const Float_t thetaOpt=400, // deg
                      const Float_t phiOpt=400,   // deg
                      const Float_t coneOpt=400,  // deg
                      const UInt_t  rseed=23192,
                      const Float_t norm=100.0,   // mV
                      const Float_t noise=20.0,   // mV
                      const Char_t* outdir="/data/users/cjreed/work/simEvts",
                      const Char_t* infn="/w2/arianna/jtatar/nt.sigtemps.root",
                      const Char_t* geofn="/data/users/cjreed/work/"
                                          "BounceStudy/Stn10/"
                                          "CampSiteGeometry.root") {
   // if any of the angles (thetaOpt, phiOpt, coneOpt) > 360, a random
   // value will be used instead
   //
   // expect angles in the Templates tree to be in degrees
   //
   // expect the waveforms in the Templates tree to have amplitude 1
   
   TRandom3 rnd(rseed);
   
   
   geof = TFile::Open(geofn);
   gg = dynamic_cast<TGeoManager*>(geof->Get("CampSite2013"));
   site = dynamic_cast<const TSnGeoStnSite*>(gg->GetTopVolume());
   
   
   TVector3 pos[NSnConstants::kNchans], nvec[NSnConstants::kNchans];
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      site->SetLPDAPosition(ch, pos[ch]);
      site->SetLPDANormalVec(ch, nvec[ch]);
      Printf("pos ch%d:",ch);
      pos[ch].Print();
      Printf("normal ch%d:",ch);
      nvec[ch].Print();
   }
   
   TArrayD zeros(6);
   
   
   inf = TFile::Open(infn);
   nnt = dynamic_cast<TTree*>(inf->Get("Templates"));
   
   TString infns(infn);
   TString indir;
   Int_t fl(0);
   if (infns.Contains('/')) {
      fl = infns.Last('/') + 1;
      indir = infns(0, fl-1);
   }
   TString plaininfn = infns(fl, infns.Length()-fl);
   TString outfn = Form("%s/FakeEvts.%s.%s", outdir, outtag,
                        plaininfn.Data());
   outf = TFile::Open(outfn.Data(),"recreate");
   outf->cd();
   TParameter<Float_t> trp("TimeResolution", timereso);
   trp.Write();
   TParameter<Float_t> nmp("Normalization", norm);
   nmp.Write();
   TParameter<Float_t> nop("NoiseRMS", noise);
   nop.Write();
   TParameter<UInt_t> rsp("RandomSeed", rseed);
   rsp.Write();
   
   
   TSnCalWvData* wave = new TSnCalWvData;
   Float_t eang(0), hang(0), hpf(0), limiter(0), coneang(0);
   Bool_t bice(kFALSE);
   nnt->SetBranchAddress("wave.",&wave);
   nnt->SetBranchAddress("EAng",&eang);
   nnt->SetBranchAddress("HAng",&hang);
   nnt->SetBranchAddress("hpf",&hpf);
   nnt->SetBranchAddress("limiter",&limiter);
   nnt->SetBranchAddress("coneAng",&coneang);
   nnt->SetBranchAddress("bIce",&bice);
   // to look up waveform for EAng, HAng
   nnt->BuildIndex("EAng + (1000*HAng)","coneAng");
   // find the max angles
   Printf("finding allowed angles...");
   std::set<Float_t> Eangs, Hangs, Cangs;
   const Long64_t nnents = nnt->GetEntries();
   for (Long64_t i=0; i<nnents; ++i) {
      nnt->GetEntry(i);
      Eangs.insert(eang);
      Hangs.insert(hang);
      Cangs.insert(coneang);
   }
#ifdef DEBUG
   std::set<Float_t>::const_iterator ang, end = Eangs.end();
   Printf("EAngs:");
   for (ang=Eangs.begin(); ang!=end; ++ang) {
      Printf("%g",*ang);
   }
   Printf("HAngs:");
   for (ang=Hangs.begin(), end=Hangs.end(); ang!=end; ++ang) {
      Printf("%g",*ang);
   }
   Printf("ConeAngs:");
   for (ang=Cangs.begin(), end=Cangs.end(); ang!=end; ++ang) {
      Printf("%g",*ang);
   }
#endif
   
   Float_t theta(0), phi(0), cone(0);
   Float_t EAng[NSnConstants::kNchans], 
           HAng[NSnConstants::kNchans];
   Float_t CAng(0);
   TSnCalWvData* evdat = new TSnCalWvData;
   TSnEventMetadata* meta = new TSnEventMetadata;
   TSnEventHeader* hdr = new TSnEventHeader;
   //ot = nnt->CloneTree(0);
   //ot->SetName("SimTemplEvts");
   ot = new TTree("SimTemplEvts","simulated events from templates",1);
   ot->SetDirectory(outf);
   ot->Branch("EventMetadata.",&meta);
   ot->Branch("EventHeader.",&hdr);
   ot->Branch("EAng",&(EAng[0]),Form("EAng[%hhu]/F",NSnConstants::kNchans));
   ot->Branch("HAng",&(HAng[0]),Form("HAng[%hhu]/F",NSnConstants::kNchans));
   ot->Branch("CAng",&CAng,"CAng/F");
   ot->Branch("theta",&theta,"theta/F");
   ot->Branch("phi",&phi,"phi/F");
   ot->Branch("NuData.",&evdat);
   // some useful aliases
   TString an;
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      // to use as a cut for a particular channel:
      an = Form("Ch%d",ch);
      ot->SetAlias(an.Data(),
                   Form("(Iteration$>=(%hhu*%hhu)) && (Iteration$<(%hhu*%hhu))",
                        NSnConstants::kNsamps, ch,
                        NSnConstants::kNsamps,
                        static_cast<UChar_t>(ch+1)));
      // to use as a variable showing the sample number [0,127] for any chan
      an = Form("SmpCh%d",ch);
      ot->SetAlias(an.Data(),
                   Form("Iteration$-%u", static_cast<UInt_t>(ch)
                        *static_cast<UInt_t>(NSnConstants::kNsamps)));
      // e.g. Draw("RawData.fData:SmpCh2","EventHeader.fNum==21 && Ch2","l")
   }

   Printf("generating events...");
   TStopwatch timer;
   timer.Start();
   
   for (UInt_t i=0; i<simevts; ++i) {
      
      if ( (i%1000)==0 ) {
         fprintf(stderr,"Processing %u/%u ...            \r",i,simevts);
      }
      
      // choose angles
      theta = (thetaOpt>360.) ? TMath::ACos( rnd.Uniform(-1.0, 0.0) ) 
                              : thetaOpt * TMath::DegToRad();
      phi   = (phiOpt>360.) ? rnd.Uniform(0.0, TMath::TwoPi())
                            : phiOpt * TMath::DegToRad();
      cone  = (coneOpt>360.) 
         ? rnd.Uniform(*(Cangs.begin()), *(Cangs.rbegin()))
         : coneOpt; // leave this one in degrees (as in the tree)
      CAng = findNearestAllowedAngle(Cangs, cone);
      
#ifdef DEBUG
      Printf("--- theta=%g, phi=%g, cone=%g",
             theta*TMath::RadToDeg(), phi*TMath::RadToDeg(), cone);
#endif
      
      // calculate channel shifts
      TArrayD pwdt = NSnChanCorl::GetPlaneWaveOffsets(theta,
                                                      phi,
                                                      zeros,
                                                      pos,
                                                      kNgTopFirn);
      TVector3 dir;
      dir.SetMagThetaPhi(1.0, theta, phi);
      
#ifdef DEBUG
      TObjArray graphs;
      graphs.SetOwner(kTRUE);
      TCanvas* c1 = new TCanvas("c1","c1",800,700);
      c1->Divide(2,2);
#endif
      
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         
         // look up the EAng, fhang for this antenna
         Float_t feang(0), fhang(0);
         findEangHang(nvec[ch], dir, feang, fhang);
         feang  = TMath::Abs(TVector2::Phi_mpi_pi(feang));
         fhang  = TMath::Abs(TVector2::Phi_mpi_pi(fhang));
         feang *= TMath::RadToDeg();
         fhang *= TMath::RadToDeg();
         // find closest allowed angle
         EAng[ch] = findNearestAllowedAngle(Eangs, feang);
         HAng[ch] = findNearestAllowedAngle(Hangs, fhang);
         const Long64_t ni = 
            nnt->GetEntryNumberWithIndex(EAng[ch] + (1000*HAng[ch]), CAng);
#ifdef DEBUG
         Printf("EAng=%g (%g), HAng=%g (%g), CAng=%g, ni=%lld",
                EAng[ch],feang,HAng[ch],fhang,CAng,ni);
#endif
         if (ni>-1) {
            nnt->GetEntry(ni);
#ifdef DEBUG
            c1->cd(ch+1);
            TGraph* och = wave->NewGraphForChan(0, kTRUE);
            const Int_t ochnp = och->GetN();
            Double_t* ochy = och->GetY();
            for (Int_t k=0; k<ochnp; ++k, ++ochy) {
               *ochy *= norm;
            }
            graphs.Add(och);
            och->SetLineColor(kBlack);
            och->SetMarkerColor(kBlack);
            och->SetMarkerStyle(7);
            och->Draw("apl");
#endif
            
            // first calculate the shift between chans due to the angle
            // ch0 is always unshifted; other chans shifted w.r.t. ch0
            // jitter the shift by the specified timing resolution
            const Double_t shift = 
               rnd.Gaus( (ch==0) ? 0.0
                            : -pwdt.At( TSnRecoChanOffsets::IndexFor(ch, 0) ),
                         timereso);
            // get a graph of the waveform
            // data only in channel 0 of the template
            TGraph* gch = wave->NewGraphForChan(0, kTRUE);
            // "fit" the graph with an spline interpolation
            TSpline3* gsp = new TSpline3("stmp", gch);
            // evaluate the spline at the new sample positions
            // (shifted, but NOT wrapped)
            // and save that into the event data waveform
            Float_t* d = evdat->GetData(ch);
            const Float_t tstep = 1.0 / NSnConstants::kSampRate;
            const Float_t tlast = static_cast<Float_t>(NSnConstants::kNsamps-1)
               / NSnConstants::kSampRate;
            Float_t xloc = shift;
            for (UChar_t s=0; s<NSnConstants::kNsamps; ++s, ++d, xloc+=tstep) {
               if ( (xloc<0.0) || (xloc>=tlast) ) {
                  *d = 0.0;
               } else {
                  *d = gsp->Eval( xloc );
               }
            }
#ifdef DEBUG
            Printf("ch%hhu: shift=%g, dt=%g", ch, shift,
                   (ch==0) ? 0.0
                   : pwdt.At( TSnRecoChanOffsets::IndexFor(ch, 0) ));
            
            TGraph* fch = evdat->NewGraphForChan(ch, kTRUE);
            Double_t* y = gch->GetY();
            Double_t* fy = fch->GetY();
            for (UChar_t s=0; s<NSnConstants::kNsamps; ++s, ++y, ++fy) {
               *y *= norm;
               *fy *= norm;
            }
            
            gch->SetLineColor(kRed+1);
            gch->SetMarkerColor(kRed+1);
            gch->SetMarkerStyle(7);
            gch->Draw("pl");
            
            delete gsp;
            gsp = new TSpline3("stmp",gch);
            gsp->SetLineColor(kAzure-6);
            gsp->SetMarkerColor(kAzure-6);
            gsp->SetMarkerStyle(7);
            gsp->Draw("pl same");
            
            graphs.Add(fch);
            fch->SetLineColor(kOrange+7);
            fch->SetMarkerColor(kOrange+7);
            fch->SetMarkerStyle(7);
            fch->Draw("pl");
#endif


            d = evdat->GetData(ch);
            // finally add noise to the waveform
            for (UChar_t s=0; s<NSnConstants::kNsamps; ++s, ++d) {
               *d = rnd.Gaus( (*d) * norm, noise );
            }
            
#ifdef DEBUG
            TGraph* nch = evdat->NewGraphForChan(ch, kTRUE);
            graphs.Add(nch);
            nch->SetLineColor(kGreen+2);
            nch->SetMarkerColor(kGreen+2);
            nch->SetMarkerStyle(7);
            nch->Draw("pl");
#endif
            
            // cleanup
#ifdef DEBUG
            graphs.Add(gch);
            graphs.Add(gsp);
#else
            delete gch;
            delete gsp;
#endif
         }

      } // end channel loop

#ifdef DEBUG
      TObject* o(0);
      while ( (o=c1->WaitPrimitive())!=0 ) {
         gSystem->ProcessEvents();
      }
      delete c1;
#endif
         
         // save this event
         ot->Fill();
         
   } // end event loop

   fprintf(stderr,"\n");

   timer.Stop();
   Printf("Finished generating events in:");
   timer.Print();
   
   outf->Write();
   
   Printf("Wrote [%s]",outf->GetName());
   
   delete outf; outf=0; // close file
}
