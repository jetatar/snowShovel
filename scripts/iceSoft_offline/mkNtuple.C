#include <iostream>
#include <fstream>
#include <zlib.h>
#include <TTree.h>
#include <TString.h>
#include <TFile.h>

/*
**
** With the output tree, plots are easily generated like:
**
** root [5] nt.Draw("data03:Iteration$","EvEvtNum==25092")
**
*/

/*
// arianna0
static const UShort_t kNumChans   = 9;
static const UShort_t kNumSamples = 256;
static const UShort_t kNumScalers = 16;
static const UShort_t kMagicNum   = 57;
*/

// icicle
static const UShort_t kNumChans   = 4;
static const UShort_t kNumSamples = 128;
static const UShort_t kNumScalers = 16;
static const UShort_t kMagicNum   = 57;


/* - originals -
#define NUM_CHANNELS 9
#define NUM_SAMPLES 256
#define NUM_SCALERS 16
#define MAGIC_NUMBER 57
*/

//#define HITBUS_MASK 0x1000

typedef enum {
    kEventHeader=0x100,
    kEventBody=0x200,
    kCagesHk=0x201,
    kGpsZdaInfo=0x300,
    kGpsGgaInfo=0x301,
    kGpsGsvInfo=0x302
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


//CAGES Structures
struct AriannaHeader_t {
   GenericHeader_t gHdr;
   UInt_t eventNumber;
   UInt_t unixTime;
   UInt_t unixTimeUs;
   void Print() const {
      gHdr.Print();
      Printf("AHeader: EvtNum=%u, Time=%u, TimeUS=%u",
         eventNumber, unixTime, unixTimeUs);
   }
   void zero() {
      gHdr.zero();
      eventNumber = unixTime = unixTimeUs = 0;
   }
};

struct AriannaBody_t {
   GenericHeader_t gHdr;
   UInt_t   eventNumber;
   UShort_t data[kNumChans][kNumSamples];
   UShort_t extra[kMagicNum];
   void Print(const Bool_t withData=kFALSE) const {
      gHdr.Print();
      Printf("ABody: EvtNum=%u",eventNumber);
      if (withData) {
         printf("data=");
         for (UShort_t i=0; i<kNumChans; i++) {
            for (UShort_t j=0; j<kNumSamples; j++) {
               printf("%hu, ",data[i][j]);
            }
         }
         printf("\n");
      }
   }
   void zero() {
      gHdr.zero();
      eventNumber = 0;
      memset(data, 0, kNumChans*kNumSamples*sizeof(UShort_t));
      memset(extra, 0, kMagicNum*sizeof(UShort_t));
   }
};


//Global variables
AriannaHeader_t gHead;
AriannaBody_t gEvent;
gzFile gHdFile;
gzFile gEvFile;
TTree* nt;
TFile* outf;

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

gzFile openGzF(const Char_t* path, const Char_t* tag,
               const Int_t run, const Int_t seq) {
   // open a data file of a certain type (header or event)
   TString fnd  = Form("%s/run%d/event/%s_%d.dat",path,run,tag,seq);
   TString fndz = Form("%s.gz",fnd.Data());
   return openGzF(fndz.Data(), fnd.Data());
}

void openFiles(const Char_t* path, const Int_t run, const Int_t seq) {
   // open the global event & header files by tags
   gHdFile = openGzF(path, "hd", run, seq);
   gEvFile = openGzF(path, "ev", run, seq);
}

void openFiles(const Char_t* hdfn, const Char_t* evfn) {
   // open the global event & header files by name
   gHdFile = openGzF(hdfn);
   gEvFile = openGzF(evfn);
}

void closeFiles() {
   // close the global event & header files
   gzclose(gHdFile);
   gzclose(gEvFile);
}

template<class T>
Bool_t readNext(gzFile f, void* adr) {
   // read the next bunch of info from the file (either header or event)
   if (f!=0) {
      const Int_t nbr = gzread(f,adr,sizeof(T));
      //fprintf(stderr,"read [%d] bytes\n",nbr);
      if (nbr!=sizeof(T)) {
         if (nbr!=0) {
            fprintf(stderr,"readNext: Error when reading file for type [%s]. "
               "Got %d / %d bytes\n", typeid(T).name(), nbr, sizeof(T));
         }
         return kFALSE;
      }
      return kTRUE;
   }
   return kFALSE;
}

Bool_t readNextEvt() {
   // read the header and event info for the next event
   gHead.zero();
   gEvent.zero();
   const Bool_t hdok = readNext<AriannaHeader_t>(gHdFile,&gHead);
   const Bool_t evok = readNext<AriannaBody_t>(gEvFile,&gEvent);
   return (hdok || evok);
}




void mkNtuple(const Int_t run=1001, const Int_t seq=25000,
              const Char_t* outdir=
                 "/home/cjreed/work/dataNtuples/data2010-2011",
              const Char_t* outtag="nt.data2010-11",
              const Char_t* dpath=
                 "/home/cjreed/work/AriannaData/data2010-2011") {
   
   openFiles(dpath, run, seq);
   if (gHdFile==0 && gEvFile==0) return; // allow one to be missing
   
   TString outfn = Form("%s/%s.run%d.s%d.root",outdir,outtag,run,seq);
   outf = TFile::Open(outfn.Data(),"recreate");
   
   makeTree();
}

void mkNtuple(const Char_t* hdfn, const Char_t* evfn,
              const Char_t* outdir, const Char_t* outtag) {
   
   openFiles(hdfn, evfn);
   if (gHdFile==0 && gEvFile==0) return; // allow one to be missing
   
   TString outfn = Form("%s/%s.root",outdir,outtag);
   outf = TFile::Open(outfn.Data(),"recreate");
   
   makeTree();
}

void makeTree() {  
   TString hd;
   nt = new TTree("nt","nt");
   nt->Branch("mbChecksum",&gEvent.gHdr.checksum,"mbChecksum/i");
   nt->Branch("HdEvtNum",&gHead.eventNumber,"HdEvtNum/i");
   nt->Branch("EvtNum",&gEvent.eventNumber,"EvEvtNum/i");
   nt->Branch("unixTime",&gHead.unixTime,"unixTime/i");
   nt->Branch("unixTimeUS",&gHead.unixTimeUs,"unixTimeUS/i");
   for (UShort_t j=0; j<kNumChans; j++) {
      hd = Form("data%02hu",j);
      nt->Branch(hd.Data(),&(gEvent.data[j][0]),
         Form("%s[%hu]/s",hd.Data(),kNumSamples));
   }
   nt->Branch("extra",&(gEvent.extra[0]),Form("extra[%hu]/s",kMagicNum));
   nt->SetDirectory(outf);
   
   UInt_t e=0;
   while (readNextEvt()) {
      if ( (e++%20)==0 ) {
         fprintf(stderr,"Processing evt %u ....                \r",e);
      }
      //gEvent.Print(kTRUE);
      nt->Fill();
   }
   fprintf(stderr,"\n");
   Printf("got %u events",e);
   outf->Write();
   delete outf;
   closeFiles();
   
}












