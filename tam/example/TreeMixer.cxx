// $Id: TreeMixer.cxx 5117 2008-05-06 21:24:30Z loizides $

#include "TreeMixer.h"

#ifndef ROOT_Iostream
#include <Riostream.h>
#endif
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif
#ifndef ROOT_TFile
#include <TFile.h>
#endif
#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif
#ifndef ROOT_TBranch
#include <TBranch.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif
#ifndef ROOT_TFriendElement
#include <TFriendElement.h>
#endif
#ifndef ROOT_TList
#include <TList.h>
#endif


///////////////////////////////////////////////////////////////////////////
//
//   TreeMixer
//
//   The TreeMixer does the actual mixing of events or rather of the 
//   tracks in the event. In th
//   the Tracks and Ntrack are mixed producing a mixed tree. The Mixing 
//   of tracks is done by using the Expand create and new with placement 
//   because the TClonesArray Class does not support using Add from 
//   TObjArray to add objects.The Ntrack mixing is done by addition.
//   The Mixer returns all mixed events.
//
//
////////////////////////////////////////////////////////////////////////////


ClassImp(TreeMixer);
ClassImp(TreeMixer::TreeDataLoader);


//_________________________________________________________________________
TreeMixer::TreeMixer():
   fEvent(NULL), fMixedTracks(NULL),fTracks(NULL),fMixedNtrack(0), 
   fTrees(NULL), fTree(NULL), fMixEvName(""), fEvName(""), 
   fExecuted(kFALSE), fNLoaders(0)
{ 
   // Default constructor.
   
   fEvent = new Event();
   fMixedNtrack = new Int_t;
}


//__________________________________________________________________________
TreeMixer::~TreeMixer()
{
   // Destructor.
   
   delete fEvent;
   delete fMixedNtrack; 
}


//_______________________________________________________________________________
Bool_t TreeMixer::AddLoader(TTree *tree)
{
   //Add TreeDataLoaders operating on the given tree.

   TreeDataLoader *tdl = new TreeDataLoader(tree);
   fLoaders.push_back(tdl);
   
   return kTRUE;
}


//____________________________________________________________________________
void* TreeMixer::GetAddress(const char* bname) 
{
   // Get address of the branch with given name, if supported.
   
   TString name(bname);
  
   //Compare branch and return mixed event or specific branch
   if(name.CompareTo("event")==0)
      return fEvent;
   if(name.CompareTo("fTracks")==0)
      return fMixedTracks;
   if(name.CompareTo("fNtrack")==0)
      return fMixedNtrack;
   
   return 0;
}


//____________________________________________________________________________
Int_t TreeMixer::GetEntry(Int_t entry, const char* bname) 
{
   // Get an entry from TreeDataLoader.   
   
   //Set name (ie branch name) to bname
   TString name(bname);
   
   //Compare branch name to event and return event if there is a match
   if(name.CompareTo("event")==0) {
     
      Int_t returnvalue = GetEvent(entry);
      return returnvalue;
   }
   
   return -1;
}


//____________________________________________________________________________
Int_t TreeMixer::GetEvent(Long64_t entry) 
{
   // Get event from data entry. 
   
   fMixedTracks = fEvent->GetTracks();
   
   // Mix Ntracks 
   
   *fMixedNtrack = 0;

   
   //Loop over loaders and retrieve Ntrack then mix
   //save new mixed Ntrack in fMixedNtrack    
   
   for(UInt_t i=0; i < fLoaders.size(); i++){
      fLoaders[i]->GetEntry(entry);
      Event *Ev = fLoaders[i]->GetPointer();
      Int_t Nt = Ev->GetNtrack();
      
      *fMixedNtrack = *fMixedNtrack + Nt;
   }
   
   //Set value of Ntrack in fEvent to mixed value
   fEvent->SetNtrack(*fMixedNtrack);
   
   
   // Mix fTracks 
   
   //Expand create using mixed Ntrack on TClonesArray then fill
   //the expanded array 
   
   fMixedTracks->ExpandCreate(fEvent->GetNtrack());   
   
   Int_t K = 0;
   
   //Loop over loaders and retrieve track array
   for(UInt_t i=0; i < fLoaders.size(); i++){
      fLoaders[i]->GetEntry(entry);
      Event *Ev = fLoaders[i]->GetPointer();
      TClonesArray *Trk = Ev->GetTracks();
      
      //Fill track arrray with new, producing mixed tracks
      for(Int_t j=0; j < Ev->GetNtrack() ; j++){      
	 
	 new(static_cast<void *>((*fMixedTracks)[K])) Track(*static_cast<Track *>((*Trk)[j]));
	 K ++;
      }      
   }  

   return 1;   
}


//_______________________________________________________________________________
Bool_t TreeMixer::Notify(TTree* tree)  
{
   // Notify that the tree has changed. Main usage will be in  
   // the mixing with TAM. 
   
   //Check fExecuted
   if (fExecuted == kTRUE)
      return kTRUE;

   //Set tree and get list of friends
   fTree = tree;
   
   fTrees = tree->GetListOfFriends();
   
   //Add tree to loader
   AddLoader(fTree);
   
   if(fTrees) { // loop over list of its friends and add to loader
      TIter nextTree(fTrees->MakeIterator());
      TFriendElement* fe = 0;
      while ( (fe = dynamic_cast<TFriendElement *>(nextTree())) ) {
	 
         if(!AddLoader(fe->GetTree())) {
            cerr << "Could not add loader to tree with name from file" << endl;
         }
      }
   }
   
   fExecuted = kTRUE;
   
   return kTRUE;
}

//___________________________________________________________________________
void TreeMixer::Terminate() 
{
   // Terminate mixer and clear variables.
   
   fNLoaders   = 0;
   fEvent      = 0;
   fTracks     = 0;
}

//________________________________________________________________________
void TreeMixer::TreeDataLoader::GetEntry(Int_t ent) 
{
   // Get entry from tree data.
   
   //Get event branch and set its address to fEvent pointer 
   if(fBranch == 0)
      {
       fBranch = fTree->GetBranch("event");
       fBranch->SetAddress(&fEvent);
      }
   
   //Get ent entry
   fBranch->GetEntry(ent);
}
