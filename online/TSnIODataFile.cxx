#include "TSnIODataFile.h"

#include <TTimeStamp.h>
#include <TError.h>

#include "TSnIOObject.h"
#include "TSnIOConfigFrame.h"
#include "TSnIOEventFrame.h"
#include "TSnIOHeaderFrame.h"
#include "TSnIOHeartbeatFrame.h"
#include "TSnIOStatusFrame.h"
#include "TSnIOPowerFrame.h"
#include "TSnIOClockSetFrame.h"
#include "TSnDAQConfig.h"
#include "TSnRunInfo.h"
#include "TSnCompMbed.h"
#include "TSnEvent.h"
#include "TSnStatusUpdate.h"
#include "TSnPowerReading.h"
#include "TSnHeartbeat.h"
#include "TSnClockSetInfo.h"

#include <cstdio>
#include <iostream>
using namespace std;

ClassImp(TSnIODataFile);

const UChar_t TSnIODataFile::kIOvers  = 4; // version of file header

//#define DEBUG


UInt_t TSnIODataFile::ReadFileHeader(FILE* file,
                                     TSnRunInfo& runf,
                                     TSnCompMbed& comp,
                                     UShort_t& seq,
                                     TSnPowerReading* pow,
                                     Int_t& version) {
   const UInt_t fbeg = ftell(file);
   version = ReadHeaderFrom(file,runf,comp,seq,pow);
   return (ftell(file) - fbeg);
}

UInt_t TSnIODataFile::PyReadFileHeader(const TSnIOBuffer& buf,
				       TSnRunInfo& runf,
				       TSnCompMbed& comp,
				       Int_t& seq,
				       TSnPowerReading* pow,
                                       Int_t& version) {
   // for use in python, since UShort_t& binding does not work
   UShort_t s(0);
   const UInt_t ret = BytesReadFileHeader(buf, runf, comp, s, pow, version);
   seq = s;
   return ret;
}

UInt_t TSnIODataFile::BytesReadFileHeader(const TSnIOBuffer& b,
					  TSnRunInfo& runf,
					  TSnCompMbed& comp,
					  UShort_t& seq,
					  TSnPowerReading* pow,
                                          Int_t& version) {
   const TSnIOBuffer::Pos_t op = b.GetPosVal();
   version = ReadHeaderFrom(b, runf, comp, seq, pow);
   return b.GetPosVal() - op;
}

UInt_t TSnIODataFile::ReadBlockHeader(FILE* file, 
                                      UChar_t& hcode, UInt_t& hlen) {
   const UInt_t fbeg = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t fend = ftell(file);
   fseek(file, fbeg, SEEK_SET);
   if ( (fend - fbeg) >= TSnIOHeaderFrame::SizeOf() ) {
      TSnIOHeaderFrame::ReadFrom(file, hcode, hlen);
#ifdef DEBUG
      Printf("<TSnIODataFile::ReadBlockHeader>: hcode=%hhu, hlen=%u",
         hcode, hlen);
#endif
      return (ftell(file) - fbeg);
   } else {
#ifdef DEBUG
      Printf("<TSnIODataFile::ReadBlockHeader>: fend=%u, fbeg=%u < hd size %u",
         fend, fbeg, TSnIOHeaderFrame::SizeOf());
#endif
      return 0;
   }
}

UChar_t TSnIODataFile::PeekIOversion(const TSnIOBuffer& b) {
   return TSnIOObject::PeekAt<UChar_t>(b);
}

UChar_t TSnIODataFile::PeekIOversion(FILE* file) {
   // jump to beginning of file, examine header for the version,
   // then jump back to the current position in the file.
   const UInt_t fbeg = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t fend = ftell(file);
   fseek(file, fbeg, SEEK_SET);
   UChar_t rv=0;
   if ( (fend-fbeg) >= sizeof(UChar_t) ) {
      rv = TSnIOObject::ReadFrom<UChar_t>(file);
   }
   fseek(file, fbeg, SEEK_SET);
#ifdef DEBUG
   Printf("<TSnIODataFile::PeekIOversion>: rv=%hhu",rv);
#endif
   return rv;
}

UInt_t TSnIODataFile::ReadConfig(FILE* file, 
                                 TSnDAQConfig& conf,
                                 TSnRunInfo& runf,
                                 TSnNetAdrSet& nets) {
   const UInt_t fbeg = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t fend = ftell(file);
   fseek(file, fbeg, SEEK_SET);
   if ( (fend - fbeg) >= TSnIOConfigFrame::SizeOf(PeekIOversion(file),conf) ) {
      TSnIOConfigFrame::ReadFrom(file, conf, runf, nets);
      return (ftell(file) - fbeg);
   } else {
      return 0;
   }
}

UInt_t TSnIODataFile::ReadPower(FILE* file,
                                TSnPowerReading& pow) {
   const UInt_t fbeg = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t fend = ftell(file);
   fseek(file, fbeg, SEEK_SET);
   if ( (fend - fbeg) >= TSnIOPowerFrame::SizeOf(PeekIOversion(file)) ) {
      TSnIOPowerFrame::ReadFrom(file, pow);
      return (ftell(file)-fbeg);
   } else {
      return 0;
   }
}

UInt_t TSnIODataFile::ReadStatus(FILE* file,
                                 const TSnDAQConfig& conf,
                                 TSnStatusUpdate& stat,
                                 TSnEvent* const evt) {
  const UInt_t fpos = ftell(file);
  fseek(file, 0, SEEK_END);
  const UInt_t end = ftell(file);
  //number of bytes left in the file
  const UInt_t Nbytes = end - fpos;
  fseek(file, fpos, SEEK_SET);

  // number of bytes in this event
  const UChar_t erv = PeekIOversion(file);
  const UInt_t sbytes = TSnIOStatusFrame::SizeOf(erv, conf);

  if ( (sbytes>Nbytes) && (Nbytes!=0) ) {
     throw std::runtime_error(Form("<TSnIODataFile::ReadStatus>: "
                                   "Unexpected end of file. "
                                   "Need [%u] bytes for "
                                   "status update, but only [%u] bytes remain "
                                   "in file.",sbytes,Nbytes));
  } else {
     TSnIOBuffer buf(sbytes);
     const UInt_t fbef = ftell(file);
     const UInt_t nels = buf.ReadFrom(file, sbytes, 1);
     const UInt_t result=static_cast<UInt_t>(ftell(file)) - fbef;
     if ( (result==sbytes) && (nels==1) ) {
        TSnIOStatusFrame::ReadFrom(buf, stat, evt);
     } else {
        throw std::runtime_error(Form("<TSnIODataFile::ReadStatus>: "
                                      "Unable to read status from file. "
                                      "evtIOvers=%hhu, sbytes=%u, "
                                      "bytes read=%u, nels=%u",
                                      erv,sbytes,result,nels));
     }
        
     /*
    Char_t* buf = new Char_t[sbytes];
    const UInt_t fbef = ftell(file);
    const UInt_t nels = fread(buf, sbytes, 1, file);
    const UInt_t result=static_cast<UInt_t>(ftell(file)) - fbef;
    if ( (result!=sbytes) || (nels!=1) ) {
      delete[] buf; buf=0;
      throw std::runtime_error(Form("<TSnIODataFile::ReadStatus>: "
         "Unable to read status from file. "
         "evtIOvers=%hhu, sbytes=%u, bytes read=%u, nels=%u",
         erv,sbytes,result,nels));
    } else {
       TSnIOStatusFrame::ReadFrom(buf, stat, evt);
    }
    delete[] buf;
     */
  }
  return ftell(file) - fpos;
}

UInt_t TSnIODataFile::ReadEvent (FILE* file,
                                 const TSnDAQConfig& conf,
                                 TSnEvent&  evt) {

   return ReadEvent(file, 
                    conf.GetWvLoseLSB(), conf.GetWvLoseMSB(),
                    conf.GetWvBaseline(),
                    evt);
}

UInt_t TSnIODataFile::ReadEvent (FILE* file,
                                  const UChar_t loseLSB,
                                  const UChar_t loseMSB,
                                  const UShort_t wvBaseline,
                                  TSnEvent& evt) {
   const UInt_t fpos = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t end = ftell(file);
   //number of bytes left in the file
   const UInt_t Nbytes = end - fpos;
   fseek(file, fpos, SEEK_SET);

   // number of bytes in this event
   const UChar_t erv = PeekIOversion(file);
   const UInt_t Evtbytes = TSnIOEventFrame::SizeOf(erv,
                                                   loseLSB,loseMSB);
   if ( (Nbytes!=0) && (Evtbytes>Nbytes) ) {
      ::Error("ReadNextEvent","Nbytes=%u, Evtbytes=%u",
              Nbytes, Evtbytes);
      throw std::runtime_error("<TSnIODataFile::ReadNextEvent>: "
                               "Requested event not in file.");
   } else {
      TSnIOBuffer buf(Evtbytes);
      const UInt_t fbef = ftell(file);
      const UInt_t nels = buf.ReadFrom(file, Evtbytes, 1);
      const UInt_t result = static_cast<UInt_t>(ftell(file)) - fbef;
      if ( (result==Evtbytes) && (nels==1) ) {
         TSnIOEventFrame::ReadFrom(buf,
                                   evt,
                                   loseLSB,
                                   loseMSB,
                                   wvBaseline);
      } else {
         ::Error("ReadNextEvent",
                 "evtIOvers=%hhu, Evtbytes=%u, bytes read=%u, nels=%u",
                 erv,Evtbytes,result,nels);
         if (ferror(file)) {
            throw std::runtime_error("<TSnIODataFile::ReadNextEvent>: "
                                     "Error reading file.");
         } else if (feof(file)) {
            throw std::runtime_error("<TSnIODataFile::ReadNextEvent>: "
                                     "End of file reached before event read");
         } else {
            throw std::runtime_error("<TSnIODataFile::ReadNextEvent>: "
                                     "Event not read from file, "
                                     "but no file error.");
         }
      }
   }
  /*
  try {
    if ((Evtbytes > Nbytes)&&(Nbytes != 0)) {
      throw 1;
    }
    Char_t* buf = new Char_t[Evtbytes];
    const UInt_t fbef = ftell(file);
    const UInt_t nels = fread(buf, Evtbytes, 1, file);
    const UInt_t result=static_cast<UInt_t>(ftell(file)) - fbef;
    if ( (result!=Evtbytes) || (nels!=1) ) {
      Printf("evtIOvers=%hhu, Evtbytes=%u, bytes read=%u, nels=%u",
         erv,Evtbytes,result,nels);
      delete[] buf;
      if (ferror(file)) {throw 2;}
      else if (feof(file))   {throw 3;}
      else { throw 4; }
    } else {
       TSnIOEventFrame::BytesReadFrom(buf,
                                      evt,
	  	 	      	      loseLSB,
                                      loseMSB,
                                      wvBaseline);

       delete[] buf;
    }
  }
  catch (int e) {
    if (e == 1) {
      cout << "TSnIODataFile::ReadNextEvent(): Unexpected end of file" << endl;
    }
    else if (e == 2) {
      cout << "TSnIODataFile::ReadNextEvent(): "
              "Incorrect number of bytes read" << endl;
    }
    else if (e == 3) {
      cout << "End of file reached" << endl;
    } else if (e == 4) {
      cout << "Read unexpected number of bytes, "
              "but not at EOF and no file error." << endl;
    }
  }
  */
   return ftell(file) - fpos;
}

UInt_t TSnIODataFile::ReadHeartbeat(FILE* file,
                                    TSnHeartbeat& hrtbt,
                                    const Bool_t isV1) {
   UInt_t n;
   TTimeStamp t;
   const UInt_t b = ReadHeartbeat(file, t, n);
   hrtbt.SetNum(n);
   hrtbt.SetTime(t);
   return b;
}

UInt_t TSnIODataFile::ReadHeartbeat(FILE* file,
				    TTimeStamp& hbtime,
				    UInt_t& hbnum,
                                    const Bool_t isV1) {
   const UInt_t fbeg = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t fend = ftell(file);
   fseek(file, fbeg, SEEK_SET);
   if ( (fend - fbeg) >= TSnIOHeartbeatFrame::SizeOf(PeekIOversion(file)) ) {
      if (isV1) {
         TSnIOHeartbeatFrame::ReadFromV1(file, hbtime, hbnum);
      } else {
         TSnIOHeartbeatFrame::ReadFrom(file, hbtime, hbnum);
      }
      return (ftell(file)-fbeg);
   } else {
      return 0;
   }
}

UInt_t TSnIODataFile::ReadClockSet(FILE* file,
                                   TSnClockSetInfo& clk) {
   const UInt_t fbeg = ftell(file);
   fseek(file, 0, SEEK_END);
   const UInt_t fend = ftell(file);
   fseek(file, fbeg, SEEK_SET);
   if ( (fend - fbeg) >= TSnIOClockSetFrame::SizeOf(PeekIOversion(file)) ) {
      TSnIOClockSetFrame::ReadFrom(file, clk);
      return (ftell(file)-fbeg);
   } else {
      return 0;
   }
}

UInt_t TSnIODataFile::WriteFileHeader(const Char_t* filename,
                                      const TSnRunInfo&   runf,
                                      const TSnCompMbed&  comp,
                                      const UShort_t seq) {
  FILE* outfile = fopen(filename, "wb");
  TSnIOObject::WriteTo(outfile, kIOvers);
  const ULong64_t maddr = comp.GetMACVal();
  TSnIOObject::WriteTo(outfile, maddr);
  TSnIOObject::WriteTo(outfile, runf.GetRun());
  TSnIOObject::WriteTo(outfile, seq);

  return ftell(outfile);
}

UInt_t TSnIODataFile::WritePower(FILE* outfile,
                                 const TSnPowerReading& pow) {
   const Int_t fbeg = ftell(outfile);
   TSnIOHeaderFrame::WriteTo(outfile, TSnIOHeaderFrame::kPowerCode,
      TSnIOPowerFrame::SizeOf(TSnIOPowerFrame::kIOvers));
   TSnIOPowerFrame::WriteTo(outfile, pow);
   return ftell(outfile) - fbeg;
}

UInt_t TSnIODataFile::WriteConfig(FILE* outfile,
                                  const TSnDAQConfig& conf,
                                  const TSnRunInfo& runf,
                                  const TSnNetAdrSet& nets) {
   const Int_t fbeg = ftell(outfile);
   TSnIOHeaderFrame::WriteTo(outfile, TSnIOHeaderFrame::kConfigCode,
      TSnIOConfigFrame::SizeOf(TSnIOConfigFrame::kIOvers, conf));
   TSnIOConfigFrame::WriteTo(outfile, conf, runf, nets);
   return ftell(outfile) - fbeg;
}

UInt_t TSnIODataFile::WriteEvent(FILE* outfile,
                                 const TSnDAQConfig& conf,
                                 const TSnEvent& evt) {

   const Int_t fbeg = ftell(outfile);
   
   //number of bytes in this event
   const UInt_t Evtbytes = TSnIOEventFrame::SizeOf(TSnIOEventFrame::kIOvers,
                                                   conf.GetWvLoseLSB(),
                                                   conf.GetWvLoseMSB());
   
   TSnIOHeaderFrame::WriteTo(outfile, TSnIOHeaderFrame::kEventCode, Evtbytes);
   /*
   Char_t* buf = new Char_t[Evtbytes];
   TSnIOEventFrame::WriteTo(buf,
                            evt,
                            conf.GetWvLoseLSB(),
                            conf.GetWvLoseMSB(),
                            conf.GetWvBaseline());


   fwrite(buf, 1, Evtbytes, outfile);

   delete[] buf;
   */
   TSnIOBuffer buf;
   TSnIOEventFrame::WriteTo(buf, evt,
                            conf.GetWvLoseLSB(),
                            conf.GetWvLoseMSB(),
                            conf.GetWvBaseline());
   buf.WriteTo(outfile);
   return ftell(outfile) - fbeg;
}

UInt_t TSnIODataFile::WriteClockSet(FILE* outfile,
                                    const TSnClockSetInfo& clk,
                                    const Bool_t isStart) {
   const Int_t fbeg = ftell(outfile);
   TSnIOHeaderFrame::WriteTo(outfile,
      isStart ? TSnIOHeaderFrame::kFileTrgStrtCode
              : TSnIOHeaderFrame::kFileTrgStopCode,
      TSnIOClockSetFrame::SizeOf(TSnIOClockSetFrame::kIOvers));
   TSnIOClockSetFrame::WriteTo(outfile, clk);
   return ftell(outfile) - fbeg;
}

UChar_t TSnIODataFile::ReadHeaderFromW(const TSnIOBuffer& b,
                                       TSnRunInfo& runf,
                                       TSnCompMbed& comp,
                                       UShort_t& seq,
                                       TSnPowerReading* pow) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadHeaderFrom(b, runf, comp, seq, pow);
}

UChar_t TSnIODataFile::ReadHeaderFromW(const Char_t*& b,
                                       TSnRunInfo& runf,
                                       TSnCompMbed& comp,
                                       UShort_t& seq,
                                       TSnPowerReading* pow) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadHeaderFrom(b, runf, comp, seq, pow);
}

UChar_t TSnIODataFile::ReadHeaderFromW(FILE*& b,
                                       TSnRunInfo& runf,
                                       TSnCompMbed& comp,
                                       UShort_t& seq,
                                       TSnPowerReading* pow) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadHeaderFrom(b, runf, comp, seq, pow);
}


