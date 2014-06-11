#include <iostream>
#include <fstream>
#include <zlib.h>
#include <TTree.h>
#include <TString.h>
#include <TFile.h>
#include <TSystem.h>

#include <stdexcept>

static const UShort_t kNumAdcs = 16;

typedef enum {
  kEventHeader=0x100,
  kEventBody=0x200,
  kCagesHk=0x201,
  kGpsZdaInfo=0x300,
  kGpsGgaInfo=0x301,
  kGpsGsvInfo=0x302,
  kDiamondHk = 0x400,
  kDiamondHkBipol = 0x450
} BlockId_t;


struct GenericHeader_t {
   BlockId_t bid;
   UInt_t checksum;
   void Print() const {
      Printf("GHeader: Block=%x, ChkSum=%u",bid,checksum);
   }
   void zero() {
      bid = kCagesHk;
      checksum = 0;
   }
};

struct HkStruct_t {
   GenericHeader_t gHdr;  ///< The Generic header
   UInt_t unixTime; ///< Time
   UShort_t adcVal[kNumAdcs]; ///< Raw ADC values from diamond ADC
   void zero() {
      gHdr.zero();
      unixTime=0;
      memset(adcVal,0,kNumAdcs*sizeof(UShort_t));
   }
}; ///< The housekeeping structure

gzFile gHkFile;
HkStruct_t gHk;
TFile* outf;
TTree* nt;

void makeTree();

gzFile openGzF(const Char_t* fndz, const Char_t* fnd=0) {
   // open a gzip file, named 'fndz' or else 'fnd'
   // fndz is tried first
   
   gzFile f = gzopen(fndz,"r");
   if (f==0) {
      f = gzopen(fnd,"r");
      if (f==0) {
         fprintf(stderr,"openGzF: Could not open [%s] nor [%s]",
            fndz, fnd);
      } else {
         Printf("opened fnd=[%s]",fnd);
      }
   } else {
      Printf("opened fndz=[%s]",fndz);
   }
   return f;
}

void openFile(const Char_t* hkfn) {
   // open the global event & header files by name
   gHkFile = openGzF(gSystem->ExpandPathName(hkfn));
}

void closeFile() {
   // close the global event & header files
   gzclose(gHkFile);
}

template<class T>
Bool_t readNext(gzFile f, void* adr) {
   // read the next bunch of info from the file (either header or event)
   const Int_t nbr = gzread(f,adr,sizeof(T));
   //fprintf(stderr,"read [%d] bytes\n",nbr);
   if (nbr!=sizeof(T)) {
      if (nbr!=0) {
         fprintf(stderr,"readNext: Error when reading file for type [%s]. "
            "Got %d / %d bytes", typeid(T).name(), nbr, sizeof(T));
      }
      return kFALSE;
   }
   return kTRUE;
}

Bool_t readNextEvt(const BlockId_t expect) {
   // read the header and event info for the next event

   gHk.zero();
   
   Bool_t ok = readNext<HkStruct_t>(gHkFile,&gHk);

   if (ok && (gHk.gHdr.bid != expect)) {
      throw std::runtime_error(Form("Expect block id %x but found %x.",
                                    static_cast<Int_t>(expect),
                                    static_cast<Int_t>(gHk.gHdr.bid)));
      ok = kFALSE;
   }
   
   return ok;
}

void hkMkNtuple(const Char_t* hkfn,
                const Char_t* outdir, const Char_t* outtag) {
   
   openFile(hkfn);
   if (gHkFile==0) return;
   
   TString outfn = Form("%s/%s.root",outdir,outtag);
   outf = TFile::Open(outfn.Data(),"recreate");
   
   makeTree();
}

void makeTree() {  
   TString hd;
   nt = new TTree("hk","hk tree");

   Bool_t ok=kTRUE;
   gHk.zero();

   int pos = gztell( gHkFile );
   BlockId_t id;
   ok &= readNext<BlockId_t>(gHkFile, &id);
   gzseek( gHkFile, pos, SEEK_SET );

   nt->Branch( "checksum", &gHk.gHdr.checksum, "checksum/i" );
   nt->Branch( "unixTime", &gHk.unixTime, "unixTime/i" );

   for (UShort_t j=0; j<kNumAdcs; j++) {
      hd = Form("adc%02hu",j);
      if( id == kDiamondHk )
      {
         nt->Branch( hd.Data(), &(gHk.adcVal[j]), Form("%s/s",hd.Data()) );
      } else if( id == kDiamondHkBipol )
      {
         nt->Branch( hd.Data(), &(gHk.adcVal[j]), Form("%s/S",hd.Data()) );
      } else {
         throw std::runtime_error(Form("Expect block id either %x or %x, but "
                                       "found %x.",
                                       static_cast<Int_t>(kDiamondHk),
                                       static_cast<Int_t>(kDiamondHkBipol),
                                       static_cast<Int_t>(id)));
      }
   }
   nt->SetDirectory(outf);
   
   UInt_t e=0;
   while (readNextEvt(id)) {
      if ( (e++%20)==0 ) {
         fprintf(stderr,"Processing evt %u ....                \r",e);
      }
      nt->Fill();
   }

   fprintf(stderr,"\n");
   Printf("got %u events",e);
   outf->Write();
   delete outf;
   closeFile();
   
}


