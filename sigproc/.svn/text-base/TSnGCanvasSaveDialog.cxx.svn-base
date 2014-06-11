#include "TSnGCanvasSaveDialog.h"

#include <TGFileDialog.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TString.h>
#include <TSystem.h>
#include <TGClient.h>


ClassImp(TSnGCanvasSaveDialog);

const char* TSnGCanvasSaveDialog::kSaveAsTypes[] =
   { "PostScript",   "*.ps",
     "Encapsulated PostScript", "*.eps",
     "PDF",          "*.pdf",
     "SVG",          "*.svg",
     "TeX",          "*.tex",
     "GIF",          "*.gif",
     "ROOT macros",  "*.C",
     "ROOT files",   "*.root",
     "XML",          "*.xml",
     "PNG",          "*.png",
     "XPM",          "*.xpm",
     "JPEG",         "*.jpg",
     "TIFF",         "*.tiff",
     "XCF",          "*.xcf",
     "All files",    "*",
     0,              0 };

TSnGCanvasSaveDialog::TSnGCanvasSaveDialog(const TRootEmbeddedCanvas* canv,
                                           const TGWindow* parent) :
   fDiag(0) {
   // basically a copy of the code in TRootCanvas...
   // too bad this doesn't seem to be provided by ROOT already
   
   if (canv!=0) {
      TCanvas* c = canv->GetCanvas();
      if (c!=0) {
         TString workdir = gSystem->WorkingDirectory();
         TString dir(".");
         TGFileInfo fi;
         fi.fFileTypes   = kSaveAsTypes;
         fi.fIniDir      = StrDup(dir);
         fi.fFileTypeIdx = 0;
         fi.fOverwrite   = kFALSE;
         // this ctor blocks until the user has selected a file
         fDiag = new TGFileDialog((parent!=0) ? parent
                                              : gClient->GetRoot(),
                                  canv, kFDSave, &fi);
         gSystem->ChangeDirectory(workdir.Data());
         // now we can get the filename from the file info
         if (fi.fFilename!=0) {
            TString fn(fi.fFilename);
            TString ft(fi.fFileTypes[fi.fFileTypeIdx+1]);
            Bool_t saved = TrySaveCanv(fn, *c);
            if (saved==kFALSE) {
               const Ssiz_t dot = ft.Index(".");
               if (dot!=kNPOS) {
                  // add on extension
                  fn += ft(dot, ft.Length());
                  saved = TrySaveCanv(fn, *c);
               }
            }
            if (saved==kFALSE) { // still
               Error("TSnGCanvasSaveDialog",
                     "File [%s] cannot be saved with this extension.",
                     fi.fFilename);
            }
         }
         
      }
   }
   
}


TSnGCanvasSaveDialog::~TSnGCanvasSaveDialog() {
   delete fDiag;
}

Bool_t TSnGCanvasSaveDialog::CanvExtensionKnown(const TString& fn) {
   if (fn.EndsWith(".root") ||
       fn.EndsWith(".ps")   ||
       fn.EndsWith(".eps")  ||
       fn.EndsWith(".pdf")  ||
       fn.EndsWith(".svg")  ||
       fn.EndsWith(".tex")  ||
       fn.EndsWith(".gif")  ||
       fn.EndsWith(".xml")  ||
       fn.EndsWith(".xpm")  ||
       fn.EndsWith(".jpg")  ||
       fn.EndsWith(".png")  ||
       fn.EndsWith(".xcf")  ||
       fn.EndsWith(".tiff") ||
       fn.EndsWith(".C")) {
      return kTRUE;
   }
   return kFALSE;
}

Bool_t TSnGCanvasSaveDialog::TrySaveCanv(const TString& fn, TCanvas& c) {
   if (CanvExtensionKnown(fn)) {
      if (fn.EndsWith(".C")) {
         c.SaveSource(fn);
      } else {
         c.SaveAs(fn);
      }
      return kTRUE;
   }
   return kFALSE;
}

