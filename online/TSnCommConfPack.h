#ifndef SNS_TSnCommConfPack
#define SNS_TSnCommConfPack

#ifndef ROOT_TString
#include <TString.h>
#endif

#include "TSnCommObject.h"
#include "TSnIOBuffer.h"

class TSnDAQConfig;
class TSnRunInfo;
class TSnNetAdrSet;

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommConfPack
//
// Class describing a complete configuration to be received by a station.
// It is composed of a TSnDAQConfig, TSnRunInfo and TSnNetAdrSet.
//
// This class facilitates writing the complete configuration to a
// binary stream.
// Note: If any of the components are undefined, the NoConfig signal will be
//       written to the binary stream.
//
// This container owns its config components.
//
////////////////////////////////////////////////////////////////////////////////

class TSnCommConfPack : public TSnCommObject {
 private:
   TSnDAQConfig*    fDaqConf;
   TSnRunInfo*      fRunInf;
   TSnNetAdrSet*    fNetAdr;
   Int_t            fIOvers; // defaults to -1 => use default version of TSnIOConfigFrame. other values allow back compat

   UChar_t ReadFrom(const TSnIOBuffer& b, const Bool_t withHeader);
   
 protected:
   TSnCommConfPack(const TSnCommConfPack& cp);

 public:
   TSnCommConfPack() :
      fDaqConf(0), fRunInf(0), fNetAdr(0), fIOvers(-1) {}
   TSnCommConfPack(const Char_t* infn,
                   const Bool_t withHeader=kTRUE);
   TSnCommConfPack(const TSnIOBuffer& b,
                   const Bool_t withHeader);
   TSnCommConfPack(const TSnDAQConfig& dq,
                   const TSnRunInfo& ri,
                   const TSnNetAdrSet& na,
                   const Int_t iovers=-1);
   virtual ~TSnCommConfPack();

   static
   void           Swap(TSnCommConfPack& a, TSnCommConfPack& b);
   
   TSnCommConfPack& operator=(TSnCommConfPack o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }

   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const;

   virtual TSnCommObject* NewClone() const 
      { return new TSnCommConfPack(*this); }
   
   const TSnDAQConfig* GetDAQConfig() const { return fDaqConf; }
         TSnDAQConfig* GetDAQConfig()       { return fDaqConf; }
   const TSnRunInfo*   GetRunInfo() const   { return fRunInf; }
         TSnRunInfo*   GetRunInfo()         { return fRunInf; }
   const TSnNetAdrSet* GetNetAdrSet() const { return fNetAdr; }
         TSnNetAdrSet* GetNetAdrSet()       { return fNetAdr; }

   const Char_t*       GetConfLabel() const;

   virtual TString     GetLabel() const { return GetConfLabel(); }

   Bool_t              IsConfComplete() const
      { return ((fDaqConf!=0) && (fRunInf!=0) && (fNetAdr!=0)); }

   UChar_t             GetConfIOvers() const;
   Int_t               GetIOvers() const { return fIOvers; }
   void                SetIOvers(const Int_t v) { fIOvers = v; }

   virtual Bool_t  IsOkForStatusVersion(const Int_t sv) const;
   
   virtual Bool_t  IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const;
   virtual void    Print(Option_t* option="") const;
   virtual void    ls(Option_t* option="") const;
   
   ClassDef(TSnCommConfPack, 2); // config data to be sent to a station
};

#endif // SNS_TSnCommConfPack
