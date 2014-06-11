#ifndef SNS_TSnEvtGuiMainFrame
#define SNS_TSnEvtGuiMainFrame

#include <TGFrame.h>
#include <RQ_OBJECT.h>
class TTree;
class TFile;

class TSnEvtGuiCanvFrame;
class TSnEvtGuiSeleFrame;
class TSnEvtGuiDataGuru;

class TSnEvtGuiMainFrame : public TGMainFrame {
   RQ_OBJECT("TSnEvtGuiMainFrame");
   
 private:
   TSnEvtGuiCanvFrame* fCanvFrame; // composite frame containing the canvases
   TSnEvtGuiSeleFrame* fSeleFrame; // composite frame containing the selections
   TSnEvtGuiDataGuru*  fDataGuru;  // manage the data
   
 public:
   TSnEvtGuiMainFrame() : fCanvFrame(0), fSeleFrame(0), fDataGuru(0) {}
   TSnEvtGuiMainFrame(TTree* tree,
                      TFile* pfile,
                      const UInt_t w=800,
                      const UInt_t h=600);
   virtual ~TSnEvtGuiMainFrame();
   
   void CloseWindow();
   void ChangeWaveChans(Int_t btn);
   void RedrawWaveCanv();
   void ChangeWaveType(Int_t id);
   void ChangeEvtNum(Long_t);
   void ChangeEntryNum(Long_t);
   void IncrementEvent(Int_t id);
   void ChangeTab(Int_t id);
   void SelMiscPlot(const Char_t* nm);

   ClassDef(TSnEvtGuiMainFrame, 1); // main event gui window
};

#endif // SNS_TSnEvtGuiMainFrame
