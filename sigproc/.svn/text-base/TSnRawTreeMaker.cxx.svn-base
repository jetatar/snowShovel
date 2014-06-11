#include "TSnRawTreeMaker.h"

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>

#include "TSnIODataFile.h"
#include "TSnIOHeaderFrame.h"
#include "TSnIOHeartbeatFrame.h"
#include "NSnConstants.h"
#include "TSnDAQConfig.h"
#include "TSnRunInfo.h"
#include "TSnNetAdrSet.h"
#include "TSnCompMbed.h"
#include "TSnEvent.h"
#include "TSnPowerReading.h"
#include "TSnHeartbeat.h"
#include "TSnClockSetInfo.h"
#include "TSnEventMetadata.h"

#include <stdexcept>

ClassImp(TSnRawTreeMaker);

const Char_t* TSnRawTreeMaker::kRTFprf    = "RawTree.";

const Char_t* TSnRawTreeMaker::kVTname    = "VoltageTree";
const Char_t* TSnRawTreeMaker::kHTname    = "HeartbeatTree";
const Char_t* TSnRawTreeMaker::kETname    = "RawEventTree";
const Char_t* TSnRawTreeMaker::kCTname    = "ConfigTree";

// branches must have unique names across trees so that the loaders
// can tell when they need to be used based on the branch name alone
const Char_t* TSnRawTreeMaker::kPwrBrNm   = "PowerReading.";
const Char_t* TSnRawTreeMaker::kHrtBrNm   = "Heartbeat.";
const Char_t* TSnRawTreeMaker::kEMtBrNm   = "EventMetadata.";
const Char_t* TSnRawTreeMaker::kEHdBrNm   = "EventHeader.";
const Char_t* TSnRawTreeMaker::kEWvBrNm   = "RawData.";
const Char_t* TSnRawTreeMaker::kCMtBrNm   = "ConfigMetadata.";
const Char_t* TSnRawTreeMaker::kCnfBrNm   = "DAQConfig.";
const Char_t* TSnRawTreeMaker::kRunBrNm   = "RunInfo.";
const Char_t* TSnRawTreeMaker::kNetBrNm   = "NetAdrSet.";
const Char_t* TSnRawTreeMaker::kCSrBrNm   = "TrigStartClock.";
const Char_t* TSnRawTreeMaker::kCSpBrNm   = "TrigStopClock.";

TSnRawTreeMaker::TSnRawTreeMaker(const Char_t* infn,
                                 const Char_t* outdir,
                                 const Char_t* outpre,
                                 const Int_t   brBufSize,
                                 const Int_t   split,
                                 const Int_t   compr)
      : fInfn(infn), fOdir(outdir), fOpre(outpre), 
        fBufsz(brBufSize), fSplit(split), fCompr(compr),
        fInf(0), fOutf(0), 
        fVt(0), fHt(0), fCt(0), fEt(0),
        fEvt(new TSnEvent), fPwr(new TSnPowerReading),
        fHrt(new TSnHeartbeat), fEvtMeta(new TSnEventMetadata),
        fConf(new TSnDAQConfig), fRunf(new TSnRunInfo),
        fNets(new TSnNetAdrSet), fHrun(new TSnRunInfo),
        fComp(new TSnCompMbed), fSeq(0),
        fClkStrt(new TSnClockSetInfo), fClkStop(new TSnClockSetInfo) {
}

TSnRawTreeMaker::~TSnRawTreeMaker() {
   if (fInf!=0) {
      fclose(fInf);
   }
   delete fOutf; // this deletes the trees
   
   delete fEvt;
   delete fPwr;
   delete fHrt;
   delete fEvtMeta;
   delete fConf;
   delete fRunf;
   delete fNets;
   delete fHrun;
   delete fComp;
   delete fClkStrt;
   delete fClkStop;
}

TString TSnRawTreeMaker::GetTreeFilename() const {
   TString ifn = fInfn.Data() + fInfn.Last('/') + 1;
   TString o(Form("%s/%s%s",
                  fOdir.Data(),fOpre.Data(),ifn.Data()));
   o.Remove(o.Last('.'));
   o += ".root";
   return o;
}

Long64_t TSnRawTreeMaker::GetNevtsWritten() const {
   return (fEt!=0) ? (fEt->GetEntries()) : 0;
}

Long64_t TSnRawTreeMaker::GetNpwrsWritten() const {
   return (fVt!=0) ? (fVt->GetEntries()) : 0;
}

Long64_t TSnRawTreeMaker::GetNhrtsWritten() const { 
   return (fHt!=0) ? (fHt->GetEntries()) : 0;
}

Bool_t TSnRawTreeMaker::OpenFiles() {
   
   if (fInf!=0) {
      fclose(fInf);
   }
   fInf = fopen(gSystem->ExpandPathName(fInfn.Data()),"rb");
   if ( (fInf!=0) && (ferror(fInf)==0) ) {
      
      delete fOutf;
      TString ofn( GetTreeFilename() );
      fOutf = TFile::Open(ofn.Data(),"RECREATE");
      if ( (fOutf!=0) && (fOutf->IsZombie()==kFALSE) ) {
         fOutf->SetCompressionLevel(fCompr);
         return kTRUE;
      } else {
         throw std::runtime_error(Form("<TSnRawTreeMaker::OpenFiles>: "
                                       "Could not open output file [%s].",
                                       ofn.Data()));
      }
      
   } else {
      throw std::runtime_error(Form("<TSnRawTreeMaker::OpenFiles>: "
                                    "Could not open input file [%s].",
                                    fInfn.Data()));
   }
   
   return kFALSE;
}

void TSnRawTreeMaker::MakeTrees() {
   
   // volt tree
   delete fVt;
   fVt = new TTree(kVTname,"voltage readings");
   fVt->Branch(kPwrBrNm, &fPwr, fBufsz, fSplit);
   fVt->SetDirectory(fOutf);
  
   // heartbeat tree
   delete fHt;
   fHt = new TTree(kHTname,"heartbeat triggers");
   fHt->Branch(kHrtBrNm, &fHrt, fBufsz, fSplit);
   fHt->SetDirectory(fOutf);
   
   // config tree
   delete fCt;
   fCt = new TTree(kCTname,"configuration parameters");
   fCt->Branch(kCMtBrNm, &fEvtMeta, fBufsz, fSplit);
   fCt->Branch(kCnfBrNm, &fConf,    fBufsz, fSplit);
   fCt->Branch(kRunBrNm, &fRunf,    fBufsz, fSplit);
   fCt->Branch(kNetBrNm, &fNets,    fBufsz, fSplit);
   fCt->Branch(kCSrBrNm, &fClkStrt, fBufsz, fSplit);
   fCt->Branch(kCSpBrNm, &fClkStop, fBufsz, fSplit);
   fCt->SetDirectory(fOutf);
   
   // event tree
   delete fEt;
   fEt = new TTree(kETname,"uncalibrated waveform events");
   fEt->Branch(kEMtBrNm, &fEvtMeta,                 fBufsz, fSplit);
   fEt->Branch(kEHdBrNm, &(fEvt->GetHeader()),      fBufsz, fSplit);
   fEt->Branch(kEWvBrNm, &(fEvt->GetRawWaveform()), fBufsz, fSplit);
   fEt->SetDirectory(fOutf);
   // some useful aliases
   TString an;
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      // to use as a cut for a particular channel:
      an = Form("Ch%d",ch);
      fEt->SetAlias(an.Data(),
                 Form("(Iteration$>=(%hhu*%hhu)) && (Iteration$<(%hhu*%hhu))",
                      NSnConstants::kNsamps, ch,
                      NSnConstants::kNsamps,
                      static_cast<UChar_t>(ch+1)));
      // to use as a variable showing the sample number [0,127] for any chan
      an = Form("SmpCh%d",ch);
      fEt->SetAlias(an.Data(),
                    Form("Iteration$-%u", static_cast<UInt_t>(ch)
                         *static_cast<UInt_t>(NSnConstants::kNsamps)));
      // e.g. Draw("RawData.fData:SmpCh2","EventHeader.fNum==21 && Ch2","l")
   }
   
}

void TSnRawTreeMaker::ProcFile() {
   
   if (OpenFiles()) {
      
      MakeTrees();
      
      Int_t fvers;
      const UInt_t bytes = TSnIODataFile::ReadFileHeader(fInf, 
                                                         *fHrun, *fComp, fSeq,
                                                         fPwr, fvers);
      if (bytes>0) {
         fEvtMeta->SetStationId(fComp->GetMAC());
         fEvtMeta->SetRunNum(fHrun->GetRun());
         fEvtMeta->SetSeqNum(fSeq);
         
         // loop over what's in the file
         LoopBlocks();
         
         // fill the config tree once per file
         fCt->Fill();
         
         // save it
         fOutf->Write();
         
      } else {
         throw std::runtime_error(Form("<TSnRawTreeMaker::ProcFile>: "
                                       "Could not read file header from "
                                       "[%s].", fInfn.Data()));
      }
      
   } else {
      Error("ProcFile","OpenFiles failed. Can't process.");
   }
   
}

void TSnRawTreeMaker::LoopBlocks() {
   
   UChar_t hcode; // block code (see TSnIOHeaderFrame) 
   UInt_t  hlen;  // block length
   UInt_t  bytes;
   Bool_t  gotConf=kFALSE;
   do {
      bytes = TSnIODataFile::ReadBlockHeader(fInf, hcode, hlen);
      if (bytes>0) {
         if (hcode==TSnIOHeaderFrame::kEventCode) {
            // an event
            if (gotConf) {
               bytes = TSnIODataFile::ReadEvent(fInf, *fConf, *fEvt);
               fEt->Fill();
            } else {
               throw std::runtime_error(Form("<TSnRawTreeMaker::LoopBlocks>: "
                                             "Cannot read event before "
                                             "config!"));
            }
         } else if (hcode==TSnIOHeaderFrame::kPowerCode) {
            // a power reading
            bytes = TSnIODataFile::ReadPower(fInf, *fPwr);
            fVt->Fill();
         } else if (hcode==TSnIOHeaderFrame::kHeartbeatCode) {
            // a heartbeat
            bytes = TSnIODataFile::ReadHeartbeat(fInf, *fHrt,
               (hlen==TSnIOHeartbeatFrame::SizeOf(1)) );
            // the above is a guess at whether the heartbeat needs
            // to be read with the special V1 reader (as V1 did
            // not write out the i/o version). ugh!
            fHt->Fill();
         } else if (hcode==TSnIOHeaderFrame::kConfigCode) {
            // a DAQ configuration
            if (gotConf==kFALSE) {
               bytes = TSnIODataFile::ReadConfig(fInf, *fConf, *fRunf, *fNets);
               if (fRunf->GetRun()==fHrun->GetRun()) {
                  fRunf->SetStationLabel(fComp->GetMAC());
                  gotConf = kTRUE;
               } else {
                  throw std::runtime_error(
                     Form("<TSnRawTreeMaker::LoopBlocks>: "
                          "Run number mismatch! Header "
                          "says [%u] while config says [%u]",
                          fHrun->GetRun(), fRunf->GetRun()));
               }
            } else {
               throw std::runtime_error(Form("<TSnRawTreeMaker::LoopBlocks>: "
                                             "Cannot read multiple "
                                             "configurations!"));
            }
         } else if (hcode==TSnIOHeaderFrame::kFileTrgStrtCode) {
            bytes = TSnIODataFile::ReadClockSet(fInf, *fClkStrt);
         } else if (hcode==TSnIOHeaderFrame::kFileTrgStopCode) {
            bytes = TSnIODataFile::ReadClockSet(fInf, *fClkStop);
         } else {
            throw std::runtime_error(Form("<TSnRawTreeMaker::LoopBlocks>: "
                                          "Unknown block with header code "
                                          "[%#02hhx]",hcode));
         } // end block types
      } // if read block header
   } while ( (feof(fInf)==0) && (ferror(fInf)==0) && (bytes>0) );
   // TSnIOHeaderFrame will refuse to read past the
   // end of the file, so the EOF flag never gets set.
   // instead, 0 bytes gets returned
   
}
