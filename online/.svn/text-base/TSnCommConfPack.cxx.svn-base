#include "TSnCommConfPack.h"

#include <TClass.h>
#include <TSystem.h>
#include <TROOT.h>

#include "TSnDAQConfig.h"
#include "TSnRunInfo.h"
#include "TSnNetAdrSet.h"
#include "TSnIOBuffer.h"
#include "TSnIOHeaderFrame.h"
#include "TSnIOConfigFrame.h"
#include "TSnIOStatusFrame.h"
#include "NSnHashUtils.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

UChar_t TSnCommConfPack::GetConfIOvers() const {
   return (fIOvers<0) ? TSnIOConfigFrame::kIOvers
      : static_cast<UChar_t>(fIOvers);
}

TSnCommConfPack::TSnCommConfPack(const Char_t* infn,
                                 const Bool_t withHeader/*=kTRUE*/) :
   fDaqConf(0), fRunInf(0), fNetAdr(0), fIOvers(-1) {
   // build a conf pack from the specified file

   const TString fn(gSystem->ExpandPathName(infn));
   FILE* inf = fopen(fn.Data(), "rb");
   if (inf!=0) {
      // TODO: make it smarter than reading the entire file?
      TSnIOBuffer b(inf);
      fIOvers = static_cast<Int_t>( ReadFrom(b, withHeader) );
   } else {
      throw std::runtime_error(Form("Could not open file [%s].", fn.Data()));
   }
}

TSnCommConfPack::TSnCommConfPack(const TSnIOBuffer& b,
                                 const Bool_t withHeader) :
   fDaqConf(0), fRunInf(0), fNetAdr(0), fIOvers(-1) {
   // build a config from the i/o buffer
   fIOvers = static_cast<Int_t>( ReadFrom(b, withHeader) );
}


TSnCommConfPack::TSnCommConfPack(const TSnDAQConfig& dq,
                                 const TSnRunInfo& ri,
                                 const TSnNetAdrSet& na,
                                 const Int_t iovers) :
   fDaqConf(new TSnDAQConfig(dq)), 
   fRunInf(new TSnRunInfo(ri)), 
   fNetAdr(new TSnNetAdrSet(na)),
   fIOvers(iovers) {
   // copy the config components
}

TSnCommConfPack::TSnCommConfPack(const TSnCommConfPack& cp) :
   TSnCommObject(cp),
   fDaqConf(0), fRunInf(0), fNetAdr(0), fIOvers(cp.GetIOvers()) {
   // copy the config components
   
   const TSnDAQConfig* c = cp.GetDAQConfig();
   if (c!=0) {
      fDaqConf = new TSnDAQConfig(*c);
   }
   const TSnRunInfo* r = cp.GetRunInfo();
   if (r!=0) {
      fRunInf = new TSnRunInfo(*r);
   }
   const TSnNetAdrSet* n = cp.GetNetAdrSet();
   if (n!=0) {
      fNetAdr = new TSnNetAdrSet(*n);
   }
}

TSnCommConfPack::~TSnCommConfPack() {
   delete fDaqConf;
   delete fRunInf;
   delete fNetAdr;
}

void TSnCommConfPack::Swap(TSnCommConfPack& a, TSnCommConfPack& b) {
   std::swap(a.fDaqConf, b.fDaqConf);
   std::swap(a.fRunInf,  b.fRunInf);
   std::swap(a.fNetAdr,  b.fNetAdr);
   std::swap(a.fIOvers,  b.fIOvers);
}

Bool_t TSnCommConfPack::IsOkForStatusVersion(const Int_t sv) const {
   
   if (IsConfComplete()) {
      /* a season stack goes here
       * if (sv > 2018to2019IOvers) {
       * } else if (sv > 2017to2018IOvers) {
       *  ...
       * } else if (sv > 2012to2013IOvers) {
       * } else if (sv == 2012to2013IOvers) {
       * } else {
       *    prior to any season
       */
      if (sv > TSnIOStatusFrame::k2012to2013IOVers) {
         return ( GetConfIOvers()  > TSnIOConfigFrame::k2012to2013IOVers );
      } else if (sv == TSnIOStatusFrame::k2012to2013IOVers) {
         return ( GetConfIOvers() == TSnIOConfigFrame::k2012to2013IOVers );
      } else {
         // really old
         return ( GetConfIOvers() < TSnIOConfigFrame::k2012to2013IOVers );
      }
   } else {
      // the no new config signal is always ok
      return kTRUE;
   }
}

UInt_t TSnCommConfPack::GetIOByteArraySize(const Bool_t withHeader) const {
   const UInt_t s = (withHeader) ? TSnIOHeaderFrame::kMaxSizeOf : 0;
   const UInt_t c = (fDaqConf!=0) 
      ? TSnIOConfigFrame::SizeOf(GetConfIOvers(), *fDaqConf)
      : TSnIOConfigFrame::kMaxSizeOf;
   return (s+c);
}

UChar_t TSnCommConfPack::ReadFrom(const TSnIOBuffer& b, 
                                  const Bool_t withHeader) {
   // build the config pack from the buffer
   // private so that it can only be called by constructors
   // (intentionally no re-assignment of configs)
   
   //const UInt_t bp = b.GetPosVal();
   Bool_t isCf = kTRUE;
   UChar_t Rv(0);
   if (withHeader) {
      UChar_t mcode(0);
      UInt_t  mlen(0);
      TSnIOHeaderFrame::ReadFrom(b, mcode, mlen);
      isCf = (mcode == TSnIOHeaderFrame::kConfigCode);
   }
   if (isCf) {
      fDaqConf = new TSnDAQConfig;
      fRunInf  = new TSnRunInfo;
      fNetAdr  = new TSnNetAdrSet;
      Rv = TSnIOConfigFrame::ReadFrom(b, *fDaqConf, *fRunInf, *fNetAdr);
   }
   /*
   if ( (b.GetPosVal() - bp) != GetIOByteArraySize(withHeader) ) {
      throw std::runtime_error("Could not get configuration pack "
                               "from buffer.");
   }
   */
   return Rv;
}

UInt_t TSnCommConfPack::WriteIOByteArrayTo(TSnIOBuffer& b) const {
   const UInt_t ob = b.GetPosVal();
   if ( IsConfComplete() ) {
      const UChar_t iov = GetConfIOvers();
      TSnIOHeaderFrame::WriteTo(b, TSnIOHeaderFrame::kConfigCode, 
                                   TSnIOConfigFrame::SizeOf(iov, *fDaqConf));
      TSnIOConfigFrame::WriteTo(b, *fDaqConf, *fRunInf, *fNetAdr, iov);
   } else {
      throw std::runtime_error("<TSnCommConfPack::WriteIOByteArrayTo>: "
                               "Can't write incomplete config.");
      /*
      // the no config signal
      TSnIOHeaderFrame::WriteTo(b, TSnIOHeaderFrame::kNoConfigCode, 0);
      */
   }
   if (b.GetPosVal() < ob) {
      throw std::runtime_error("<TSnCommConfPack::WriteIOByteArrayTo>: "
                               "Position in buffer moved backwards.");
   }
   return (b.GetPosVal() - ob);
}

const Char_t* TSnCommConfPack::GetConfLabel() const {
   // get the config label.
   // returns NULL if the config pack is incomplete.
   
   if ( IsConfComplete() ) {
      return fDaqConf->GetLabel();
   }
   return NULL;
}

Bool_t  TSnCommConfPack::IsEqual(const TObject* obj) const {
   if (obj!=0) {
      if (obj->IsA() == TSnCommConfPack::Class()) {
         const TSnCommConfPack* cf = static_cast<const TSnCommConfPack*>(obj);
         Bool_t same(kTRUE);
         if (fDaqConf!=0) {
            same = fDaqConf->IsEqual(cf->GetDAQConfig());
         }
         if ( same && (fRunInf!=0) ) {
            same = fRunInf->IsEqual(cf->GetRunInfo());
         }
         if ( same && (fNetAdr!=0) ) {
            same = fNetAdr->IsEqual(cf->GetNetAdrSet());
         }
         if ( same ) {
            same = GetConfIOvers() == cf->GetConfIOvers();
         }
         return same;
      }
   }
   return kFALSE;
}

ULong_t TSnCommConfPack::Hash() const {
   if ( IsConfComplete() ) {
      ULong_t h = fDaqConf->Hash();
      NSnHashUtils::HashCombine(h, *fRunInf);
      NSnHashUtils::HashCombine(h, *fNetAdr);
      return h;
   } else {
      return TObject::Hash();
   }
}

void TSnCommConfPack::Print(Option_t* option) const {
   TString opt(option);
   opt.ToUpper();
   if (opt.Contains("LABEL")) {
      if (IsConfComplete()==kFALSE) {
         Printf("No new config");
      } else {
         Printf("%s",fDaqConf->GetLabel());
      }
   } else {
      if (fRunInf!=0) {
         fRunInf->Print(option);
      } else {
         Printf("No run info.");
      }

      if (fDaqConf!=0) {
         fDaqConf->Print(option);
      } else {
         Printf("No DAQ config.");
      }
      if (fNetAdr!=0) {
         fNetAdr->Print(option);
      } else {
         Printf("No network address set.");
      }
      if (IsConfComplete()==kFALSE) {
         Printf("Will send station the 'no new config' signal.");
      }
   }
   Printf("Config Frame IO version [%hhu]", GetConfIOvers());
}

void TSnCommConfPack::ls(Option_t* option) const {
   TROOT::IndentLevel();
   TString conflbl("No Config");
   if ( IsConfComplete() ) {
      conflbl = fDaqConf->GetLabel();
   }
   std::cout << "OBJ: " << IsA()->GetName() << "\t" << 
      conflbl.Data() << std::endl;
}
