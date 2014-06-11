#include "TSnCommWinSet.h"

#include <TString.h>

ClassImp(TSnCommWinSet);

void TSnCommWinSet::Print(Option_t* option) const {
   Printf("  comm win dur  = %u", GetDuration());
   Printf("  comm win per  = %u", GetPeriod());
   Printf("  connect t.o.  = %hhu (mins)", GetConnectTOmin());
   Printf("  listen t.o.   = %hhu (mins)", GetListenTOmin());
   if (GetSendData()<0) {
      Printf("  com send bits (%hd) : ", GetSendData());
      Printf("    send %s",(IsSendingAllFiles() ? "all files" : "latest file"));
      Printf("    %s", (IsObeyingTimeout() ? "stop sending at timeout" :
                        "send until finished"));
      Printf("    %s", (IsDeletingFiles() ? "delete file if received" :
                        "keep files on station"));
      Printf("    %s", (IsForcingSBDdata() ? "data files sent even over SBD" :
                        "no data files sent over SBD"));
   } else {
      Printf("  com send data = %hd", GetSendData());
   }
}


