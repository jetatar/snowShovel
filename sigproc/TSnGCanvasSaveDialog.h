#ifndef SNS_TSnGCanvasSaveDialog
#define SNS_TSnGCanvasSaveDialog

#include <TObject.h>
#include <RQ_OBJECT.h>
class TGFileDialog;
class TCanvas;
class TString;
class TRootEmbeddedCanvas;
class TGWindow;

class TSnGCanvasSaveDialog : public TObject {
   RQ_OBJECT("TSnGCanvasSaveDialog");

 public:
   static const char* kSaveAsTypes[];
   
 private:
   TGFileDialog* fDiag; //! (transient) the file dialog box

   static
   Bool_t CanvExtensionKnown(const TString& fn);
   static
   Bool_t TrySaveCanv(const TString& fn, TCanvas& c);
   
 public:
   TSnGCanvasSaveDialog(const TRootEmbeddedCanvas* canv = 0,
                        const TGWindow* parent = 0);
   virtual ~TSnGCanvasSaveDialog();
   
   ClassDef(TSnGCanvasSaveDialog, 1); // a dialog box for saving TGCanvases
};

#endif // SNS_TSnGCanvasSaveDialog
