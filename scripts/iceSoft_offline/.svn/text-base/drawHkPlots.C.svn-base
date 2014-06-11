
TFile* inf;
TGraph* gWind, * gSol, * gBatBox, * gInstrBox,
      * gBatVolt, * gTemp1, * gTemp2, * gAnem;
TCanvas* c1, * c2;

Bool_t gShwTimes=kTRUE;

void getGraph(TGraph*& gr, const Char_t* gname) {
   if (inf!=0) {
      gr = dynamic_cast<TGraph*>(inf->Get(gname));
   }
}

void drawGraph(TGraph* gr, const Char_t* ytitle=0) {
   gr->Draw("ap");
   TH1* h = gr->GetHistogram();
   if (h!=0) {
      if (gShwTimes) {
         h->GetXaxis()->SetTimeFormat("%m/%d %H:%M %F1970-01-01 00:00:00");
         h->GetXaxis()->SetNdivisions(505);
      } else {
         h->GetXaxis()->SetTimeFormat("%m/%d %F1970-01-01 00:00:00");
      }
      h->GetXaxis()->SetTimeDisplay(1);
      h->GetXaxis()->SetTitle("Date, Time (UTC)");
      if (ytitle!=0) {
         h->GetYaxis()->SetTitle(ytitle);
      }
   }
   
   if (gPad!=0) {
      gPad->Update();
   }
   
}

void drawHkPlots(const Char_t* infn, const Bool_t shwTimes=kTRUE,
                 const Int_t cwid=1400, const Int_t chit=800) {
   
   gSystem->Setenv("TZ","UTC");
   
   inf = TFile::Open(infn);
   if ((inf==0) || (inf->IsZombie())) {
      Printf("Could not open [%s]",infn);
      return;
   }
   
   gShwTimes = shwTimes;
   
   getGraph(gWind,     "gWind");
   getGraph(gSol,      "gSol");
   getGraph(gBatBox,   "gBatBox");
   getGraph(gInstrBox, "gInstrBox");
   getGraph(gBatVolt,  "gBatVolt");
   getGraph(gTemp1,    "gTemp1");
   getGraph(gTemp2,    "gTemp2");
   getGraph(gAnem,     "gAnem");
   
   c1 = new TCanvas("c1","c1 - currents",cwid,chit);
   c1->Divide(2,2);
   c1->cd(1);
   drawGraph(gWind, "Current 4 - wind (A)");
   c1->cd(2);
   drawGraph(gSol,  "Current 3 - solar (A)");
   c1->cd(3);
   drawGraph(gBatBox, "Current 2 - battery box (A)");
   c1->cd(4);
   drawGraph(gInstrBox, "Current 1 - instr. box (A)");
   c1->cd();
   c1->Update();
   
   c2 = new TCanvas("c2","c2 - others",cwid,chit);
   c2->Divide(2,2);
   c2->cd(1);
   drawGraph(gBatVolt, "Battery Voltage (V)");
   c2->cd(2);
   drawGraph(gTemp1, "Temp 1 - inside box (C)");
   c2->cd(3);
   drawGraph(gAnem, "Wind speed (knotts)");
   c2->cd(4);
   drawGraph(gTemp2, "Temp 2 - solar panel (C)");
   c2->cd();
   c2->Update();
   
   
}
