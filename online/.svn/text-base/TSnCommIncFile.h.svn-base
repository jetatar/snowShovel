#ifndef SNS_TSnCommIncFile
#define SNS_TSnCommIncFile

#include <TString.h>
class TTimeStamp;

#include "TSnCommIncoming.h"
class TSnStatusUpdate;

class TSnCommIncFile : public TSnCommIncoming {
 private:
   TString          fFilename;
   UInt_t           fNevtsRcvd;
   UInt_t           fNpwrsRcvd;
   UInt_t           fNevtsSent;
   UInt_t           fNpwrsSent;
   UInt_t           fMbedCRC;
   UInt_t           fLocalCRC;
   UChar_t          fFvers;
   
 public:
   TSnCommIncFile() :
      fNevtsRcvd(0), fNpwrsRcvd(0),
      fNevtsSent(0), fNpwrsSent(0),
      fMbedCRC(0), fLocalCRC(0),
      fFvers(0) {}
   TSnCommIncFile(const TTimeStamp& t, 
                  const TSnStatusUpdate& s,
                  const Int_t stvers) : TSnCommIncoming(t,s,stvers),
                                        fNevtsRcvd(0), fNpwrsRcvd(0),
                                        fNevtsSent(0), fNpwrsSent(0),
                                        fMbedCRC(0), fLocalCRC(0),
                                        fFvers(0) {}
   TSnCommIncFile(const Char_t* f) : fFilename(f),
                                     fNevtsRcvd(0), fNpwrsRcvd(0),
                                     fNevtsSent(0), fNpwrsSent(0),
                                     fMbedCRC(0), fLocalCRC(0),
                                     fFvers(0) {}
   TSnCommIncFile(const TString& f) : fFilename(f),
                                      fNevtsRcvd(0), fNpwrsRcvd(0),
                                      fNevtsSent(0), fNpwrsSent(0),
                                      fMbedCRC(0), fLocalCRC(0),
                                      fFvers(0) {}
   TSnCommIncFile(const Char_t* f,
                  const TTimeStamp& t, 
                  const TSnStatusUpdate& s,
                  const Int_t stvers) : TSnCommIncoming(t,s,stvers),
                                        fFilename(f),
                                        fNevtsRcvd(0), fNpwrsRcvd(0),
                                        fNevtsSent(0), fNpwrsSent(0),
                                        fMbedCRC(0), fLocalCRC(0),
                                        fFvers(0) {}
   TSnCommIncFile(const TString& f,
                  const TTimeStamp& t, 
                  const TSnStatusUpdate& s,
                  const Int_t stvers) : TSnCommIncoming(t,s,stvers),
                                        fFilename(f),
                                        fNevtsRcvd(0), fNpwrsRcvd(0),
                                        fNevtsSent(0), fNpwrsSent(0),
                                        fMbedCRC(0), fLocalCRC(0),
                                        fFvers(0) {}
   virtual ~TSnCommIncFile() {}
   
   const TString& GetFilename() const { return fFilename; }
   const Char_t*  GetFilenameStr() const { return fFilename.Data(); }
   UInt_t         GetNevtsRcvd() const { return fNevtsRcvd; }
   UInt_t         GetNpwrsRcvd() const { return fNpwrsRcvd; }
   UInt_t         GetNevtsSent() const { return fNevtsSent; }
   UInt_t         GetNpwrsSent() const { return fNpwrsSent; }
   UInt_t         GetMbedCRC()   const { return fMbedCRC; }
   UInt_t         GetLocalCRC()  const { return fLocalCRC; }
   UChar_t        GetFileVersion() const { return fFvers; }
   
   void           IncrementNevtsRcvd() { ++fNevtsRcvd; }
   void           IncrementNpwrsRcvd() { ++fNpwrsRcvd; }
   
   void           SetFilename(const TString& f) { fFilename = f; }
   void           SetFilename(const Char_t* f)  { fFilename = f; }
   void           SetNevtsRcvd(const UInt_t n)  { fNevtsRcvd = n; }
   void           SetNpwrsRcvd(const UInt_t n)  { fNpwrsRcvd = n; }
   void           SetNevtsSent(const UInt_t n)  { fNevtsSent = n; }
   void           SetNpwrsSent(const UInt_t n)  { fNpwrsSent = n; }
   void           SetMbedCRC(const UInt_t c)    { fMbedCRC = c; }
   void           SetLocalCRC(const UInt_t c)   { fLocalCRC = c; }
   void           SetFileVersion(const UChar_t v) { fFvers = v; }

   void           PrintEvts(Option_t* option="") const;
   void           PrintPwrs(Option_t* option="") const;
   void           PrintMbedCRC(Option_t* option="") const;
   void           PrintLocalCRC(Option_t* option="") const;
   virtual void   Print(Option_t* option="") const;
   
   ClassDef(TSnCommIncFile, 2); // info packet about an incoming file
};

#endif // SNS_TSnCommIncFile
