// $Id: TreeMixer.h 5117 2008-05-06 21:24:30Z loizides $

#ifndef TreeMixer_h
#define TreeMixer_h

#include <vector>
using std::vector;
#ifndef Event_h
#include <../test/Event.h>
#endif

class TObjArray;
class TBranch;
class TTree;
class TFile;
class TreeDataLoader; 
class TString;
class Track;
class TClonesArray;
class TList;


class TreeMixer : public TObject {
  
 private:

 class TreeDataLoader : public TObject {
  private:
    TString                fFilename;       //Filename 
    Event                 *fEvent;          //!Event Class Object  
    TBranch               *fBranch;         //!Event Superbranch
    TTree                 *fTree;           //!Tree from Loader

  public:
    TreeDataLoader(TTree *ntree=0) : fEvent(NULL), fBranch(0), fTree(ntree) {}
    virtual ~TreeDataLoader() {}
   
    void        GetEntry(Int_t ent);
    Event*      GetPointer() { return fEvent; }
    void        SetFilename(TString file) {fFilename = file;}
   
    ClassDef(TreeDataLoader,0); //TreeDataLoader for event trees
  };
  
  Event                          *fEvent;             //local pointer to Event in loader
  TClonesArray                   *fMixedTracks;       //Mixed Track Array
  TClonesArray                   *fTracks;            //Track Array 
  Int_t                          *fMixedNtrack;       //Ntrack after Mixing 
  TList                          *fTrees;             //list of friend trees (or trees given in a chain)
  TTree                          *fTree;              //the tree itself (eg. given by TAM)
  TString                         fMixEvName;         //mixed event branch name
  TString                         fEvName;            //event branch name
  Bool_t                          fExecuted;          //true if Notify() is called
  Int_t                           fNLoaders;          //number of loaders (one for every friend tree
  vector<TreeDataLoader *>        fLoaders;           //Loaders where trees loaded from input are stored
  vector<TString>                 fFilenames;         //Names of input files 

  Bool_t                          AddLoader(TTree *tree);
  Int_t                           GetEvent(Long64_t entry);

 public:  
  TreeMixer();
  ~TreeMixer();
  
  void*               GetAddress(const char* bname);
  Int_t               GetEntry(Int_t entry, const char* bname);
  TString             GetEvName()                      {return fEvName;}
  TString             GetMixEvName()                   {return fMixEvName;}
      //void                Init();

  Bool_t              Notify(TTree* tree);
  void                Reset()                          {fEvent->Clear();}
  void                SetEvName(const char *evn)       {fEvName = evn;}
  void                SetMixEvName(const char *mevent) {fMixEvName= mevent;}
  void                Terminate();
  
  ClassDef(TreeMixer,1); // Event mixer for data in tree friends.
};  

#endif /*TreeMixer*/
