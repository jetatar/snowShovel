
TFile* inf, * outf;

void extractIdeal(const Char_t* wavefile,
                  const Char_t* outfn,
                  const Int_t evnum,
                  const Int_t nchans=4) {
   inf = TFile::Open(wavefile);
   TString hn;
   TH1* h;
   if ( (inf!=0) && (inf->IsZombie()==kFALSE) ) {
      outf = TFile::Open(outfn, "recreate");
      for (Int_t ch=0; ch<nchans; ch++) {
         hn = Form("ShiftedPSWvfms/hShiftedPSWvfms_ch%02d_ev%06d",ch,evnum);
         h  = dynamic_cast<TH1*>(inf->Get(hn.Data()));
         if (h==0) {
            Error("extractIdeal","Could not get histogram named [%s] "
                  "from file [%s].",
                  hn.Data(),
                  wavefile);
            return;
         } else {
            outf->cd();
            hn = Form("hIdealWvfm_ch%02d",ch);
            h->SetName(hn.Data());
            h->Write();
         }
      }
      delete outf;
      outf=0;
   }
}
