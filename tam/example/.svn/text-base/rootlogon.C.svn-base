// $Id: rootlogon.C 5117 2008-05-06 21:24:30Z loizides $

{ 
   Int_t saveL = gErrorIgnoreLevel;
   gErrorIgnoreLevel=kFatal;
   if(gSystem->Load("libTAM.so")==-1) {
      gSystem->Exec("cd $ROOTSYS && make all-tam && make map-tam");
      if(gSystem->Load("libTAM.so")==-1) {
         ::Error("rootlogon.C","Could not create and load libTAM.so!");
         return;
      }
   }

   if(gSystem->Load("libEvent.so")==-1) {
      gSystem->Exec("./prepareEvents.sh");
      if(gSystem->Load("libEvent.so")==-1) {
         gErrorIgnoreLevel=saveL;
         ::Error("rootlogon.C","Could not create and load libEvent.so!");
         return;
      }
   }
  
   if(gROOT->LoadMacro("ExampleMod.cxx+")) {
      gErrorIgnoreLevel=saveL;
      ::Error("rootlogon.C","Could not create and load ExampleMod_cxx.so!");
      return;
   }

   if(gROOT->LoadMacro("TreeMixer.cxx+")) {
      gErrorIgnoreLevel=saveL;
      ::Error("rootlogon.C","Could not create and load TreeMixer_cxx.so!");
      return;
   }

   //compile cross dependent macros
   gSystem->CompileMacro("MixerBranchLoader.cxx","c");
   gSystem->CompileMacro("MixerLoader.cxx","c");

   if(gROOT->LoadMacro("MixerBranchLoader.cxx+")) {
      gErrorIgnoreLevel=saveL;
      ::Error("rootlogon.C","Could not create and load MixerBranchLoader_cxx.so!");
      return;
   }

   if(gROOT->LoadMacro("MixerLoader.cxx+")) {
      gErrorIgnoreLevel=saveL;
      ::Error("rootlogon.C","Could not create and load MixerLoader_cxx.so!");
      return;
   }

   if(gROOT->LoadMacro("runExampleMod.C+")) {
      gErrorIgnoreLevel=saveL;
      ::Error("rootlogon.C","Could not create and load runExampleMod_C.so!");
      return;
   }

   gErrorIgnoreLevel=saveL;

   gStyle->SetFillColor(0);
   gStyle->SetFrameFillColor(6);
   gStyle->SetFrameFillStyle(0);
   gStyle->SetFrameFillColor(6);
   gStyle->SetFrameFillStyle(0);

   Info("rootlogon.C", 
        "Welcome to TAM version: %s", 
        TAModule::Version());
}
