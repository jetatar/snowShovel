#include "TSnDbKey.h"

#include <TTimeStamp.h>

#include <cstdio>

ClassImp(TSnDbKey);

const Char_t* TSnDbKey::GetSQLTimestampUTC(const TTimeStamp& ts) {
   
   // Given a timestamp, return an ISO-8601 compliant string
   // of the form "TIMESTAMP WITH TIME ZONE 'YYYY-MM-DD HH:MM:SS+00'"
   //
   // Note: the time is ALWAYS converted to UTC.
   //
   // This string should be compliant with an SQL standard TIMESTAMP
   //
   // (The single quotes are included.)
   
   TString s(Form("TIMESTAMP WITH TIME ZONE '%s+00'",ts.AsString("s")));
   return s.Data();
}

Bool_t TSnDbKey::GetTimestampFromStr(TTimeStamp& ts, const Char_t* result) {
   // Given an SQL resulting timestamp string, construct the timestamp ts
   //
   // expect "YYYY-MM-DD HH:MM:SS+HH"
   //
   // return true on success, false on fail
  
   if (result!=0) {
      UInt_t y, m, d, hr, mi, se;
      Int_t  tz;
      const Int_t nv = sscanf(result,"%04u-%02u-%02u %02u:%02u:%02u%d",
                              &y, &m, &d, &hr, &mi, &se, &tz);
      if (nv==7) {
         ts.Set(y,m,d,hr,mi,se,0,kTRUE,-tz*3600);
	 return kTRUE;
      }
   }
   return kFALSE;
}
