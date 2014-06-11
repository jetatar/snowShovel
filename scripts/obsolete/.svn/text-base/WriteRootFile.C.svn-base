#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

#include <cstdio>
#include <stdexcept>
#include <iostream>

#include <TSnDAQConfig.h>
#include <TSnRunInfo.h>
#include <TSnIODataFile.h>
#include <TSnIOHeaderFrame.h>
#include <TSnCompMbed.h>
#include <NSnConstants.h>
#include <TSnEvent.h>
#include <TSnPowerReading.h>
#include <TSnNetAdrSet.h>
#endif

/**
 gSystem->Load("$SNS/lib/libdbdat.so"); gSystem->Load("$SNS/lib/libonline.so");
 */

void WriteRootFile(const Char_t* otfn,
                   const Char_t* infn,
                   const Bool_t  printConf=kFALSE) {
  
   TFile* rootfile = new TFile(otfn,"RECREATE");
   rootfile->cd();
   
   TSnDAQConfig      conf;
   TSnRunInfo        runf;
   TSnCompMbed       comp;
   TSnEvent          evnt;
   UShort_t          seq;
   TSnPowerReading   pow;
   TSnNetAdrSet      nets;
   
   // the voltage reading tree
   Float_t av1, av2, rm1, rm2;
   UInt_t vtime;
   TTree* voltt = new TTree("volttree","volttree");
   voltt->SetDirectory(rootfile);
   voltt->Branch("aveV1",&av1,"aveV1/F");
   voltt->Branch("aveV2",&av2,"aveV2/F");
   voltt->Branch("rmsV1",&rm1,"rmsV1/F");
   voltt->Branch("rmsV2",&rm2,"rmsV2/F");
   voltt->Branch("vtime",&vtime,"vtime/i");
   voltt->SetDirectory(rootfile);
   
   // the heartbeat tree
   TTimeStamp* hbtimestamp = new TTimeStamp;
   UInt_t hbnum;
   TTree* hbtree = new TTree("hbtree","heartbeats");
   hbtree->Branch("hbtime","TTimeStamp",&hbtimestamp);
   hbtree->Branch("hbnum",&hbnum,"hbnum/i");
   hbtree->SetDirectory(rootfile);
   
   // the event tree
   UInt_t fStationCRC;
   UInt_t eventNumber;
   UInt_t unixTime;
   UInt_t unixTimeUs = 0;
   UShort_t data[NSnConstants::kNchans][NSnConstants::kNsamps];
   UShort_t extra[57];
   Int_t dtms;
   memset(extra,0,57*sizeof(UShort_t));
   TTree *runtree = new TTree("runtree","runtree");
   runtree->SetDirectory(rootfile);
   runtree->Branch("mbChecksum", &fStationCRC, "mbChecksum/i"); 
   runtree->Branch("HdEvtNum",&eventNumber,"HdEvtNum/i");
   runtree->Branch("EvtNum",&eventNumber,"EvEvtNum/i");
   runtree->Branch("unixTime",&unixTime,"unixTime/i");
   runtree->Branch("unixTimeUS",&unixTimeUs,"unixTimeUS/i");
   runtree->Branch("DTms",&dtms,"DTms/I");
   TString hd;
   for (UShort_t j=0; j<NSnConstants::kNchans; j++) {
      hd = Form("data%02hu",j);
      runtree->Branch(hd.Data(),&(data[j][0]),
		 Form("%s[%hu]/s",hd.Data(),NSnConstants::kNsamps));
   }
   runtree->Branch("extra",&(extra[0]),"extra[57]/s");
   runtree->SetDirectory(rootfile);
   
   union {
     UInt_t i;
     UShort_t s[2];
     UChar_t c[4];
   } tmp;
   const Int_t kMbedTimeIdxHi = 0;
   const Int_t kMbedTimeIdxLo = 1;
   const Int_t kTrgWordIdx    = 10;
   const Int_t kCpuCrcIdxHi   = 20;
   const Int_t kCpuCrcIdxLo   = 21;
   const Int_t kRunIdxHi      = 30;
   const Int_t kRunIdxLo      = 31;
   const Int_t kSeqIdx        = 35;

   FILE* inf = fopen(gSystem->ExpandPathName(infn),"rb");
   if (inf!=0) {
      try {
         //cerr << "reading file header" << endl;
	 UInt_t bytes = 
            TSnIODataFile::ReadFileHeader(inf, runf, comp, seq, &pow);

         //cerr << "reading block header" << endl;
         UChar_t hcode; // block code (see TSnIOHeaderFrame) 
         UInt_t  hlen;  // block length
         TSnIODataFile::ReadBlockHeader(inf, hcode, hlen);
         // probably a power reading
         if (hcode==TSnIOHeaderFrame::kPowerCode) {
            //cerr << "reading power" << endl;
            TSnIODataFile::ReadPower(inf, pow);
            av1 = pow.GetAveV1();
            av2 = pow.GetAveV2();
            rm1 = pow.GetRmsV1();
            rm2 = pow.GetRmsV2();
            vtime = pow.GetTime();
            voltt->Fill();
            
            // read new header
            //cerr << "reading new header" << endl;
            TSnIODataFile::ReadBlockHeader(inf, hcode, hlen);
         }
         
         // get the config
         while (hcode!=TSnIOHeaderFrame::kConfigCode
                && (feof(inf)==0)
                && (ferror(inf)==0) ) {
            //cerr << "skipping to config" << endl;
            fseek(inf, hlen, SEEK_CUR); // skip this unwanted block
            TSnIODataFile::ReadBlockHeader(inf, hcode, hlen);
         }
         //cerr << "reading config" << endl;
         const Int_t cbr = TSnIODataFile::ReadConfig(inf, conf, runf, nets);
         if (cbr>0) {
            if (printConf) {
               runf.Print();
               conf.Print();
               nets.Print();
            }
            TDirectory* od = gDirectory;
            rootfile->cd();
            runf.Write("RunInfo");
            conf.Write("DAQConfig");
            nets.Write("NetAdrSet");
            od->cd();
            
            UInt_t eventsread=0, voltsread=0, hbread=0;
            
            while ( (feof(inf)==0) && (ferror(inf)==0) ) {
               //cerr << "reading block header" << endl;
               bytes = TSnIODataFile::ReadBlockHeader(inf, hcode, hlen);
               if (bytes>0) {
                  if (hcode==TSnIOHeaderFrame::kEventCode) {
                     //cerr << "reading event!" << endl;
                     bytes = TSnIODataFile::ReadEvent(inf, conf, evnt);
                     // fill the event tree
                     fStationCRC = evnt.GetStationCRC();
	             eventNumber = evnt.GetEvtNum();
                     unixTime    = evnt.GetTime().GetSec();
                     for (UShort_t j=0; j<NSnConstants::kNchans; j++) {
	               memcpy(&(data[j][0]),evnt.GetWvData(j),
                              NSnConstants::kNsamps*sizeof(UShort_t));
	             }
                     tmp.i = evnt.GetTime().GetSec();
	             extra[kMbedTimeIdxHi] = tmp.s[0];
	             extra[kMbedTimeIdxLo] = tmp.s[1];
	             extra[kTrgWordIdx]    = evnt.GetTrgInfo().GetTrgBits();
	             tmp.i = evnt.GetCRC();
	             extra[kCpuCrcIdxHi]   = tmp.s[0];
	             extra[kCpuCrcIdxLo]   = tmp.s[1];
                     tmp.i = runf.GetRun();
                     extra[kRunIdxHi]      = tmp.s[0];
                     extra[kRunIdxLo]      = tmp.s[1];
                     extra[kSeqIdx]        = seq;
                     
                     dtms                  = evnt.GetDTms();
                     
                     runtree->Fill();
                     ++eventsread;
                  } else if (hcode==TSnIOHeaderFrame::kPowerCode) {
                     //cerr << "reading power" << endl;
                     bytes = TSnIODataFile::ReadPower(inf, pow);
                     av1 = pow.GetAveV1();
                     av2 = pow.GetAveV2();
                     rm1 = pow.GetRmsV1();
                     rm2 = pow.GetRmsV2();
                     vtime = pow.GetTime();
                     voltt->Fill();
                     ++voltsread;
		  } else if (hcode==TSnIOHeaderFrame::kHeartbeatCode) {
		     bytes = TSnIODataFile::ReadHeartbeat(inf, *hbtimestamp, hbnum);
		     hbtree->Fill();
		     ++hbread;
                  } else {
                     Printf("Unrecognized block, header code %x (%d)",
                            hcode,hcode);
                     fseek(inf, hlen, SEEK_CUR);
                  }
               } else {
                  // reached the end of the file
                  // (but TSnIOHeaderFrame will refuse to read past the
                  //  end, so the EOF flag never gets set)
                  break;
               }
            }
            Printf("Read %u events, %u power readings and %u heartbeats.",
               eventsread, voltsread, hbread);
         } else {
            Printf("Could not find configuration in file.");
         }

      } catch (const std::exception& e) {
         Printf("EXCEPTION: %s",e.what());
      }
      fclose(inf);
   } else {
      Printf("Could not open file [%s]", infn);
   }

  //rootfile->cd();
  //tree->Write();
  rootfile->Write();
  delete rootfile;
}
