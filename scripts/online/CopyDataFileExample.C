#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>

#include <cstdio>
#include <stdexcept>
#include <iostream>

#include <TSnDAQConfig.h>
#include <TSnRunInfo.h>
#include <TSnIODataFile.h>
#include <TSnCompMbed.h>
#include <NSnConstants.h>
#include <TSnEvent.h>
#endif

/**
 * gSystem->Load("$SNS/lib/libdbdat.so"); gSystem->Load("$SNS/lib/libonline.so");
 */

void CopyDataFileExample(const Char_t* infn="/home/jamesaw1/work/snowShovel_datafiles/SnEvtsM0002F7F0CC6Er00000s00002.dat") {
      
   TSnDAQConfig conf;
   TSnRunInfo   runf;
   TSnCompMbed  comp;
   TSnEvent     evnt;

   FILE* inf = fopen(infn,"rb");
   if (inf!=0) {
      try {
	TSnIODataFile::OpenFile(inf, conf, runf, comp);
        FILE* outfile = TSnIODataFile::WriteFile("/home/jamesaw1/work/snowShovel_datafiles/SnEvtsM0002F7F0CC6Er00000s00002_copy.dat", conf, runf, comp);

        UInt_t bytes;
        int eventsread = 0;

        while ((bytes = TSnIODataFile::ReadNextEvent(inf, conf, evnt)) > 0) {
          TSnIODataFile::WriteNextEvent(outfile, conf, evnt);
          eventsread++;
        }

	std::cout << eventsread << std::endl;
      } catch (const std::exception& e) {
         Printf(e.what());
      }
      fclose(inf);
   }

   runf.Print();
   conf.Print();
   comp.Print();   
   evnt.Print();
}
