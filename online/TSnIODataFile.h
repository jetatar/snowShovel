#ifndef SNS_TSnIODataFile
#define SNS_TSnIODataFile

class TTimeStamp;

#include "TSnIOObject.h"
#include "TSnRunInfo.h"
#include "TSnCompMbed.h"
#include "TSnPowerReading.h"
class TSnDAQConfig;
class TSnIOConfigFrame;
class TSnIOEventFrame;
class TSnStatusUpdate;
class TSnEvent;
class TSnNetAdrSet;
class TSnHeartbeat;
class TSnClockSetInfo;

class TSnIODataFile : public TSnIOObject {
 public:
   static const UChar_t  kIOvers;
   
   TSnIODataFile() {}
   virtual ~TSnIODataFile() {}
   
   static
   UInt_t SizeOfFileHeader(const UChar_t rv) {
      UInt_t s = sizeof(UChar_t) // i/o vers
         + sizeof(ULong64_t) // mac adr
         + sizeof(UInt_t) // run num
         + sizeof(UShort_t); // seq num
      if (rv==2) {
         s += 2u*sizeof(UShort_t); // voltages
      }
      return s;
   }

   template<class T>
   static
   UChar_t ReadHeaderFrom(T& b,
                          TSnRunInfo& runf,
                          TSnCompMbed& comp,
                          UShort_t& seq,
                          TSnPowerReading* pow) {
     const UChar_t version = TSnIOObject::ReadFrom<UChar_t>(b);
     if (version>0) {
       comp.SetMAC(TSnIOObject::ReadFrom<ULong64_t>(b));
       runf.SetRun(TSnIOObject::ReadFrom<UInt_t>(b));
       seq = TSnIOObject::ReadFrom<UShort_t>(b);
       if (version==2) {
         const UShort_t v1 = TSnIOObject::ReadFrom<UShort_t>(b);
         const UShort_t v2 = TSnIOObject::ReadFrom<UShort_t>(b);
         if (pow!=0) {
	   pow->Set(v1, v2, 0, 0, 0);
         }
       }
     }
     return version;
   }

   static
   UInt_t PyReadFileHeader(const TSnIOBuffer& buf,
			   TSnRunInfo& runf,
			   TSnCompMbed& comp,
			   Int_t& seq,
			   TSnPowerReading* pow,
                           Int_t& version);
  
   static
   UInt_t BytesReadFileHeader(const TSnIOBuffer& b,
			      TSnRunInfo& runf,
			      TSnCompMbed& comp,
			      UShort_t& seq,
			      TSnPowerReading* pow,
                              Int_t& version);

   static
   UInt_t ReadFileHeader(FILE* file,
                         TSnRunInfo& runf,
                         TSnCompMbed& comp,
                         UShort_t& seq,
                         TSnPowerReading* pow,
                         Int_t & version);
   
   static
   UInt_t ReadBlockHeader(FILE* file, UChar_t& hcode, UInt_t& hlen);
   
   static
   UChar_t PeekIOversion(const TSnIOBuffer& b);

   static
   UChar_t PeekIOversion(FILE* file);
   
   static
   UInt_t ReadConfig(FILE* file, 
                     TSnDAQConfig& conf,
                     TSnRunInfo& runf,
                     TSnNetAdrSet& nets);
   
   static
   UInt_t ReadPower(FILE* file,
                    TSnPowerReading& pow);

   static
   UInt_t ReadStatus(FILE* file,
                     const TSnDAQConfig& conf,
                     TSnStatusUpdate& stat,
                     TSnEvent* const evt=0);

   static
   UInt_t ReadEvent (FILE* file,
                     const TSnDAQConfig& conf,
                     TSnEvent&  evt);

   static
   UInt_t ReadEvent (FILE* file,
		     const UChar_t loseLSB,
		     const UChar_t loseMSB,
		     const UShort_t wvBaseline,
		     TSnEvent& evt);
   
   static 
   UInt_t ReadHeartbeat(FILE* file,
                        TSnHeartbeat& hrtbt,
                        const Bool_t isV1=kFALSE);
   static
   UInt_t ReadHeartbeat(FILE* file,
			TTimeStamp& hbtime,
			UInt_t& hbnum,
                        const Bool_t isV1=kFALSE);

   static
   UInt_t ReadClockSet(FILE* file,
                       TSnClockSetInfo& clk);

   static
   UInt_t WriteFileHeader(const Char_t* filename,
                          const TSnRunInfo&   runf,
                          const TSnCompMbed&  comp,
                          const UShort_t seq);

   static
   UInt_t WritePower(FILE* outfile,
                     const TSnPowerReading& pow);

   static
   UInt_t WriteConfig(FILE* outfile,
                      const TSnDAQConfig& conf,
                      const TSnRunInfo& runf,
                      const TSnNetAdrSet& nets);

   static
   UInt_t WriteEvent(FILE* outfile,
                     const TSnDAQConfig& conf,
                     const TSnEvent& evt);

   static
   UInt_t WriteClockSet(FILE* outfile,
                        const TSnClockSetInfo& clk,
                        const Bool_t isStart);

   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadHeaderFromW(const TSnIOBuffer& b,
                           TSnRunInfo& runf,
                           TSnCompMbed& comp,
                           UShort_t& seq,
                           TSnPowerReading* pow);
   static
   UChar_t ReadHeaderFromW(const Char_t*& b,
                           TSnRunInfo& runf,
                           TSnCompMbed& comp,
                           UShort_t& seq,
                           TSnPowerReading* pow);
   static
   UChar_t ReadHeaderFromW(FILE*& b,
                           TSnRunInfo& runf,
                           TSnCompMbed& comp,
                           UShort_t& seq,
                           TSnPowerReading* pow);


   ClassDef(TSnIODataFile, 1); // for reading a raw data file
};

#endif
