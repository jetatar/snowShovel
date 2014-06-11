// $Id: MixerBranchLoader.cxx 5117 2008-05-06 21:24:30Z loizides $

#ifndef ROOT_Iostream
#include <Riostream.h>
#endif
#ifndef   TAMBrachInfo_h
#include <TAMBranchInfo.h>
#endif
#ifndef MixerBranchLoader_h
#include "MixerBranchLoader.h"
#endif
#ifndef  MixerLoader_h
#include "MixerLoader.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////
//
//                ROOT Standalone Mixer Branch Loader
//
//
//        The Mixer Branch Loader provides an interface between the Mixer and TAM. 
//     This Mixer Branch Loader effectively loads virtual branches that can be mixed   
//     if desired. The Mixer Branch Loader retrieves Branch Information from the Tree. 
//     Mixer. The Mixer Branch Loader is created by the Mixer Loader. It also Resets 
//     the mixer before processing a new event. 
//
//
/////////////////////////////////////////////////////////////////////////////////////////


ClassImp(MixerBranchLoader);


//_______________________________________________________________________________________
MixerBranchLoader::MixerBranchLoader() :
   TAMVirtualBranchLoader(0), fMixer(NULL), fMixerLoader(NULL), fBName("")
{
   // Default Constructor
}


//_______________________________________________________________________________________
MixerBranchLoader::MixerBranchLoader(TAMBranchInfo *binfo, MixerLoader *l,TreeMixer *fTmix) :  
   TAMVirtualBranchLoader(binfo),fMixer(NULL),fMixerLoader(l), fBName("")
{  
   // Initialize
   
   fMixer = fTmix;
   fBName = binfo->GetName();
}


//______________________________________________________________________________________
MixerBranchLoader::~MixerBranchLoader()
{
   // Destructor
}


//______________________________________________________________________________________
Int_t MixerBranchLoader::GetEntry(Long64_t entry) 
{ 
   // Return entry from TreeMixer.
   
   return fMixer->GetEntry(entry, fBName);
}

//______________________________________________________________________________________
void MixerBranchLoader::Clear(Option_t *option/*=""*/)
{
   // Clear up data structures used by the TreeMixer.
   
   fMixer->Reset();
}


//______________________________________________________________________________________
void* MixerBranchLoader::GetAddress() const
{
   // Called by TAM returns branch address from TreeMixer.
   
   return fMixer->GetAddress(fBName);
}


//______________________________________________________________________________________
Bool_t MixerBranchLoader::Notify(TTree *tree)
{
   // Notify called by TAM whenever the tree changes. The branch mixer
   // simply forwards this to its MixerLoader which in turn will 
   // forward it to the TreeMixer.
   
   if(fMixerLoader) fMixerLoader->Notify(tree);
   
   return kTRUE;
}
