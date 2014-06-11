// $Id: runExampleMod.C 5117 2008-05-06 21:24:30Z loizides $

//////////////////////////////////////////////////////////////////////
//
//            TAM Example Mixing Module Macro
//
//
//   The TAM Example Mixing Module Macro runs the Example Module
//   The Example Module Macro makes an ExampleMod object that 
//   it then loads into the selector. The selector is then loaded 
//   with the mixer loader. A TChain is created to hold a one or 
//   trees that will then be processed.The output is located in the
//   current folder under example_output.root.
//
//  
//////////////////////////////////////////////////////////////////////

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TStopwatch.h>
#include "ExampleMod.cxx"
#include "MixerLoader.cxx"
#endif

//________________________________________________________
void runExampleMod(const char *filename1, 
                   const char *filename2=0, 
                   const char *ofile = "example_output.root")
{ 

   //Declare Example Module to be loaded to TAM Selector Module 
   ExampleMod *mymod = new ExampleMod("ExampleMod",
				      "Example module working on event branch");
   
   //Make the TAM selector and add the example module to it
   TAMSelector *selector = new TAMSelector; 
   selector->AddInput(mymod);

   //Declare Tree T and retrieve it from filename1
   TChain *chain = new TChain("T");
   chain->AddFile(filename1);
   
   //If input is two files, add friend to chain 
   if (!(filename2 == 0)) {
      
      //Declare Tree T and retrieve it from filename2
      TChain *friendchain = new TChain("T");
      friendchain->AddFile(filename2);
      
      //Add tree as friend of main chain
      chain->AddFriend(friendchain,"T_2");   

      //Declare TAM Module and load Example Module to TAM Module
      MixerLoader *mloader = new MixerLoader;
      //Add Mixer Loader to TAMSelector
      selector->AddLoader(mloader);
   }
   
   //Run and use stopwatch to time
   TStopwatch sw;
   chain->Process(selector);
   sw.Stop();
   cout <<"Processing time --> " << flush; sw.Print();
   
   //Get Output from Mudule and open output file and write to it
   TList *out = selector->GetModOutput();
   std::auto_ptr<TFile> outf(TFile::Open(ofile,"recreate"));
   out->Write(); 
   
   //Clear modules and memory
   delete chain;
   delete mymod;
   delete selector;
} 
