// $Id: MixerLoader.cxx 5117 2008-05-06 21:24:30Z loizides $


#ifndef ROOT_Iostream
#include <Riostream.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif
#ifndef TAMBranchInfo_h
#include <TAMBranchInfo.h>
#endif
#ifndef  TAMVirtualBranchLoader_h
#include <TAMVirtualBranchLoader.h>
#endif
#ifndef MixerLoader_h
#include "MixerLoader.h"
#endif
#ifndef TreeMixer_h
#include "TreeMixer.h"
#endif
#ifndef MixerBranchLoader_h
#include "MixerBranchLoader.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
//
//                ROOT Standalone Mixer Loader
// 
//   
//      The Mixer Loader creates Mixer Branch Loaders. It also sets the name of the 
//   superbranch event and it notifies when a file is added In calling the Notify
//   function the Mixer is initialized.
//
//
////////////////////////////////////////////////////////////////////////////////////////////////


ClassImp(MixerLoader);


//_____________________________________________________________________________________________
MixerLoader::MixerLoader() : 
   TAMVirtualLoader(), fMixer(NULL), fTree(NULL), fIsInit(kFALSE), fNLoaders(0), 
   fNNotified(0),fEvName("")
{
   // Default constructor.
  
   Init();
}


//_____________________________________________________________________________________________
MixerLoader::~MixerLoader()
{
   // Destructor 
   
   delete fMixer;
   fMixer = 0;
   fTree  = 0;
}


//_____________________________________________________________________________________________
TAMVirtualBranchLoader* MixerLoader::CreateBranchLoader(TTree *tree, TAMBranchInfo* brInfo)
{
   // Create MixerBranchLoaders to load branches.
   
   //Check branch name
   if(!SetBrName(brInfo)) {
      Error("CreateBranchLoader", "Could not determine branch name!");
      return 0;
   }
   
   //Setup mixer branch loader
   MixerBranchLoader *mb = new MixerBranchLoader(brInfo, this, fMixer);
   mb->SetMixer(fMixer);
   mb->SetBranchName(brInfo->GetName());
   
   //Increase number of loaders to remember in notify
   fNLoaders++;
   
   return mb;
}


//_____________________________________________________________________________________________
Bool_t MixerLoader::Init() 
{
   // Initiate MixerLoader variables.
  
   //Check if already initialized
   if(fIsInit == kTRUE){return kTRUE;}
   
   //Initialize 
   fMixer = new TreeMixer();
   
   fNLoaders = 0;
   fNNotified = 0;
   fIsInit = kTRUE;
   
   return kTRUE;
}


//_______________________________________________________________________________________________
Bool_t MixerLoader::Notify(TTree* tree) 
{ 
   // Make sure that a notify received from the loaders (which receive it
   // from TAM) notifies (and initializes) the mixer exactly once.
   
   Bool_t ret=kTRUE;
   
   if(fNNotified==0) {
      fTree=tree;
      ret=fMixer->Notify(tree);
   }
   
   fNNotified++;
   if(fNNotified==fNLoaders)
      fNNotified=0;
   
   return ret;
}


//_____________________________________________________________________________________________
Bool_t MixerLoader::SetBrName(TAMBranchInfo *binfo)
{
   // Set branch name.
   
   const TString bname(binfo->GetName());
   
   if(bname.CompareTo("event")==0) {
      fMixer->SetEvName("event");
      return kTRUE;}
   
   return kFALSE;
}

