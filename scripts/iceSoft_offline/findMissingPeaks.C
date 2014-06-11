
static const Int_t nchans=4;
static const Int_t nsamps=128;

TChain* nt;
TFile* inf;
TH2* h2[nchans];
TH1* hp;
TCanvas* c1, * c2;

Int_t  col[nchans] = {kOrange+7, kAzure-6, kViolet-1, kGreen+2};

void findMissingPeaks(const Char_t* infn=
   "/home/cjreed/work/temp/nt.temp.ev_*.dat.gz.root",
                      const Bool_t waitForClick=kTRUE) {
   
   gStyle->SetOptStat(0);
   
   nt = new TChain("nt");
   nt->Add(infn);
   
   const Long64_t nents = nt->GetEntries();
   Printf("nents=%lld",nents);


   UShort_t samples[nchans][128];
   
   nt->SetBranchStatus("*",0);
   TString brnm;
   
   for (Int_t i=0; i<nchans; i++) {
      brnm = Form("data%02d",i);
      nt->SetBranchStatus(brnm.Data(),1);
      nt->SetBranchAddress(brnm.Data(),&(samples[i][0]));
      brnm = Form("h2_%02d",i);
      h2[i] = new TH2F(brnm.Data(),
         Form("ch%02d sample vs event;event;sample;ADC",i),
         nents, -0.5, nents-0.5,
         nsamps, -0.5, nsamps-0.5);
      h2[i]->SetLineColor(col[i]);
   }
   
   
   
   Float_t a[nchans]={0}, r[nchans]={0}, n[nchans]={0};
   Long64_t ne=0;
   UShort_t smp=0;
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      ne++;
      for (Int_t ch=0; ch<nchans; ch++) {
         for (Int_t i=0; i<128; i++) {
            smp = samples[ch][i];
            h2[ch]->Fill(e, i, smp);
            a[ch] += smp;
            r[ch] += smp * smp;
            n[ch] += 1.0;
         }
      }
   }

   for (Int_t ch=0; ch<nchans; ch++) {
      a[ch] /= (n[ch]-1.0);
      r[ch] /= (n[ch]-1.0);
      r[ch] -= a[ch]*a[ch];
      r[ch]  = TMath::Sqrt(r[ch]);

      Printf("ch %d: ave noise = %g, rms = %g", ch, a[ch], r[ch]);
   }
   
   Printf("Processed %lld evts",ne);
   
   c1 = new TCanvas("c1","c1",800,700);
   c1->Divide(2,2);

   c2 = new TCanvas("c2","c2",800,700);
   
   Int_t nhigh=0;
   for (Int_t xb=nents; xb>0; xb--) {
      Bool_t miss=kFALSE;
      for (Int_t ch=0; ch<nchans; ch++) {
         if (h2[ch]->Integral()>0) {

            Float_t sum=0, high=0, c=0;
            for (Int_t yb=nsamps; yb>0; yb--) {
               c = h2[ch]->GetCellContent(xb, yb);
               sum += c;
               if (c > (2.0*r[ch])+a[ch]) {
                  high += c;
               }
            }
            c1->cd(ch+1);
            h2[ch]->Draw("colz");

            if (high > a[ch]) {
               nhigh++;
            } else {
               Printf("no peak in ch %d, evt %d",ch,xb);
               miss = kTRUE;
            }
            
         }
      }
      TString dopt;
      if (miss) {
         for (Int_t ch=0; ch<nchans; ch++) {
            c2->cd();
            hp = h2[ch]->ProjectionY("_py",xb,xb);
            if (hp->Integral()>0) {
               hp->Draw(dopt);
               dopt = "same";
            }
         }
         c2->Update();
         if (waitForClick) {
            c2->WaitPrimitive();
         }
      }
   }

   Printf("nhigh = %d", nhigh);
}
