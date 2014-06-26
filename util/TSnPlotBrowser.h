#ifndef SNS_TSnPlotBrowser
#define SNS_TSnPlotBrowser

 
//////////////////////////////////////////////////////////////////////////
// $Id$
// Author : Corey Reed   09/22/2004
// 
// TSnPlotBrowser
//
// Allows for easily displaying and switching between several sets of
// plots. See implementation file for more info.
//////////////////////////////////////////////////////////////////////////

// root includes
#ifndef ROOT_TQObject
#include "TQObject.h"
#endif
#ifndef ROOT_TNamed
#include "TNamed.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif
#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif
#ifndef ROOT_TArrayI
#include "TArrayI.h"
#endif
#ifndef ROOT_THashList
#include "THashList.h"
#endif
#ifndef ROOT_TGLayout
#include "TGLayout.h"
#endif
#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif
#ifndef ROOT_RQ_OBJECT
#include "RQ_OBJECT.h"
#endif
#include <TVirtualPad.h>

class TCanvas;
class TGMainFrame;
class TGVerticalFrame;
class TGCanvas;

class TSnPlotBrowser : public TQObject {

RQ_OBJECT("TSnPlotBrowser")

public:
   enum EPadOpts {
      kLogx             = BIT(0),   // SetLogx
      kLogy             = BIT(1),   // SetLogy
      kLogz             = BIT(2),   // SetLogz  (def: on)
      kGridx            = BIT(3),   // SetGridx
      kGridy            = BIT(4),   // SetGridy
      kCrosshair        = BIT(5),   // SetCrosshair
      kFixedAspectRatio = BIT(6),   // SetFixedAspectRatio
      kTickx            = BIT(7),   // SetTickx (def: on)
      kTicky            = BIT(8),   // SetTicky (def: on)
      kAutoOpt          = BIT(9),   // use defaults based on type of plot
      kDefaults         = kTickx | kTicky,
      k1DDefaults       = kLogy | kDefaults,
      k2DDefaults       = kLogz | kDefaults
   };
   
private:
   struct TArPlotSet : public TNamed {
      UInt_t         fXDivs;     //  num x divisions in canvas (columns)
      UInt_t         fYDivs;     //  num y divisions in canvas (rows)
      TObjArray      fPlots;     //  list of plots
      TObjArray      fDrawOpts;  //  list of draw options, corresponding to fPlots
      TArrayI        fPads;      //  list of pad numbers, corresponding to fPlots
      TArrayI        fPadOpts;   //  list of options for each pad
      
      TArPlotSet();
      TArPlotSet(const Char_t* name);
      virtual ~TArPlotSet() {}
      
      void     Add(TObject* plot, const Char_t* drawOpt, Int_t pad,
                   const Int_t padOpt);
      void     DrawPlots(TCanvas& c);
      void     CalcCanvDivs(UInt_t& cols, UInt_t& rows) const;
      Int_t    GetMaxPad() const;
      void     SetPadOpts(TVirtualPad* vp, Int_t opt, TObject* obj) const;

      static TString ReplaceSpaces(const Char_t* s);
   };
   
   TCanvas*          fCanv;      //! canvas plots are drawn to
   TGMainFrame*      fBtnWnd;    //! button bar window
   TGVerticalFrame*  fBtnFrame;  //! button bar frame
   TGCanvas*         fBtnBar;    //! button bar canvas
   TGLayoutHints     fWndHints;  //  button window layout hints
   TGLayoutHints     fBtnHints;  //  button layout hints
   TString           fPBName;    //  name of this plot browser (displayed as title of button window)
   UInt_t            fBtnWidth;  //  button window width (default: 180)
   UInt_t            fBtnHeight; //  button window height (default: 600)
   THashList         fPlotSets;  //  collection of sets of plots


   void        AddButton(TArPlotSet* set);
   void        MakeButtonBar();
   
public:
   TSnPlotBrowser(const Char_t* name = "Plot Browser",
                  const UInt_t wid=180, const UInt_t hit=600);
   virtual ~TSnPlotBrowser();
   
   TCanvas*    GetCanvas() const        { return fCanv; }
   void        SetCanvas(TCanvas* canv) { fCanv = canv; }
   
   void        AddNewPlotSet(const Char_t* setName,
                             const TObjArray& plots,
                             const TObjArray& drawOpts,
                             const TArrayI& pads,
                             const TArrayI& padOpts,
                             UInt_t cols=0,
                             UInt_t rows=0);
   void        AddNewPlotSet(const Char_t* setName,
                             TObject* plot,
                             const Char_t* drawOpt=0,
                             const Int_t  padOpt=kAutoOpt);
   
   void        AddPlotToSet(const Char_t* setName,
                            TObject* plot,
                            const Char_t* drawOpt=0,
                            const Int_t pad=0,
                            const Int_t padOpt=kAutoOpt);
                            
   void        DivideCanvForSet(const Char_t* setName,
                                const UInt_t cols,
                                const UInt_t rows);

   TObject*    FindPlotInSet(const Char_t* setName,
                             const Char_t* plotName) const;                                
   void        DrawPlots(const Char_t* setName);
   void        DrawButtonBar();

   static
   void        CalcCanvDivs(UInt_t& cols,
                            UInt_t& rows,
                            const UInt_t nplots,
                            const UInt_t xdivs=0,
                            const UInt_t ydivs=0);
   
   static
   void        CalcCanvDivsP(UInt_t* cols,
                             UInt_t* rows,
                             const UInt_t nplots,
                             const UInt_t xdivs=0,
                             const UInt_t ydivs=0) {
      // does nothing if either cols or rows is a null pointer
      //
      // this function is callable in pyroot using array.array('I',[0])
      // for cols and rows
      if ((cols!=0) && (rows!=0)) {
         CalcCanvDivs(*cols, *rows, nplots, xdivs, ydivs);
      }
   }
   
   
   ClassDef(TSnPlotBrowser,1) // class for easy browsing of sets of plots
};

#endif //SNS_TSnPlotBrowser
