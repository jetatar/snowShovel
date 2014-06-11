#include "TSnEventHeader.h"

#include "TString.h"

ClassImp(TSnEventHeader);

void TSnEventHeader::Print(Option_t* option) const {
  Printf("  time          = %s", fTime.AsString());
  Printf("  event number  = %u", fNum);
  Printf("  trigger num   = %u", fTrgNum);
  Printf("  delta T (ms)  = %d", fDTms);
  printf("  trigger bits  = ");  fTrgInfo.PrintBits(kTRUE);
}

