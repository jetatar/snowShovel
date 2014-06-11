// $Id: ExampleMod.h 5117 2008-05-06 21:24:30Z loizides $

#ifndef ExampleMod_H
#define ExampleMod_H

#include <TAModule.h>
#include <../test/Event.h>

class Track;
class TH1D;
class Event;
class TCanvas;
class TPad;
class EventHeader;


class ExampleMod : public TAModule {
 private:
   Event          *fEvent;         //!Event Superbranch  
   TH1D           *fNtrackhist;    //!NTrack Histogram
   TH1D           *fPthist;        //!Pt Histogram
   TH1D           *fBxhist;        //!Bx Histogram
   Long64_t        fCounter;       //!Count number of processed events
   
 protected:
   void       Begin();
   void       Process();
   void       SlaveBegin();
   void       SlaveTerminate();
   void       Terminate();
   
 public:
   ExampleMod(const char *name="examplemod", 
              const char *title="TAM Example Module"); 
   virtual ~ExampleMod();
   
   void  FillHist();
   void  MakeHist();
   
   ClassDef(ExampleMod,1); //ROOT standalone Example TAModule 
 };

#endif /*ExampleMod*/
