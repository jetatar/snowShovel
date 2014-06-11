#include "TSnCompAmp.h"

#include <TTimeStamp.h>

ClassImp(TSnCompAmp);

const Char_t* TSnCompAmp::kCompNm = "amp";

TSnCompAmp::~TSnCompAmp() {
  delete fDateCnstr;
}

void TSnCompAmp::SetDateConstructed(const TTimeStamp& d) {
  delete fDateCnstr;
  fDateCnstr = new TTimeStamp(d);
}

void TSnCompAmp::SetDateConstructed(const UInt_t tloc,
                                    const Bool_t isUTC,
                                    const Int_t secOffset,
                                    const Bool_t dosDate) {
  delete fDateCnstr;
  fDateCnstr = new TTimeStamp(tloc, isUTC, secOffset, dosDate);
}
