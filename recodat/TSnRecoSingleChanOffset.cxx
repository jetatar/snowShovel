#include "TSnRecoSingleChanOffset.h"

ClassImp(TSnRecoSingleChanOffset);


void TSnRecoSingleChanOffset::Print(Option_t* option) const {
   TSnRecoResult::Print(option);
   Printf("Time Offset     = %g", fOff);
   Printf("Offset Error    = %g", fErr);
   Printf("Fit Quality     = %g", fLik);
}


