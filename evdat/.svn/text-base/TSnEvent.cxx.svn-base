#include "TSnEvent.h"

#include "TString.h"

#include <cstring>

ClassImp(TSnEvent);

void TSnEvent::Print(Option_t* option) const {
  // use option "d" to print the waveform data
  
  Printf("TSnEvent:");
  fHeader.Print(option);
  fRawWvfm.Print(option);
}


const char* TSnEvent::GetName() const {
  return Form("Event%06u",GetEvtNum());
}

