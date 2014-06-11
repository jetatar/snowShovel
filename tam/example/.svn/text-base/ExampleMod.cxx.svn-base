// $Id: ExampleMod.cxx 5117 2008-05-06 21:24:30Z loizides $

#include <TH1D.h>
#include <TChain.h>
#include <TAMSelector.h>
#include <TFile.h>
#include <TRefArray.h>
#include "ExampleMod.h"

//////////////////////////////////////////////////////////////////////
//
//            TAM Example Mixing Module 
//
//
//   The TAM Example Mixing Module reads a single tree or uses the  
//   ROOT standalone Mixer to Mix two trees from two Event files. 
//   The Example Module makes no distinction between mixed or single 
//   trees, this is possible because the Example Module uses the TAM  
//   Module to mix events and combine them into a single tree before  
//   processing the events. 
//
//
//////////////////////////////////////////////////////////////////////


ClassImp(ExampleMod);

		 
//________________________________________________________________________
ExampleMod::ExampleMod(const char *name, const char *title):
   TAModule(name,title), fEvent(NULL), fNtrackhist(NULL),
   fPthist(NULL), fBxhist(NULL), fCounter(0)  
{
   // Constructor 
}


//________________________________________________________________________
ExampleMod::~ExampleMod()
{
   // Destructor.
}


//_______________________________________________________
void ExampleMod::Begin()
{  
   // Run on client computer. 
   
   //Error handling
   SendError(kWarning,"Begin","Function called in module [%s].", GetName()); 
}


//_______________________________________________________
void ExampleMod::SlaveBegin()
{
   // Run on computer doing analysis.ReqBranch must be located 
   // in this function.
   
   //Call histogram making function
   MakeHist();
   
   //Request branches
   ReqBranch("event",fEvent);
   
   //Error handling
   SendError(kWarning,"SlaveBegin","Function called in module [%s].", GetName()); 
}


//_______________________________________________________
void ExampleMod::Process()
{
   // Process entries. Function called for each event. LoadBranch should
   // be in this function.
   
   //Load branches 
   LoadBranch("event");
   
   //If this module is named ExampleMod, don't let it (or its submodules)
   //process events higher than 10000
   if ( (strcmp(GetName(),"ExampleMod")==0 ) &&
	(fCounter > 10000 ) ) {
      SendError(kAbortModule,"Process","Aborting module [%s].", GetName());
      return; 
   }; 
   
   //Call histogram filling function
   FillHist();
   
   //Count number of events being processed
   fCounter++;
   
   //Error handling every 100 events
   if (fCounter % 100 == 0)
      SendError(kWarning,"Process","Function called in module [%s].", GetName()); 
}


//_______________________________________________________
void ExampleMod::SlaveTerminate()
{ 
   // Terminate for computer that just finished analysis
   // error handling.

   SendError(kWarning,"Slave Terminate","Function called in module [%s].", GetName()); 
}


//_______________________________________________________
void ExampleMod::Terminate()
{ 
   // Terminate on client computer.
   
   cerr <<"Events Processed = " << fCounter << endl;
}


//________________________________________________________
void ExampleMod::MakeHist() 
{  
   // Create some histograms and add them to output.
   
   fNtrackhist = new TH1D("NTrackHist","NTrackHist",100,500,1300);
   AddOutput(fNtrackhist);
   
   fPthist = new TH1D("PtHist","PtHist",110,0,5);
   AddOutput(fPthist);
   
   fBxhist = new TH1D("BxHist","BxHist",110,-0.5,0.5);
   AddOutput(fBxhist);
}


//________________________________________________________
void ExampleMod::FillHist() 
{ 
   // Fill histograms. 
   
   //Fill NTrack histogram
   fNtrackhist->Fill(fEvent->GetNtrack());
   
   //Fill  Pt histogram
   TClonesArray *tracks = fEvent->GetTracks();
   
   for(Int_t i=0;i<(fEvent->GetNtrack());i++) {
      
      Track *t=dynamic_cast<Track*>(tracks->At(i));
      if(!(t == NULL)){
	 fPthist->Fill(t->GetPt());
      };
   }
   
   //Fill Bx histogram
   for(Int_t i=0;i<(fEvent->GetNtrack());i++) {
      
      Track *t=dynamic_cast<Track*>(tracks->At(i));
      if(!(t == NULL)){
	 fBxhist->Fill(t->GetBx());
      };
   }
}
