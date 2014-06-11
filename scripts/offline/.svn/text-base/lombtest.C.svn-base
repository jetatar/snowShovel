#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TCanvas.h>
#include <TGraph.h>
#include <TF1.h>
#include <TError.h>
#include <TString.h>
#include <TRandom.h>
#include <TMath.h>
#include <TLine.h>
#include <TPad.h>
#include <TLatex.h>
#include <TH1.h>
#include <TH2.h>
#include <TStopwatch.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TLeaf.h>

#include <fstream>

using namespace std;
#endif

static const Float_t  kEpsilon = 1e-8;
static const Double_t kTwoPi   = TMath::TwoPi();
static const UInt_t   kMACC    = 4; // num interpolation points per 1/4 cycle
                                    // of highest frequency

enum EWinType_t {
   kSquare,
   kWelch,
   kBartlett
};

void setContours(TH1* h, const Bool_t log);
void drawSigLines(const Float_t M);
template<typename Num_t>
void avevar(const Num_t* const data, const UInt_t n,
            Num_t& ave, Num_t& var);
Float_t getRange(const Float_t* const x, const UInt_t n,
                 Float_t& xmin, Float_t& xmax);
void slowLomb(const Float_t* const x, const Float_t* const y, const UInt_t n,
              const Float_t ofac,   // oversampling factor (>=4 usually)
              const Float_t hifac,  // max frequency = hifac * <f_c>
              Float_t* const px, Float_t* const py, const UInt_t np, // output
              UInt_t&  nout,
              UInt_t&  jmax,
              Float_t& prob);
template<typename Num_t>
void four1(Num_t* const data, const UInt_t nn, Int_t isign);
template<typename Num_t>
void realft(Num_t* const data, const UInt_t n, Int_t isign);
void spread(const Float_t y, Float_t* const yy, const UInt_t n,
            const Float_t x, const UInt_t m);
void fastLomb(const Float_t* const x, const Float_t* const y, const UInt_t n,
              const Float_t ofac, const Float_t hifac,
              Float_t* const wk1, Float_t* const wk2, const UInt_t nwk,
              UInt_t& nout, UInt_t& jmax, Float_t& prob);
template<typename Num_t>
void twofft(Num_t* data1, Num_t* data2,
            Num_t* fft1, Num_t* fft2,
            const UInt_t n);
template<typename Num_t>
void correl(Num_t* data1, Num_t* data2,
            const UInt_t n,
            Num_t* ans);

template<class Hist_t, typename Num_t>
Bool_t checkDataType(Hist_t* h, Num_t& brdata);
template<typename Num_t>
Bool_t checkDataType(TBranch* br, Num_t& brdata);
template<typename Num_t>
void copyDataTo(TBranch* br, const Int_t entry, Num_t& source, Num_t& dest);
void copyDataTo(TH1F* h, const Int_t bin, Float_t& source, Float_t& dest);
void copyDataTo(TH1D* h, const Int_t bin, Double_t& source, Double_t& dest);
template<class DSource_t, typename Num_t>
void spectrum(DSource_t* br, Num_t* const p, const Int_t m,
              const Int_t k, const Bool_t overlap,
              const Int_t firstEntry=0,
              const EWinType_t window=kBartlett);
Float_t SquareWin(const Int_t i, const Float_t a, const Float_t b);
Float_t WelchWin(const Int_t i, const Float_t a, const Float_t b);
Float_t BartlettWin(const Int_t i, const Float_t a, const Float_t b);


TCanvas* c1, * c2, * c3;
TGraph* gr, * gout, * gfast;
TH2* hsig, * hsiglog;
TF1* f1;

void lombtest() {
   
   c1 = new TCanvas("c1","c1",1200,800);
   
   const Float_t amp=3.0, freq=0.8;
   Float_t xmin=0, xmax=100;
   f1 = new TF1("f1",
      Form("(%g*sin(2*pi*%g*x))",amp,freq),
      xmin,xmax);
   f1->SetLineStyle(7);
   f1->SetLineColor(kAzure-2);
   f1->SetLineWidth(1);
   f1->SetNpx(1000);
   
   const Float_t xsmear=0.05, ysmear = 3.;
   
   const UInt_t n = 100;
   gr = new TGraph(n);
   Float_t* x = new Float_t[n],
          * y = new Float_t[n],
          * dx = x,
          * dy = y;
   for (UInt_t i=0; i<n; i++, dx++, dy++) {
      *dx  = xmin + (gRandom->Rndm() * (xmax-xmin));
      *dy  = f1->Eval(*dx) + gRandom->Gaus(0,ysmear);
      *dx += gRandom->Gaus(0,xsmear);
      gr->SetPoint(i, *dx, *dy);
   }
   
   const Float_t ofac=4, hifac=2;
   const UInt_t  np = static_cast<UInt_t>(0.5*(ofac*hifac)*n);
   const UInt_t  nfreqt = static_cast<UInt_t>(ofac*hifac*n*kMACC);
         UInt_t  nfreq  = 64;
   while ( nfreq < nfreqt ) nfreq <<= 1;
   const UInt_t  nwk = nfreq << 1;
   Printf("nwk=%u",nwk);
   Float_t* px = new Float_t[np],
          * py = new Float_t[np],
          * wx = new Float_t[nwk],
          * wy = new Float_t[nwk];

   UInt_t  nout=0, jmax=0;
   Float_t prob=0;
   TStopwatch slow;
   slow.Start();
   slowLomb(x, y, n,
            ofac, hifac,
            px, py, np,
            nout, jmax, prob);
   slow.Stop(); slow.Print();
   Printf("nout=%u, jmax=%u, prob=%g, px=%g, py=%g",
      nout,jmax,prob,px[jmax],py[jmax]);
   gout = new TGraph(nout, px, py);

   UInt_t fnout=0, fjmax=0;
   Float_t fprob=0;
   TStopwatch fast;
   fast.Start();
   fastLomb(x, y, n,
            ofac, hifac,
            wx, wy, nwk,
            fnout, fjmax, fprob);
   fast.Stop(); fast.Print();
   Printf("fnout=%u, fjmax=%u, fprob=%g, wx=%g, wy=%g",
      fnout,fjmax,fprob, wx[fjmax], wy[fjmax]);
   gfast = new TGraph(fnout, wx, wy);
   
   c1->Divide(2,2);
   c1->cd(1);
   gr->SetMarkerStyle(24);
   gr->SetTitle(Form("%s + gaussian smear (#sigma_{t}=%g, #sigma_{a}=%g)"
                     ";time (s);amplitude (au)",
                     f1->GetTitle(),xsmear, ysmear));
   gr->Draw("ap");
   //f1->Draw("same");
   //gr->Draw("p");

   c1->cd(2);
   TH1* hd = gr->GetHistogram()->DrawCopy("axis");
   hd->GetXaxis()->SetRangeUser(xmin+0.30*(xmax-xmin),
                                xmin+0.45*(xmax-xmin));
   f1->Draw("same");
   gr->Draw("p");

   const Float_t M = 2.0 * nout / ofac;

   c1->cd(3);
   gout->SetMarkerStyle(3);
   gout->SetTitle(
      Form("Lomb spectral analysis (t=%g);frequency (Hz);power",
         slow.RealTime()));
   gout->Draw("alp");
   c1->Update();
   drawSigLines(M);
      
   c1->cd(4);
   gfast->SetMarkerStyle(3);
   gfast->SetTitle(
      Form("FastLomb spectral analysis (t=%g);frequency (Hz);power",
         fast.RealTime()));
   gfast->Draw("alp");
   c1->Update();
   drawSigLines(M);
   
   c1->cd();
   
   if (xsmear<kEpsilon && ysmear<kEpsilon) {
      // map out sigma's
      // re-make the data (no smearing)
      dx = x; dy = y;
      Float_t* smx = new Float_t[n],
             * smy = new Float_t[n],
             * dsx = smx,
             * dsy = smy;
      const UInt_t  nsteps=250;
      const Float_t sxmin = 0, sxmax = 0.5,
                    symin = 0, symax = 2.50;
      const UInt_t goodbin = fjmax;
      Float_t xsmr, ysmr;
      hsig = new TH2F("hsig",
                      "Spectral power of correct frequency vs smearing"
                      ";#sigma_{time} / period;#sigma_{amp} / amplitude;"
                      "power",
         nsteps, sxmin, sxmax,
         nsteps, symin, symax);
      hsig->GetXaxis()->SetNdivisions(505);
      hsiglog = new TH2F("hsiglog",
                         "Spectral power of correct frequency vs smearing"
                         ";#sigma_{time} / period;#sigma_{amp} / amplitude;"
                         "null hypothesis probability",
         nsteps, sxmin, sxmax,
         nsteps, symin, symax);
      hsiglog->GetXaxis()->SetNdivisions(505);
      for (UInt_t j=nsteps; j>0; j--) {
         xsmr = hsig->GetXaxis()->GetBinCenter(j);
         for (UInt_t k=nsteps; k>0; k--) {
            ysmr = hsig->GetYaxis()->GetBinCenter(k);
            // smear the data
            dsx = smx; dsy = smy; dx = x; dy = y;
            for (UInt_t i=0; i<n; i++, dx++, dy++, dsx++, dsy++) {
               *dsx = *dx + gRandom->Gaus(0,xsmr/freq);
               *dsy = *dy + gRandom->Gaus(0,ysmr*amp);
            }
            // doit
            fastLomb(smx, smy, n,
                     ofac, hifac,
                     wx, wy, nwk,
                     fnout, fjmax, fprob);
            //Printf("smrx=%g, smry=%g, wy[%u]=%g",
            //   xsmr, ysmr, goodbin, wy[goodbin]);
            hsig->SetBinContent(j, k, wy[goodbin]);

            // evaluate significance of maximum
            const Float_t expy = TMath::Exp(-wy[goodbin]);
            const Float_t effm = 2.0*fnout/ofac;
            Float_t prob = effm*expy;
            if (prob>0.01) {
               prob = 1.0 - TMath::Power(1.0-expy, effm);
            }
            hsiglog->SetBinContent(j, k, TMath::Log10(prob));
         }
      }

      c2 = new TCanvas;
      c2->cd();
      hsig->Draw("colz");

      c3 = new TCanvas;
      c3->cd();
      setContours(hsiglog, kTRUE);
      hsiglog->Draw("cont4z");
   
      delete[] smx; smx=0;
      delete[] smy; smy=0;
   }
   
   delete[] px; px=0;
   delete[] py; py=0;
   delete[] wx; wx=0;
   delete[] wy; wy=0;
   delete[] x; x=0;
   delete[] y; y=0;
}

void setContours(TH1* h, const Bool_t log) {
   const Int_t ncont = 4;
   Double_t conts[ncont] = {
      -19, -9, -3, -1
   };
   if (log==kFALSE) {
      for (Int_t i=0; i<ncont; i++) {
         conts[i] = TMath::Power(10.0, conts[i]);
      }
   }
   h->SetContour(ncont, conts);
}

void drawSigLines(const Float_t M) {
   // draw some significances
   const UInt_t  nsigs=4;
   const Float_t psigs[nsigs] = {1e-1, 1e-3, 1e-9, 1e-12};
   Bool_t drawlbl = kFALSE;
   for (UInt_t i=0; i<nsigs; i++) {
      const Float_t ys = -TMath::Log(1.0-TMath::Power(1.0-psigs[i], 1/M));
      if (ys < gPad->GetUymax() && ys > gPad->GetUymin() ) {
         Printf("ps=%g, ys=%g", psigs[i], ys);
         TLine*  ln = new TLine(gPad->GetUxmin(),ys,gPad->GetUxmax(),ys);
         ln->SetLineStyle(3);
         ln->SetLineColor(kCyan-2);
         ln->Draw();
         const Float_t tx = gPad->GetUxmin() + 
            0.05*(gPad->GetUxmax()-gPad->GetUxmin());
         TLatex* lb = new TLatex(tx,
                                 ys + 0.01*(gPad->GetUymax()-gPad->GetUymin()),
                                 Form("%1.1e",psigs[i]));
         lb->SetTextColor(kCyan-2);
         lb->Draw();
         drawlbl = kTRUE;
      }
   }
   if (drawlbl) {
      TLatex* lbl = new TLatex(
         gPad->GetUxmin() + 0.25*(gPad->GetUxmax()-gPad->GetUxmin()),
         gPad->GetUymin() + 0.50*(gPad->GetUymax()-gPad->GetUymin()),
         "null hypoth. prob.");
      lbl->SetTextColor(kCyan-2);
      lbl->SetTextAngle(32);
      lbl->Draw();
   }
}

template<typename Num_t>
void avevar(const Num_t* const data, const UInt_t n,
            Num_t& ave, Num_t& var) {
   // use corrected two-pass algorithm to minimize roundoff error
   // NUM REC C - 14.1, eqn 14.1.8
   
   ave = var = 0;
   if (n>0) {
      const Num_t* d  = data;
      const Num_t  fn = static_cast<Num_t>(n);
      for (UInt_t i=0; i<n; i++, d++) {
         ave += *d;
      }
      ave /= fn;
      
      d = data;
      Num_t df, tdf=0, tdf2=0;
      for (UInt_t i=0; i<n; i++, d++) {
         df    = *d - ave;
         tdf  += df;
         tdf2 += df*df;
      }
      var  = tdf2 - ((tdf*tdf)/fn);
      var /= fn-1.0;
   }
}

Float_t getRange(const Float_t* const x, const UInt_t n,
                 Float_t& xmin, Float_t& xmax) {
   // find range of x
   
   xmin=*x, xmax=*x;
   const Float_t* dx = x;
   for (register UInt_t j=1; j<n; j++, dx++) {
      if (*dx > xmax) xmax = *dx;
      if (*dx < xmin) xmin = *dx;
   }
   return (xmax - xmin);
}

void slowLomb(const Float_t* const x, const Float_t* const y, const UInt_t n,
              const Float_t ofac,   // oversampling factor (>=4 usually)
              const Float_t hifac,  // max frequency = hifac * <f_c>
              Float_t* const px, Float_t* const py, const UInt_t np, // output
              UInt_t&  nout,
              UInt_t&  jmax,
              Float_t& prob) {
   // NUM REC C - 13.8, pg 579-581
   
   nout = static_cast<UInt_t>(0.5*ofac*hifac*n);
   if (nout > np) {
      Error("slowLomb","Output arrays too small.");
      return;
   }
   
   Float_t ave, var;
   avevar(y, n, ave, var);
   if (var < kEpsilon) {
      Error("slowLomb","No variance.");
      return;
   }
   
   // find range of x
   Float_t xmin=*x, xmax=*x;
   const Float_t xdif = getRange(x, n, xmin, xmax);
   const Float_t xave = 0.5*(xmax+xmin);
   
   const Float_t pincr = 1.0/(xdif*ofac);
   Float_t pnow=pincr; // starting freq
   
   // trig recurrences
   Double_t  arg;
   Double_t* const wi   = new Double_t[n];
   Double_t* const wpi  = new Double_t[n];
   Double_t* const wpr  = new Double_t[n];
   Double_t* const wr   = new Double_t[n];
   Double_t* dwi        = wi;
   Double_t* dwpi       = wpi;
   Double_t* dwpr       = wpr;
   Double_t* dwr        = wr;
   const Float_t* dx=x, * dy=y;
   register UInt_t i,j;
   for (j=0; j<n; j++, dx++, dwi++, dwpi++, dwpr++, dwr++) {
      arg   = kTwoPi * ( (*dx - xave) * pnow );
      *dwpr = TMath::Sin(0.5*arg);
      *dwpr = -2.0*((*dwpr)*(*dwpr));
      *dwpi = TMath::Sin(arg);
      *dwr  = TMath::Cos(arg);
      *dwi  = *dwpi;
   }
   
   // main loop over frequencies
   Float_t pymax=0;
   Float_t sumsh, sumc, sums, sumsy, sumcy,
           wtau, swtau, cwtau, wtemp,
           ss, cc, yy;
   Float_t* dpx=px, * dpy=py;
   for (i=0; i<nout; i++, dpx++, dpy++) {
      *dpx = pnow;
      sumsh = sumc = 0;
      dwr = wr; dwi = wi;
      for (j=0; j<n; j++, dwr++, dwi++) {
         sumsh += (*dwi) * (*dwr);
         sumc  += (*dwr - *dwi) * (*dwr + *dwi);
      }
      wtau  = 0.5*TMath::ATan2(2.0*sumsh, sumc);
      swtau = TMath::Sin(wtau);
      cwtau = TMath::Cos(wtau);
      sums = sumc = sumsy = sumcy = 0;
      dwr = wr; dwi = wi; dwpr=wpr; dwpi=wpi; dy=y;
      for (j=0; j<n; j++, dwr++, dwi++, dwpr++, dwpi++, dy++) {
         ss     = (*dwi * cwtau) - (*dwr * swtau);
         cc     = (*dwr * cwtau) + (*dwi * swtau);
         sums  += ss*ss;
         sumc  += cc*cc;
         yy     = *dy - ave;
         sumsy += yy*ss;
         sumcy += yy*cc;
         wtemp  = *dwr;
         *dwr   = (wtemp*(*dwpr) - (*dwi)*(*dwpi)) + (*dwr);
         *dwi   = ((*dwi)*(*dwpr) + wtemp*(*dwpi)) + (*dwi);
      }
      *dpy = 0.5*(sumcy*(sumcy/sumc) + sumsy*(sumsy/sums))/var;
      if (*dpy >= pymax) {
          jmax = i;
         pymax = *dpy;
      }
      pnow += pincr;
   }
   // evaluate significance of maximum
   const Float_t expy = TMath::Exp(-pymax);
   const Float_t effm = 2.0*nout/ofac;
   prob = effm*expy;
   if (prob>0.01) {
      prob = 1.0 - TMath::Power(1.0-expy, effm);
   }
   
   // cleanup
   delete[] wi;
   delete[] wpi;
   delete[] wpr;
   delete[] wr;
}

void fastLomb(const Float_t* const x, const Float_t* const y, const UInt_t n,
              const Float_t ofac, const Float_t hifac,
              Float_t* const wk1, Float_t* const wk2, const UInt_t nwk,
              UInt_t& nout, UInt_t& jmax, Float_t& prob) {
   // NUM REC C - 13.8, pg 582-584
   //
   // workspace arrays must be at least the power of 2 larger than
   // ofac * hifac * n * kMACC
   
   nout = static_cast<UInt_t>(0.5*ofac*hifac*n);
   const UInt_t nfreqt = static_cast<UInt_t>(ofac*hifac*n*kMACC);
   UInt_t nfreq = 64;
   while ( nfreq < nfreqt ) nfreq <<= 1;
   const UInt_t ndim = nfreq << 1;
   if (ndim > nwk) {
      Error("fastLomb","Workspace arrays too small (%u < %u)",
         nwk, ndim);
      return;
   }
   Float_t ave, var;
   avevar(y, n, ave, var);
   if (var < kEpsilon) {
      Error("fastLomb","Variance too small");
      return;
   }
   const Float_t twovar = 2.0*var;
   
   // range
   Float_t xmin=*x, xmax=*x;
   const Float_t xdif = getRange(x, n, xmin, xmax);
   
   // zero the workspaces
   memset(wk1, 0, ndim*sizeof(Float_t));
   memset(wk2, 0, ndim*sizeof(Float_t));
   
   const Float_t fndim = static_cast<Float_t>(ndim);
   const Float_t fac   = fndim / (xdif*ofac);
   
   // extirpolate the data to the workspaces
   Float_t ck, ckk;
   const Float_t* dx=x, * dy=y;
   register UInt_t j;
   for (j=0; j<n; j++, dx++, dy++) {
      ck  = (*dx - xmin)*fac;
      while (ck  >= fndim) ck  -= fndim; // ck mod fndim
      ckk = 2.0*ck;
      while (ckk >= fndim) ckk -= fndim; // ckk mod fndim
      ck++;
      ckk++;
      /*
      ofstream outf("wk2.txt");
      outf << ckk << endl;
      outf << ndim << endl;
      for (UInt_t k=0; k<ndim; k++) {
         outf << wk2[k] << endl;
      }
      outf.close();
      */
      spread(*dy - ave, wk1, ndim, ck,  kMACC);
      spread(1.0,       wk2, ndim, ckk, kMACC);
      /*
      ofstream outs("wk2_spread.txt");
      outs << ckk << endl;
      outs << ndim << endl;
      for (UInt_t k=0; k<ndim; k++) {
         outs << wk2[k] << endl;
      }
      outs.close();
      static UInt_t kjdsl=0;
      if (kjdsl++>11) return;
      */
   }

   // Take the FFT's
   realft(wk1, ndim, 1);
   realft(wk2, ndim, 1);
   
   const Float_t df = 1.0 / (xdif*ofac);
   Float_t pmax = -1.0f;
   
   Float_t* dwk1= wk1,   * dwk2= wk2,
          * kwk1= wk1+2, * kwk2= wk2+2,
          * pwk1=kwk1+1, * pwk2=kwk2+1;
   Float_t hypo, hc2wt, hs2wt, cwt, swt, cterm, sterm, den;
   for (j=0; j<nout; j++, dwk1++, dwk2++, kwk1+=2, kwk2+=2, pwk1+=2, pwk2+=2) {
      hypo  = TMath::Sqrt( ((*kwk2)*(*kwk2)) + ((*pwk2)*(*pwk2)) );
      hc2wt = 0.5*(*kwk2)/hypo;
      hs2wt = 0.5*(*pwk2)/hypo;
      cwt   = TMath::Sqrt(0.5+hc2wt);
      swt   = TMath::Sqrt(0.5-hc2wt);
      swt   = (hs2wt<0) ? -TMath::Abs(swt) : TMath::Abs(swt);
      den   = 0.5*n + hc2wt*(*kwk2) + hs2wt*(*pwk2);
      cterm = cwt*(*kwk1) + swt*(*pwk1);
      cterm = cterm*cterm / den;
      sterm = cwt*(*pwk1) - swt*(*kwk1);
      sterm = sterm*sterm / (n-den);
      *dwk1 = (j+1)*df;
      *dwk2 = (cterm+sterm)/twovar;
      if (*dwk2 > pmax) {
         jmax = j;
         pmax = *dwk2;
      }
   }
   const Float_t expy = TMath::Exp(-pmax);
   const Float_t effm = 2.0*nout/ofac;
   prob = effm*expy;
   if (prob > 0.01) {
      prob = 1.0 - TMath::Power(1.0-expy, effm);
   }
}

void spread(const Float_t y, Float_t* const yy, const UInt_t n,
            const Float_t x, const UInt_t m) {
   // extirpolate (spread) a value y into m actual array elements
   // that best approximate the "fictional" array element x given the
   // array yy.
   //
   // NUM REC C - 13.8, pg 583-584
   
   static const Float_t nfac[11] = { // factorials
      0,       1,   1,    2,     6,
      24,    120, 720, 5040, 40320,
      362880
   };
   
   if (m>10) {
      Fatal("spread","factorial table too small");
      return;
   }
   if (x<0) {
      Fatal("spread","x negative");
      return;
   }
   
   UInt_t ix = static_cast<UInt_t>(x);
   if (x == static_cast<Float_t>(ix)) yy[ix-1] += y;
   else {
      // note: for some reason, if these Int_t's are instead of type UInt_t,
      // the algorithm doesn't work -- even though ilo and ihi are always
      // non-negative!
      const Int_t ilo=TMath::Min(TMath::Max(static_cast<Int_t>(x-0.5*m+1.0),1),
                                 static_cast<Int_t>(n-m+1));
      const Int_t ihi=ilo+m-1;
      Float_t fac=x-ilo, fnden=nfac[m];
      
      register Int_t j;
      for(j=ilo+1;j<=ihi;j++) fac *= (x-j);
      yy[ihi-1] += y*fac/(fnden*(x-ihi));
/*
      for (j=ihi-1;j>=ilo;j--) {
         fnden=(fnden/(j+1-ilo))*(j-ihi);
         yy[j-1] += y*fac/(fnden*(x-j));
      }
*/
      j=ihi-1;
      Float_t* dyy = yy+j-1;
      for (; j>=ilo; j--, dyy--) {
         fnden=(fnden/(j+1-ilo))*(j-ihi);
         *dyy += y*fac/(fnden*(x-j));
      }
   }
}

template<typename Num_t>
void realft(Num_t* const data, const UInt_t n, Int_t isign) {
   // real FFT
   //
   // NUM REC C - 12.3, pg 513-514

   if ( ((n-1)&n)!=0 ) {
      Error("realft","n (=%u) must be a power of 2.",n);
      return;
   }
   
   Double_t theta = TMath::Pi() / static_cast<Double_t>(n>>1);
   Float_t c1=0.5, c2;
   if (isign==1) {
      c2 = -0.5;
      four1(data,n>>1,1); // forward FFT
   } else {
      c2 = 0.5;
      theta = -theta;     // set up for inverse tranform
   }
   
   Double_t wtemp     = TMath::Sin(0.5*theta);
   const Double_t wpr = -2.0*wtemp*wtemp;
   const Double_t wpi = TMath::Sin(theta);
   Double_t wr        = 1.0+wpr;
   Double_t wi        = wpi;
   Float_t h1r, h1i, h2r, h2i;
   const UInt_t n4  = n>>2; // n/4
   register UInt_t i;
   Num_t* d1 = data+2,   * d2 = d1+1,
        * d3 = data+n-2, * d4 = d3+1;
   for (i=1; i<n4; i++, d1+=2, d2+=2, d3-=2, d4-=2) {
      h1r   =  c1 * ((*d1)+(*d3));
      h1i   =  c1 * ((*d2)-(*d4));
      h2r   = -c2 * ((*d2)+(*d4));
      h2i   =  c2 * ((*d1)-(*d3));
      *d1   =  h1r + wr*h2r - wi*h2i;
      *d2   =  h1i + wr*h2i + wi*h2r;
      *d3   =  h1r - wr*h2r + wi*h2i;
      *d4   = -h1i + wr*h2i + wi*h2r;
      wtemp = wr;
      wr    = wtemp*wpr - wi*wpi    + wr;
      wi    = wi*wpr    + wtemp*wpi + wi;
   }
   
   // now fill in the beginning and end of the array
   if (isign==1) {
      h1r   = *data;
      d1    = data+1;
      *data = h1r + (*d1);
      *d1   = h1r - (*d1);
   } else {
      h1r   = *data;
      d1    = data+1;
      *data = c1*(h1r + (*d1));
      *d1   = c1*(h1r - (*d1));
      four1(data,n>>1,-1);       // inverse tranform
   }
}

template<typename Num_t>
void four1(Num_t* const data, const UInt_t nn, Int_t isign) {
   // DFT
   // data must be [0..2*nn-1]
   // nn must be a power of two
   //
   // NUM REC C - 12.2, pg 507-508
   
   if ( ((nn-1)&nn)!=0 ) {
      Error("four1","nn (=%u) must be a power of 2.",nn);
      return;
   }
   
   const UInt_t n = nn<<1;  // nn * 2
   Num_t tempr,tempi;
   UInt_t m;
   register UInt_t i,j=1;
   Num_t* di=data, * di1=data+1, * dj, * dj1;
   for (i=1; i<n; i+=2, di+=2, di1+=2) {
      if (j > i) {
         dj    = data + j - 1;
         dj1   = dj + 1;
         tempr = *dj;  *dj  = *di;  *di  = tempr;
         tempr = *dj1; *dj1 = *di1; *di1 = tempr;
      }
      m = nn;
      while (m>1 && j>m) {
         j -= m;
         m >>= 1;
      }
      j += m;
   }
   
   Double_t wtemp, wr, wpr, wpi, wi, theta;
   UInt_t mmax=2, istep;
   while (n > mmax) {
      istep = mmax<<1;
      theta = isign * (kTwoPi / mmax);
      wtemp = TMath::Sin(0.5*theta);
      wpr   = -2.0 * wtemp * wtemp;
      wpi   = TMath::Sin(theta);
      wr    = 1.0;
      wi    = 0.0;
      for (m=1; m<mmax; m+=2) {
         for (i=m; i<=n; i+=istep) {
            j     = i + mmax;
            dj    = data + j - 1;
            dj1   = dj+1;
            di    = data + i - 1;
            di1   = di+1;
            tempr = wr * (*dj)  - wi*(*dj1);
            tempi = wr * (*dj1) + wi*(*dj);
            *dj   = *di  - tempr;
            *dj1  = *di1 - tempi;
            *di  += tempr;
            *di1 += tempi;
         }
         wtemp = wr;
         wr = wtemp*wpr - wi*wpi    + wr;
         wi = wi*wpr    + wtemp*wpi + wi;
      }
      mmax = istep;
   }
}

template<typename Num_t>
void twofft(Num_t* data1, Num_t* data2,
            Num_t* fft1, Num_t* fft2,
            const UInt_t n) {
   // data1 and data2 are two real input arrays
   // call four1 and return two complex fft output arrays
   //
   // data must be [0..n-1]
   // fft  must be [0..2*n-1]
   // n must be a power of 2
   //
   // NUM REC C - 12.3, pg 511
   
   if ( ((n-1)&n)!=0 ) {
      Error("twofft","n (=%u) must be a power of 2.",n);
      return;
   }
   
   Num_t* d1j=data1, * d2j=data2,
        * f1e=fft1,  * f1o=fft1+1;
   for (UInt_t j=0; j<n; ++j, ++d1j, ++d2j, f1e+=2, f1o+=2) {
      *f1e = *d1j;
      *f1o = *d2j;
   }

   four1(fft1,n,1); // transform

   Float_t rep, rem, aip, aim;
   fft2[0] = fft1[1];
   fft1[1] = fft2[1] = 0.0;
   f1e = fft1+2;
   f1o = fft1+3;
   Num_t* f2e = fft2+2, * f2o = fft2+3;
   Num_t* vb1 = fft1+n+n-1;
   Num_t* bb1 = vb1-1;
   Num_t* vb2 = fft2+n+n-1;
   Num_t* bb2 = vb2-1;
   for (UInt_t j=2; j<=n; j+=2, f1e+=2, f1o+=2, f2e+=2, f2o+=2,
                          vb1-=2, vb2-=2, bb1-=2, bb2-=2) {
      rep = 0.5*((*f1e)+(*bb1));
      rem = 0.5*((*f1e)-(*bb1));
      aip = 0.5*((*f1o)+(*vb1));
      aim = 0.5*((*f1o)-(*vb1));
      *f1e =  rep;
      *f1o =  aim;
      *bb1 =  rep;
      *vb1 = -aim;
      *f2e =  aip;
      *f2o = -rem;
      *bb2 =  aip;
      *vb2 =  rem;
   }
}

template<typename Num_t>
void correl(Num_t* data1, Num_t* data2,
            const UInt_t n,
            Num_t* ans) {
   // compute correlation of data sets
   //
   // data must be [0..n-1]
   // ans must be [0..2*n-1]
   //
   // NUM REC C - 13.2, pg 546
   
   Num_t* fft = new Num_t[n<<1];
   twofft(data1, data2, fft, ans, n); // get both transforms
   const Num_t no2 = static_cast<Num_t>(n>>1);
   Num_t* ae = ans, * ao = ans+1;
   Num_t* fe = fft, * fo = fft+1;
   Num_t dum;
   for (UInt_t i=1; i<n+2; i+=2, ao+=2, ae+=2, fe+=2, fo+=2) {
      dum = *ae;
      *ae = (((*fe)*(dum)) + ((*fo)*(*ao)))/no2;
      *ao = (((*fo)*(dum)) - ((*fe)*(*ao)))/no2;
   }
   ans[1] = ans[n];
   realft(ans, n, -1);
   delete[] fft;
}

template<class Hist_t, typename Num_t>
Bool_t checkDataType(Hist_t* h, Num_t& brdata) {
   // check the bin type of the hist
   
   if (sizeof(Num_t)!=sizeof(h->At(0))) {
      Error("checkDataType","Size of data type in hist [%s] is [%d], while "
            "size of array type is [%d]",
            h->GetName(), sizeof(h->At(0)), sizeof(Num_t));
      return kFALSE;
   } else {
      return kTRUE;
   }
}

template<typename Num_t>
Bool_t checkDataType(TBranch* br, Num_t& brdata) {
   // check branch is ok and call SetAddress
   
   const TObjArray* leafs = br->GetListOfLeaves();
   if (leafs->GetEntries() > 1) {
      Error("checkDataType","Branch holds more than one leaf.");
      return kFALSE;
   }
   const TLeaf* lf = dynamic_cast<TLeaf*>(leafs->At(0));
   if (lf->GetLenType() != sizeof(Num_t)) {
      Error("checkDataType","Size of data type in branch [%s] is [%d], while "
            "size of array type is [%d]",
            br->GetName(), lf->GetLenType(), sizeof(Num_t));
      return kFALSE;
   }
   
   br->SetAddress(&brdata);
   return kTRUE;
}

template<typename Num_t>
void copyDataTo(TBranch* br, const Int_t entry, Num_t& source, Num_t& dest) {
   // GetEntry from br, into 'source'. copy to 'dest'.
   // DOES NOT increment entry
   br->GetEntry(entry);
   dest = source;
}

void copyDataTo(TH1F* h, const Int_t bin, Float_t& source, Float_t& dest) {
  // GetBinContent from h at 'bin' and copy to 'dest'.
  // WARNING: all boundry checks are circumvented for speed purposes!
  //
  // 'source' is not used at all; merely present to overload the
  // TBranch version.

  dest = h->GetArray()[bin];
}

void copyDataTo(TH1D* h, const Int_t bin, Double_t& source, Double_t& dest) {
  // GetBinContent from h at 'bin' and copy to 'dest'.
  // WARNING: all boundry checks are circumvented for speed purposes!
  //
  // 'source' is not used at all; merely present to overload the
  // TBranch version.

  dest = h->GetArray()[bin];
}

template<class DSource_t, typename Num_t>
void spectrum(DSource_t* br, Num_t* const p, const Int_t m,
              const Int_t k, const Bool_t overlap,
              const Int_t firstEntry,
              const EWinType_t window) {
   // from NUM REC C - 13.4, pg 557
   //
   // 'br' can be either a TBranch or a TH1[F,D,..]. for a histogram,
   // 'firstEntry' should be 1, since 0 is the underflow bin. note that
   // a pointer to the base class TH1 will NOT work (as it does not contain
   // functions for accessing the data arrays)
   //
   // data in branch should be of type float. its value is the
   // amplitude of the (evenly sampled) waveform in each bin
   //
   // sets p to the data's power at frequency i/(2*m) for i=0..m-1
   // power = mean square amplitude (eqn 13.4.5)
   //
   // overlap = true  ==> use (2*k + 1)*m data points
   // overlap = false ==> use 4*k*m data points
   //
   // window indicates how data points should be weighted
   
   
   if ( ((m-1)&m)!=0 ) {
      Error("spectrum","m (=%u) must be a power of 2.",m);
      return;
   }
   
   if (br==0) {
      Error("spectrum","Null hist/branch.");
      return;
   }
   
   Num_t brdata;
   const Bool_t dtok = checkDataType(br, brdata);
   if (dtok==kFALSE) {
      Error("spectrum","Data type mismatch.");
      return;
   }
   
   Int_t entry = firstEntry;
   if (firstEntry==0 && br->IsA()->InheritsFrom("TH1")) {
      Error("spectrum",
	    "firstEntry==0 requested for histogram (underflow bin)");
      return;
   }
   
   // choose the window function
   Float_t (*winfcn)(const Int_t, const Float_t, const Float_t) = 0;
   switch (window) {
      case kSquare:
         winfcn = &SquareWin;
         break;
      case kWelch:
         winfcn = &WelchWin;
         break;
      case kBartlett:
         winfcn = &BartlettWin;
         break;
      default:
         Error("spectrum",
               "Unknown window type [%d]",static_cast<Int_t>(window));
         return;
   };
   
   const Int_t mm  = m+m,
               m4  = mm+mm,
               m43 = m4+3;
   
   Num_t* const w1  = new Num_t[m4],
        * const w2  = new Num_t[m],
        *       dw1,
        *       dw2,
        *       dw3,
        *       dw4,
        *       dp;
   
   const Float_t facm = static_cast<Float_t>(m),
                 facp = 1.0 / facm;
   
   // sum weights sqd
   Num_t sumw=0, t, den=0;
   register Int_t j, kk, joff, joffn, j2;
   for (j=0; j<mm; j++) {
      t     = winfcn(j, facm, facp);
      sumw += t*t;
   }
   
   // initialize spectrum to 0
   memset(p, 0, m*sizeof(Num_t));
   
   if (overlap) {
      // initalize the "save" half-buffer
      dw2 = w2;
      for (j=0; j<m; j++, dw2++) {
	 copyDataTo(br, entry++, brdata, *dw2);
	 // br->GetEntry(entry++);
         // *dw2 = brdata;
      }
   }
   
   for (kk=0; kk<k; kk++) {
      for (joff=0; joff<2; joff++) {
         if (overlap) {
            
            dw2 = w2;
            for (j=0; j<m; j++, dw2++) {
               *(w1 + joff+j+j) = *dw2;
            }
            dw2 = w2;
            for (j=0; j<m; j++, dw2++) {
	       copyDataTo(br, entry++, brdata, *dw2);
               // br->GetEntry(entry++);
               // *dw2 = brdata;
            }
            joffn = joff+mm;
            dw2 = w2;
            for (j=0; j<m; j++, dw2++) {
               *(w1 + joffn+j+j) = *dw2;
            }
            
         } else {
            
            dw1 = w1+joff;
            for (j=joff; j<m4; j+=2, dw1+=2) {
	       copyDataTo(br, entry++, brdata, *dw1);
	       // br->GetEntry(entry++);
               // *dw1 = brdata;
            }
            
         }
      }
      // apply the window
      for (j=0; j<mm; j++) {
         j2        = j+j;
         t         = winfcn(j, facm, facp);
         dw1       = w1 + j2;
         *(dw1+1) *= t;
         *dw1     *= t;
      }
      
      four1(w1,mm,1); // FFT windowed data
      
      dw1 = w1+1;
      *p += ((*w1)*(*w1)) + ((*dw1)*(*dw1));
      
      dp = p+1;
      for (j=1; j<m; j++, dp++) {
         j2   = j+j;
         
         dw1  = w1+j2;
         dw2  = dw1+1;
         dw3  = w1+m43-j2;
         dw4  = dw3-1;
         
         *dp +=   ((*dw1)*(*dw1)) + ((*dw2)*(*dw2))
                + ((*dw3)*(*dw3)) + ((*dw4)*(*dw4));
      }
      den += sumw;
   }
   den *= m4;
   
   dp = p;
   for (j=0; j<m; j++, dp++) {
      *dp /= den;
   }
   
   delete[] w1;
   delete[] w2;
   
}

Float_t SquareWin(const Int_t i, const Float_t a, const Float_t b) {
   return 1.0;
}

Float_t WelchWin(const Int_t i, const Float_t a, const Float_t b) {
   const Float_t d = (i-1-a)*b;
   return (1.0 - (d*d));
}

Float_t BartlettWin(const Int_t i, const Float_t a, const Float_t b) {
   const Float_t d = (i-1-a)*b;
   return (1.0 - TMath::Abs(d));
}

