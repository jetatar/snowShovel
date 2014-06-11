#ifndef SNS_TSnCommMbedLocalFile
#define SNS_TSnCommMbedLocalFile

#ifndef ROOT_TString
#include <TString.h>
#endif

#include "TSnCommObject.h"

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommMbedLocalFile
//
// Class describing a local file to be stored on an MBED, such as a new MBED
// program or a DEFCONF.DAT
//
////////////////////////////////////////////////////////////////////////////////

class TSnCommMbedLocalFile : public TSnCommObject {
 private:
   TString      fFilename; // the name of the (original) file
   UInt_t       fLen;      // the number of bytes in the file
   Char_t*      fData;     //[fLen] the binary contents of the file (which may be deleted after storage in the queue)

   mutable UInt_t fChk;    //! checksum
   
   void LoadFile(const Char_t* infn);
   void Reset();

 protected:
   TSnCommMbedLocalFile(const TSnCommMbedLocalFile& m);
   
 public:
   TSnCommMbedLocalFile(const Char_t* infn=NULL);
   virtual ~TSnCommMbedLocalFile();
   
   static
   void          Swap(TSnCommMbedLocalFile& a, TSnCommMbedLocalFile& b);
   
   TSnCommMbedLocalFile& operator=(TSnCommMbedLocalFile o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }
   
   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const;

   const Char_t* GetFilename()  const { return fFilename; }
   const Char_t* GetData()      const { return fData; }
   UInt_t        GetDatLen()    const { return fLen; }
   UInt_t        GetDataLen()   const { return GetDatLen(); }
   
   void          CalcChecksum() const;
   UInt_t        GetChecksum()  const { return fChk; }

   TString       GetShortFilename() const 
      { return ConvertToShortFilename(fFilename.Data()); }

   virtual TString GetLabel() const { return GetShortFilename(); }

   virtual TSnCommObject* NewClone() const
      { return new TSnCommMbedLocalFile(*this); }

   virtual Bool_t  IsOkForStatusVersion(const Int_t sv) const;

   virtual Bool_t  IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const { return fFilename.Hash(); }
   virtual void    Print(Option_t* option="") const;
   virtual void    ls(Option_t *option="") const;
   
   static
   TString       ConvertToShortFilename(const TString lfn);
   
   ClassDef(TSnCommMbedLocalFile, 1); // a local file to be stored on an MBED (a new MBED program or a DEFCONF.DAT)
};

#endif // SNS_TSnCommMbedLocalFile
