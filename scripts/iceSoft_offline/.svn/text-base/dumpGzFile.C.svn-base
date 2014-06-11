#include <cstdio>
#include <zlib.h>
#include <TString.h>
#include <TSystem.h>

/*
// arianna0
static const UShort_t kNumChans   = 9;
static const UShort_t kNumSamples = 256;
static const UShort_t kNumScalers = 16;
static const UShort_t kMagicNum   = 57;
static const UShort_t kNumAdcs    = 16;
*/

// icicle
static const UShort_t kNumChans   = 4;
static const UShort_t kNumSamples = 128;
static const UShort_t kNumScalers = 16;
static const UShort_t kMagicNum   = 57;
static const UShort_t kNumAdcs    = 16;


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
   void dump(FILE* s) {
      if (s!=0) {
         fprintf(s, "ghdr: %#x\t%u\n",bid,checksum);
      }
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
   void dump(FILE* s) {
      if (s!=0) {
         gHdr.dump(s);
         fprintf(s,"ahdr: %u\t%u\t%u\n",eventNumber,unixTime,unixTimeUs);
      }
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
   void dump(FILE* s) {
      if (s!=0) {
         gHdr.dump(s);
         fprintf(s,"body: %u\n",eventNumber);
         for (Int_t ch=0; ch<kNumChans; ch++) {
            fprintf(s,"ch%02d: ",ch);
            for (Int_t sm=0; sm<kNumSamples; sm++) {
               fprintf(s,"%4hu\t",data[ch][sm]);
            }
            fprintf(s,"\n");
         }
         fprintf(s, "extr: ");
         for (Int_t mn=0; mn<kMagicNum; mn++) {
            fprintf(s,"%hu\t",extra[mn]);
         }
         fprintf(s,"\n");
      }
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
   void dump(FILE* s) {
      if (s!=0) {
         gHdr.dump(s);
         fprintf(s,"hkst: %u\t",unixTime);
         for (Int_t na=0; na<kNumAdcs; na++) {
            fprintf(s,"%hu\t",adcVal[na]);
         }
         fprintf(s,"\n");
      }
   }
}; ///< The housekeeping structure

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


enum EGZType {
   kHD=0,
   kEV=1,
   kHK=2
};


//Global variables
AriannaHeader_t gHead;
AriannaBody_t gEvent;
HkStruct_t gHk;

gzFile inf;



void dumpGzFile(const Char_t* infn, const Char_t* outfn,
                const EGZType type) {
   // dump a dat.gz file into a text file
   
   
   inf = gzopen(gSystem->ExpandPathName(infn),"r");
   if (inf!=0) {
      
      FILE* outf = fopen(gSystem->ExpandPathName(outfn),"w");
      if (outf!=0) {
         if        (type==kHD) {
            while (readNext<AriannaHeader_t>(inf, &gHead)) {
               gHead.dump(outf);
            }
         } else if (type==kEV) {
            while (readNext<AriannaBody_t>(inf, &gEvent)) {
               gEvent.dump(outf);
            }
         } else if (type==kHK) {
            while (readNext<HkStruct_t>(inf, &gHk)) {
               gHk.dump(outf);
            }
         } else {
            Printf("Unknown type [%d]",static_cast<Int_t>(type));
         }

         fclose(outf);
      } else {
         Printf("Could not open output file [%s]",outfn);
      }
      gzclose(inf);
   } else {
      Printf("Could not open file [%s]",infn);
   }
   
}

