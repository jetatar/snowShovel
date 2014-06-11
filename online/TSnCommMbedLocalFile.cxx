#include "TSnCommMbedLocalFile.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sys/stat.h>

#include <TClass.h>
#include <TSystem.h>
#include <TROOT.h>

#include "TSnIOBuffer.h"
#include "NSnCRCUtils.h"
#include "TSnIOHeaderFrame.h"
#include "TSnIOStatusFrame.h"

//#define DEBUG

ClassImp(TSnCommMbedLocalFile);

TSnCommMbedLocalFile::TSnCommMbedLocalFile(const Char_t* infn) :
   fLen(0), fData(0), fChk(0) {
   if (infn!=NULL) {
      LoadFile(infn);
   }
}

TSnCommMbedLocalFile::TSnCommMbedLocalFile(const TSnCommMbedLocalFile& m) :
   TSnCommObject(m),
   fFilename(m.GetFilename()), fLen(m.GetDatLen()),
   fData(new Char_t[m.GetDatLen()]), fChk(0) {
   // deep copy ctor. checksum is recalculated; not copied.
   memcpy(fData, m.GetData(), sizeof(Char_t)*m.GetDataLen());
   CalcChecksum();
}

TSnCommMbedLocalFile::~TSnCommMbedLocalFile() {
   delete[] fData;
}

void TSnCommMbedLocalFile::Swap(TSnCommMbedLocalFile& a,
                                TSnCommMbedLocalFile& b) {
   std::swap(a.fFilename, b.fFilename);
   std::swap(a.fLen,      b.fLen);
   std::swap(a.fData,     b.fData);
   std::swap(a.fChk,      b.fChk);
}

Bool_t TSnCommMbedLocalFile::IsOkForStatusVersion(const Int_t sv) const {
   if (sv<TSnIOStatusFrame::kIOvers) {
      Warning("IsOkForStatusVersion",
              "Station reports status i/o version [%d] but current "
              "version is [%d]. I must assume this is ok, and you are "
              "trying to re-program the MBED. If you are changing the "
              "DEFCONF.DAT, you may be sending an incompatible config!",
              sv, TSnIOStatusFrame::kIOvers);
   }
   return kTRUE;
}

void TSnCommMbedLocalFile::LoadFile(const Char_t* infn) {
   fFilename = gSystem->ExpandPathName(infn);
#ifdef DEBUG
   Printf("loading file [%s]",fFilename.Data());
#endif
   struct stat results;
   if (stat(fFilename.Data(), &results)==0) {
      FILE* f = fopen(fFilename.Data(), "rb");
      if (f!=0) {
         fLen = results.st_size;
#ifdef DEBUG
         Printf("fLen=%u, st_size=%d",fLen,
                static_cast<Int_t>(results.st_size));
#endif
         delete[] fData;
         fData = new Char_t[fLen];
         const size_t br = fread(fData, fLen, 1u, f);
         if (1u==br) {
            CalcChecksum();
         } else {
            const TString err(Form("Read wrong number of elements "
                                   "(%u not 1) from [%s].",
                                   static_cast<UInt_t>(br), fFilename.Data()));
            Reset();
            throw std::runtime_error(err.Data());
         }
      } else {
         const TString err(Form("Could not open file [%s].",
                                fFilename.Data()));

         Reset();
         throw std::runtime_error(err.Data());
      }
   } else {
      const TString err(Form("Could not get size of file [%s].",
                             fFilename.Data()));
      Reset();
      throw std::runtime_error(err.Data());
   }
}

void TSnCommMbedLocalFile::CalcChecksum() const {
   fChk = NSnCRCUtils::GetCRC32for(fData, fLen);
}

TString TSnCommMbedLocalFile::ConvertToShortFilename(const TString lfn) {
   // remove any directory
   TString bfn( lfn(lfn.Last('/')+1, lfn.Length()) );
   bfn.ToUpper();
   const Ssiz_t dot = bfn.Last('.');
   TString sfn;
   for (Ssiz_t i=0, n=0; (i<dot) && (n<8); ++i) {
      const Char_t c = bfn[i];
      if (c!='.') {
         sfn += c;
         ++n;
      }
   }
   sfn += '.';
   const Ssiz_t bl = bfn.Length();
   if ( (bl-1-dot)==3 ) {
      sfn += bfn(dot+1, bl);
   } else {
      sfn += "dat";
   }
   return sfn;
}

UInt_t TSnCommMbedLocalFile::GetIOByteArraySize(const Bool_t withHeader) const {
   if (withHeader==kFALSE) {
      throw std::runtime_error("<TSnCommMbedLocalFile::GetIOByteArraySize>: "
                               "IO byte array for Mbed local file always has "
                               "headers.");
   }
   const UInt_t bs =
        TSnIOHeaderFrame::kMaxSizeOf + 12u  // filename (8.3 only!)
      + TSnIOHeaderFrame::kMaxSizeOf + fLen // file contents
      + TSnIOHeaderFrame::kMaxSizeOf;       // checksum
   return bs;
}

UInt_t TSnCommMbedLocalFile::WriteIOByteArrayTo(TSnIOBuffer& b) const {
   CalcChecksum();
   const UInt_t ob = b.GetPosVal();
   const TString shortfn(GetShortFilename());
   const Int_t sfl = shortfn.Length() + 1;
   Char_t* sfn = new Char_t[sfl];
   memset(sfn, 0, sizeof(Char_t)*sfl);
   strncpy(sfn, shortfn.Data(), shortfn.Length());
   TSnIOHeaderFrame::WriteTo(b, TSnIOHeaderFrame::kMbedFilenameCode, sfl);
   TSnIOObject::WriteStrTo(b, sfn, sfl);
   TSnIOHeaderFrame::WriteTo(b, TSnIOHeaderFrame::kMbedFileCode, fLen);
   TSnIOObject::WriteBytesTo(b, fData, fLen);
   TSnIOHeaderFrame::WriteTo(b, TSnIOHeaderFrame::kMbedFileChksCode, fChk);
   delete[] sfn;
   if (b.GetPosVal() < ob) {
      throw std::runtime_error("<TSnCommMbedLocalFile::WriteIOByteArrayTo>: "
                               "Position in buffer moved backwards.");
   }
   return (b.GetPosVal() - ob);
}

void TSnCommMbedLocalFile::Reset() {
   fFilename = "";
   fLen = 0;
   delete[] fData; fData = 0;
}

Bool_t TSnCommMbedLocalFile::IsEqual(const TObject* obj) const {
   // check file name, data length and checksum
   if (obj!=0) {
      if (obj->IsA() == TSnCommMbedLocalFile::Class()) {
         const TSnCommMbedLocalFile* l = 
            static_cast<const TSnCommMbedLocalFile*>(obj);
         if (0==fFilename.CompareTo(l->GetFilename())) {
            if (fLen==l->GetDatLen()) {
               if (GetChecksum()==l->GetChecksum()) {
                  return kTRUE;
               }
            }
         }
      }
   }
   return kFALSE;
}

void TSnCommMbedLocalFile::Print(Option_t* option) const {
   CalcChecksum();
   Printf("TSnCommMbedLocalFile: ");
   Printf("  filename  = [%s]", fFilename.Data());
   Printf("  shortened = [%s]", GetShortFilename().Data());
   Printf("  num bytes = %u", fLen);
   Printf("  checksum  = %u", fChk);
}

void TSnCommMbedLocalFile::ls(Option_t* option) const {
   TROOT::IndentLevel();
   std::cout << "OBJ: " << IsA()->GetName() << "\t" << fFilename.Data()
      << std::endl;
}
