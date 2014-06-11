// $Id: MixerLoader.h 5117 2008-05-06 21:24:30Z loizides $

#include <typeinfo>

#ifndef ROOT_TString
#include <TString.h>
#endif
#ifndef ROOT_TList
#include <TList.h>
#endif
#ifndef TAMVirtualLoader_h
#include <TAMVirtualLoader.h>
#endif
#ifndef MixerLoader_h
#define MixerLoader_h

class MixerBranchLoader;
class TString;
class TTree;
class TAMBranchInfo;
class TAMVirtualBranchLoader;
class TreeMixer;


class MixerLoader : public TAMVirtualLoader {
private:
  
   TreeMixer               *fMixer;            //!data mixer which does the real work
   TTree                   *fTree;             //!friend tree given from TAM
   Bool_t                   fIsInit;           //!=true if it is initialized
   UInt_t                   fNLoaders;         //!number of plugins exposed to TAM
   UInt_t                   fNNotified;        //!number of notifications received
   TString                  fEvName;           //event branch name
 
   Bool_t                   Init();
   Bool_t                   SetBrName(TAMBranchInfo *binfo);

public:
   MixerLoader();
   ~MixerLoader();
   
   TAMVirtualBranchLoader*  CreateBranchLoader(TTree *tree, TAMBranchInfo* brInfo);
   Bool_t                   IsInit()                const { return fIsInit; }
   Bool_t                   Notify(TTree *tree);
   void                     SetEventBrName(const char *n) { fEvName = n; }
   
   ClassDef(MixerLoader,1);  // Plugin loader of the branch mixer into TAM
};

#endif /*MixerLoader*/

