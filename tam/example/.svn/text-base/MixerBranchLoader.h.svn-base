// $Id: MixerBranchLoader.h 5117 2008-05-06 21:24:30Z loizides $

#ifndef MixerBranchLoader_h
#define MixerBranchLoader_h

#ifndef TAMVirtualBranchLoader_h
#include <TAMVirtualBranchLoader.h>
#endif
#ifndef  TreeMixer_h 
#include "TreeMixer.h"
#endif

class TTree;
class TList;
class TAMBranchInfo;
class TString;
class TreeMixer;
class MixerLoader;


class MixerBranchLoader : public TAMVirtualBranchLoader {
private:
   TreeMixer          *fMixer;        //data mixer which does the real work (not owned)
   MixerLoader        *fMixerLoader;  //loader which calls/creates branch loader
   TString             fBName;        //requested branch name for mixing

 public:
   MixerBranchLoader();
   MixerBranchLoader(TAMBranchInfo *binfo, MixerLoader *l, TreeMixer *fTmix);
   ~MixerBranchLoader();
   
   void                   Clear(Option_t *option="");
   void*                  GetAddress() const;
   Int_t                  GetEntry(Long64_t entry); 
   Bool_t                 Notify(TTree *tree);
   void                   SetMixer(TreeMixer *m)          { fMixer = m; }
   void                   SetBranchName(const char* name) { fBName = name; }
   
   ClassDef(MixerBranchLoader,1);  //TAM branch loader plugin for tree data mixing.

};

#endif /*MixerBranchLoader*/

