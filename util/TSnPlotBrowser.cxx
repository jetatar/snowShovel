#include "TSnPlotBrowser.h"

#include "TCanvas.h"
#include "TGFrame.h"
#include "TGCanvas.h"
#include "TObjString.h"
#include "TGButton.h"
#include "TMath.h"
#include "TError.h"
#include "TVirtualPad.h"
#include "TROOT.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(5,11,3)
#define R__ASSERT(e) \
   if (!(e)) Fatal("", kAssertMsg, _QUOTE_(e), __LINE__, __FILE__)
#endif

ClassImp(TSnPlotBrowser)
//////////////////////////////////////////////////////////////////////////
// $Id$
// Author : Corey Reed   09/22/2004
// 
// Allows for easily displaying and switching between several sets of
// plots. Each set of plots are displayed on a canvas together.
// A button bar is created, with one button for each set of plots.
//
// By default, the plot browser will calculate the optimal number of pads
// that the canvas should be divided into when displaying each set.
// However, the user may specify the desired number of rows and columns
// to use in the canvas for any set of plots.
//
// Users can specify drawing options for every plot in a set. This is done
// when the plot is added to the set.
//
// By default, plots will be drawn in the next available (empty) pad.
// However, the user may specify the desired pad to which each plot should
// be drawn. This is done when the plot is added to the set.
//
// IMPORTANT NOTE:
// The plots must not be deleted until after the plot browser has been
// destroyed. Doing so will cause the plot browser to try to use dangling
// pointers and will probably cause a nasty crash. This is especially
// important to consider when using histograms from a file -- if the
// histograms belong to this file (which they do by default), they will
// be deleted when the file is closed.
// 
//
// Usage:
// {
//    // We call "new" on the TFile to be sure it is not closed too soon,
//    // which would delete our histograms and cause the plot browser
//    // to crash.
//    TFile* mf = new TFile("myfile.root");
//    
//    TSnPlotBrowser* pb = new TSnPlotBrowser;
//    pb->AddPlotToSet("Momentum",mf->Get("px_hist"));
//    pb->AddPlotToSet("Momentum",mf->Get("py_hist"));
//    pb->AddPlotToSet("Momentum",mf->Get("pz_hist"));
//    pb->AddPlotToSet("Momentum",mf->Get("pt_hist")); 
//    // no log y for this 1D hist, but turn on the grid
//    pb->AddPlotToSet("Momentum",mf->Get("rapidity_hist"),0,0,
//                     TSnPlotBrowser::kDefaults
//                     | TSnPlotBrowser::kGridx
//                     | TSnPlotBrowser::kGridy);
//    pb->AddPlotToSet("Energy",mf->Get("dEdx_graph"));
//    pb->AddPlotToSet("Energy",mf->Get("dEdxVsP_hist"),"colz");
//    pb->AddPlotToSet("Energy",mf->Get("E_hist"),"",4);
//    pb->AddPlotToSet("Energy",mf->Get("Et_hist"),"",3);
//    
//    pb->DrawButtonBar();
// }
//////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------
// nested class TArPlotSet functions
TSnPlotBrowser::TArPlotSet::TArPlotSet() :
   fXDivs(0),
   fYDivs(0) {
   // default constructor
   fPlots.SetOwner(kFALSE);
   fDrawOpts.SetOwner(0);
}

TSnPlotBrowser::TArPlotSet::TArPlotSet(const Char_t* name) :
   TNamed(name,0),
   fXDivs(0),
   fYDivs(0) {
   // normal constructor
   fPlots.SetOwner(kFALSE);
   fDrawOpts.SetOwner(0);
   SetName(ReplaceSpaces(name));
}

TString TSnPlotBrowser::TArPlotSet::ReplaceSpaces(const Char_t* s) {
   // for some reason, TQObject now strips spaces from the slot
   // so replace them with underscores
   TString str(s);
   str.ReplaceAll(" ","_");
   return str;
}

void TSnPlotBrowser::TArPlotSet::Add(TObject* plot,
                                     const Char_t* drawOpt,
                                     Int_t pad,
                                     const Int_t padOpt) {
   // add the plot to this set, to be drawn in the specified pad
   // if a plot is already being drawn to the pad, add the draw option "same"
   // (if "same" is not already specified)
   // if 'pad' is less than 1, the plot will be drawn to the next available
   // pad number
   
   TString dopt(drawOpt);
   const Int_t idx = fPads.GetSize();
   if (pad < 1) {
      if (idx==0) {
         pad = 1;
      } else {
         for (Int_t i=0; i<idx; i++) {
            if (fPads[i] >= pad) pad = fPads[i] + 1;
         }
      }
   } else {
      Bool_t padUsed=kFALSE;
      for (Int_t i=0; i<idx; i++) {
         if (fPads[i] == pad) {
            padUsed = kTRUE;
            break;
         }
      }
      if (padUsed) {
         if ( dopt.Contains("same") == kFALSE ) {
            dopt.Append("same");
         }
      }
   }
   fPlots.AddAtAndExpand(plot,idx);
   fDrawOpts.AddAtAndExpand(new TObjString(dopt.Data()),idx);
   fPads.Set(idx+1);
   fPads[idx] = pad;
   if (pad >= fPadOpts.GetSize()) {
      fPadOpts.Set(pad+1);
   }
   fPadOpts[pad] = (fPadOpts[pad]==0) ? padOpt : fPadOpts[pad] | padOpt;
}

void TSnPlotBrowser::TArPlotSet::DrawPlots(TCanvas& c) {
   c.Clear();
   UInt_t cols=0, rows=0;
   CalcCanvDivs(cols, rows);
   c.Divide(cols, rows);
   const Int_t nplots = fPads.GetSize();
   TObject* obj=0;
   TObjString* str=0;
   TVirtualPad* vp=0;
   for (Int_t i=0; i<nplots; i++) {
      obj = fPlots.At(i);
      if (obj!=0) {
         vp = c.GetPad(fPads.At(i));
         if (vp!=0) {
            vp->cd();
            SetPadOpts(vp, fPadOpts.At(fPads.At(i)), obj);
            str = dynamic_cast<TObjString*>(fDrawOpts.At(i));
            fPlots.At(i)->Draw( (str!=0) ? str->GetString().Data() : "" );
         } else {
            Error("DrawPlots","Desired pad number %d could not be found.",
                  fPads.At(i));
         }
      }
   }
   c.cd();
   c.Update();
}

void TSnPlotBrowser::TArPlotSet::SetPadOpts(TVirtualPad* vp,
                                            Int_t opt,
                                            TObject* obj) const {
   R__ASSERT(vp!=0);
   if ( (opt & kAutoOpt) > 0 ) {
      if (obj->InheritsFrom("TH3")) {
         opt = kDefaults;
      } else if (obj->InheritsFrom("TH2")) {
         opt = k2DDefaults;
      } else if (obj->InheritsFrom("TH1")) {
         opt = k1DDefaults;
      } else {
         opt = kDefaults;
      }
   }
   vp->SetLogx(            (opt & kLogx)              > 0);
   vp->SetLogy(            (opt & kLogy)              > 0);
   vp->SetLogz(            (opt & kLogz)              > 0);
   vp->SetGridx(           (opt & kGridx)             > 0);
   vp->SetGridy(           (opt & kGridy)             > 0);
   vp->SetCrosshair(       (opt & kCrosshair)         > 0);
   vp->SetFixedAspectRatio((opt & kFixedAspectRatio)  > 0);
   vp->SetTickx(           (opt & kTickx)             > 0);
   vp->SetTicky(           (opt & kTicky)             > 0);
   
}

void TSnPlotBrowser::TArPlotSet::CalcCanvDivs(UInt_t& cols,
                                              UInt_t& rows) const {
   TSnPlotBrowser::CalcCanvDivs(cols, rows, GetMaxPad(),
                                fXDivs, fYDivs);
}

Int_t TSnPlotBrowser::TArPlotSet::GetMaxPad() const {
   const Int_t size = fPads.GetSize();
   Int_t pp=0;
   for (Int_t i=0; i<size; i++) {
      if (fPads[i] > pp) pp = fPads[i];
   }
   return pp;
}
//----------------------------------------------------------------------


TSnPlotBrowser::TSnPlotBrowser(const Char_t* name/*  = "Plot Browser" */,
                               const UInt_t wid/* =180 */,
                               const UInt_t hit/* =600 */) :
   fCanv(0),
   fBtnWnd(0),
   fBtnFrame(0),
   fBtnBar(0),
   fWndHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY),
   fBtnHints(kLHintsTop | kLHintsExpandX),
   fPBName(name),
   fBtnWidth(wid),
   fBtnHeight(hit) {
   // default constructor
   fPlotSets.SetOwner(kTRUE);
}

TSnPlotBrowser::~TSnPlotBrowser() {
   // destructor
   // don't delete fCanv. let the user take care of it
   fBtnWnd->CloseWindow();
}

void TSnPlotBrowser::AddNewPlotSet(const Char_t* setName,
                                   const TObjArray& plots,
                                   const TObjArray& drawOpts,
                                   const TArrayI& pads,
                                   const TArrayI& padOpts,
                                   UInt_t cols/* =0 */,
                                   UInt_t rows/* =0 */) {
   // add a new set of plots using the specified set name
   // plots    - list of plots in the set
   // drawOpts - draw options for each plot
   // pads     - the pad each plot should be drawn to
   // padOpts  - options for each pad. indexed by pad number
   // cols     - num of columns in the canvas when drawing this set
   // rows     - num of rows in the canvas when drawing this set
   //
   // if xdivs or ydivs are 0, the rows/columns will be calculated automatically
   // upon drawing
   //
   // number of plots is assumed to be the same as the size of the 'pads' array
   //
   // plots, drawOpts and pads must all be indexed the same. even if they 
   // contain gaps.
   
   TArPlotSet* set = new TArPlotSet(setName);
   set->fXDivs    = cols;
   set->fYDivs    = rows;
   const Int_t nplots = pads.GetSize();
   TObjString* str=0;
   for (Int_t i=0; i<nplots; i++) {
      str = dynamic_cast<TObjString*>(drawOpts.At(i));
      set->Add(plots.At(i),
               (str!=0) ? str->GetString().Data() : 0,
               pads.At(i),
               padOpts.At(pads.At(i)));
   }
   fPlotSets.Add(set);
   if ( (fBtnWnd!=0) ) AddButton(set);
}

void TSnPlotBrowser::AddNewPlotSet(const Char_t* setName,
                                   TObject* plot,
                                   const Char_t* drawOpt/* =0 */,
                                   Int_t  padOpt/* =kAutoOpt */) {
   // add a new set of plots using the specified set name
   // plot     - the first plot in the set
   // drawOpt  - draw options for this plot
   
   TArPlotSet* set = new TArPlotSet(setName);
   set->Add(plot, drawOpt, 1, padOpt);
   fPlotSets.Add(set);
   if ( (fBtnWnd!=0) ) AddButton(set);
}

void TSnPlotBrowser::AddPlotToSet(const Char_t* setName,
                                  TObject* plot,
                                  const Char_t* drawOpt/* =0 */,
                                  const Int_t pad/* =0 */,
                                  Int_t padOpt/* =kAutoOpt */) {
   // add the plot to the specified set, to be drawn in the specified pad
   // if a plot is already being drawn to the pad, add the draw option "same"
   // (if "same" is not already specified)
   // if 'pad' is less than 1, the plot will be drawn to the next available
   // pad number
   //
   // if the specified set is not found, will create a new set.
   // if a new set is created, the pad number is ignored.
   
   TArPlotSet* set = dynamic_cast<TArPlotSet*>(
      fPlotSets.FindObject(TArPlotSet::ReplaceSpaces(setName)));
   if (set!=0) {
      set->Add(plot, drawOpt, pad, padOpt);
   } else {
      AddNewPlotSet(setName, plot, drawOpt, padOpt);
   }
}

void TSnPlotBrowser::DivideCanvForSet(const Char_t* setName,
                                      const UInt_t cols,
                                      const UInt_t rows) {
   // set the number of rows and columns the canvas should be divided into
   // for the specified set
   
   TArPlotSet* set = dynamic_cast<TArPlotSet*>(
      fPlotSets.FindObject(TArPlotSet::ReplaceSpaces(setName)));
   if (set!=0) {
      set->fXDivs = cols;
      set->fYDivs = rows;
   } else {
      Error("DivideCanvForSet","No plot set named [%s] found.", setName);
   }
}

TObject* TSnPlotBrowser::FindPlotInSet(const Char_t* setName,
                                       const Char_t* plotName) const {
   // find the specified plot in the specified set
   
   TArPlotSet* set = dynamic_cast<TArPlotSet*>(
      fPlotSets.FindObject(TArPlotSet::ReplaceSpaces(setName)));
   if (set!=0) {
      return set->fPlots.FindObject(plotName);
   } else {
      Error("FindPlotInSet","No plot set named [%s] found.", setName);
      return 0;
   }
}

void TSnPlotBrowser::DrawPlots(const Char_t* setName) {
   // draw the specified plot set
   
   TArPlotSet* set = dynamic_cast<TArPlotSet*>(
      fPlotSets.FindObject(TArPlotSet::ReplaceSpaces(setName)));
   if (set!=0) {
      if ( (fCanv==0) ||
           (gROOT->GetListOfCanvases()->FindObject(fCanv)==0) ) {
         fCanv = new TCanvas;
      }
      set->DrawPlots(*fCanv);
   } else {
      Error("DrawPlots","No plot set named [%s] found.", setName);
   }
}

void TSnPlotBrowser::AddButton(TArPlotSet* set) {
   // add a button for the specified set and redraw the button bar

   if (fBtnFrame!=0) {
      if (fBtnBar!=0) {
         TGTextButton* btn = new TGTextButton(fBtnFrame,
                                              Form("%s",set->GetName()));
         TString slot=Form("DrawPlots(=\"%s\")",set->GetName());
         btn->Connect("Clicked()",
                      "TSnPlotBrowser", this,
                      slot.Data());
         fBtnBar->AddFrame(btn, &fBtnHints);

         DrawButtonBar();
      } else {
         Error("AddButton",
               "No button bar to add a button to. "
               "Serious failure: button frame exists, button bar does not.");
      }
   } else {
      Error("AddButton",
            "No button frame to add a button to. "
            "MakeButtonBar() must be called first.");
   }
}

void TSnPlotBrowser::MakeButtonBar() {
   // make the button bar if the list of plot sets is not empty
   // makes a vertical button bar
   
   if (fPlotSets.IsEmpty()==kFALSE) {
      delete fBtnWnd;
      delete fBtnBar;
      delete fBtnFrame;
      fBtnWnd = new TGMainFrame(gClient->GetRoot(),fBtnWidth,fBtnHeight);
      fBtnWnd->SetWindowName(fPBName.Data());
      fBtnBar = new TGCanvas(fBtnWnd,fBtnWidth,fBtnHeight);
      fBtnFrame = new TGVerticalFrame(fBtnWnd,fBtnWidth,fBtnHeight);
      fBtnBar->SetContainer(fBtnFrame);

      TIter nextPlotSet(fPlotSets.MakeIterator());
      TArPlotSet* set=0;
      while ( (set = dynamic_cast<TArPlotSet*>(nextPlotSet())) ) {
         AddButton(set);
      }
 
      fBtnWnd->AddFrame(fBtnBar, &fWndHints);
      
   } else {
      Warning("MakeButtonBar",
              "No plot sets exist yet. Can not make button bar.");
   }
}

void TSnPlotBrowser::DrawButtonBar() {
   // draw the button bar
   // makes the button bar if necessary
   
   if (fBtnWnd==0) MakeButtonBar();
   if (fBtnWnd!=0) {
      fBtnWnd->MapSubwindows();
      fBtnWnd->Layout();
      fBtnWnd->MapWindow();
   } else {
      Error("DrawButtonBar",
            "Button bar does not exist and could not be created. Draw failed.");
   }
}

void TSnPlotBrowser::CalcCanvDivs(UInt_t& cols,
                                  UInt_t& rows,
                                  const UInt_t nplots,
                                  const UInt_t xdivs,
                                  const UInt_t ydivs) {
   // calculate a reasonable number of columns and rows that can be passed
   // to canvas->Divide for the specified number of plots.
   //
   // a desired number of columns and/or rows may be specified by setting
   // non-zero values for xdivs and/or ydivs, respectively
   
   if ( (xdivs==0) && (ydivs==0) ) {
      if (nplots > 0) {
         const UInt_t sqrt = static_cast<UInt_t>(TMath::Sqrt(nplots)),
                      a    = sqrt + ( (nplots % sqrt) > 0 ? 1 : 0 ),
                      b    = nplots / sqrt;
         cols = TMath::Max(a,b);
         rows = TMath::Min(a,b);
      }
   } else if ( (xdivs!=0) && (ydivs==0) ) {
      // columns specified. use minimum number of rows.
      cols = xdivs;
      rows = (nplots / xdivs) + ( (nplots % xdivs) > 0 ? 1 : 0 );
   } else if ( (xdivs==0) && (ydivs!=0) ) {
      // rows specified. use minimum number of columns.
      cols = (nplots / ydivs) + ( (nplots % ydivs) > 0 ? 1 : 0 );
      rows = ydivs;
   } else {
      cols = xdivs;
      rows = ydivs;
   }
}
