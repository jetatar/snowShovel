#ifndef SNS_TSnEvtGuiDataGuru
#define SNS_TSnEvtGuiDataGuru

#include <TObject.h>
#include <TString.h>
class TTree;
class TFile;
class TCanvas;
class TGButtonGroup;
class TH1;
class TH2;
class THashTable;
class TGraph;
class TObjArray;
class TGComboBox;

class TSnEvtGuiSeleFrame;
class TSnEventHeader;
class TSnEventMetadata;

#include <vector>

class TSnEvtGuiDataGuru : public TObject {
 private:
   TTree*             fTree;        // tree with event header info (not owned)
   TFile*             fPlotf;       // file with plots from TSnPlotWaveformsMod (not owned)
   
   std::vector<bool>  fWaveChansOn; //! which channels to draw
   std::vector<Int_t> fChanColors;  //! channel colors
   Int_t              fEvt;         //! current event
   TString            fWave;        //! current plot waveform tag
   std::vector<TH1*>  fWaveHists;   //! hists in wave canvas (so they can deleted)
   THashTable*        fWaveSources; //! 2d hists used for projecting
   THashTable*        fMiscPlots;   //! plots that go on the misc canvas
   TGraph*            fEntryVsEvt;  //! graph mapping event to entry
   Double_t           fMinEvt;      //! min event num
   Double_t           fMaxEvt;      //! max event num
   Double_t           fMinEntry;    //! min entry
   Double_t           fMaxEntry;    //! max entry

   TSnEventHeader*    fEvtHdr;      //! event header
   TSnEventMetadata*  fEvtMeta;     //! event metadata
   
   TH2*     Load2DHist(const Char_t* name);
   TObject* LoadMiscPlot(const Char_t* name);

 public:
   TSnEvtGuiDataGuru() : fTree(0), fPlotf(0), fEvt(0), fWaveSources(0) {}
   TSnEvtGuiDataGuru(TTree* tree, TFile* plotf);
   virtual ~TSnEvtGuiDataGuru();

   TTree*  GetTree() { return fTree; }
   TFile*  GetPlotFile() { return fPlotf; }
   
   Int_t   GetColorOfChan(const Int_t ch) { return fChanColors.at(ch); }
   const std::vector<Int_t>& GetChanColors() const { return fChanColors; }

   Double_t GetMinEvtNum() const { return fMinEvt; }
   Double_t GetMaxEvtNum() const { return fMaxEvt; }
   Double_t GetMinEntryNum() const { return fMinEntry; }
   Double_t GetMaxEntryNum() const { return fMaxEntry; }
   
   void    SetTree(TTree* t) { fTree = t; }
   void    SetPlotFile(TFile* p) { fPlotf = p; }
   void    SetEvtNum(const Int_t e) { fEvt = e; } // external redraw
   
   void    SetEntryNum(const Int_t entry);
   Bool_t  GetEntryNum(const Int_t event,
                       Int_t& entry);
   Bool_t  GetEventNum(const Int_t entry,
                       Int_t& event);
   
   void DisableChan(const Int_t ch, TSnEvtGuiSeleFrame* seleFrame);
   void EnableChan(const Int_t ch, TSnEvtGuiSeleFrame* seleFrame);
   
   void ToggleWaveChan(const Int_t ch) 
      { fWaveChansOn.at(ch) = !(fWaveChansOn.at(ch)); }
   void SetWaveChanOn(const Int_t ch, const bool on)
      { fWaveChansOn.at(ch) = on; }
   
   void ClearMisc(TCanvas* c);
   void RedrawMisc(TCanvas* c,
                   TSnEvtGuiSeleFrame* seleFrame);

   void ClearWaves(TCanvas* c);
   void ChangeWaveChans(TCanvas* c,
                        TSnEvtGuiSeleFrame* seleFrame,
                        const Int_t ch);
   void RedrawWaveChans(TCanvas* c,
                        TSnEvtGuiSeleFrame* seleFrame);
   void ChangeWaveType(TCanvas* c,
                       TSnEvtGuiSeleFrame* seleFrame,
                       const Int_t id);

   void UpdateLabels(TSnEvtGuiSeleFrame* seleFrame);


   void ReadAllWaveHists(TSnEvtGuiSeleFrame* seleFrame);
   TObjArray* NewListOfObjsWithTag(const Char_t* tag);
   void ReadAllMiscHists(TSnEvtGuiSeleFrame* seleFrame);
   Int_t SendNamesTo(TObjArray* names,
                     TGComboBox* combo,
                     const Int_t start);
   
   ClassDef(TSnEvtGuiDataGuru, 1); // manage the data and send plots to the gui
};

#endif // SNS_TSnEvtGuiDataGuru
