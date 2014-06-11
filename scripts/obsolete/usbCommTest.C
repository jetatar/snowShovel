#include <TMath.h>
#include <TSystem.h>

#include "TSnIOStatusFrame.h"
#include "TSnStatusUpdate.h"
#include "TSnEvent.h"

#include "readConfig.C"
#include "usbUtils.C"

static const UInt_t bufSize = 4096;

Char_t buf[bufSize];

TSnStatusUpdate status;
TSnEvent evt;

void usbCommTest(const Char_t* cfn="commTestConf.dat") {
   
   R__ASSERT(bufSize>TSnIOStatusFrame::kMaxSizeOf);
   
   // connect
   const Int_t fd = openDevice();
   
   Printf("connected");
   
   // wait for status update
   Char_t* b = buf;
   Int_t nb=0;
   Bool_t talking=kFALSE;
   while (kTRUE) {
      nb = readNextBytes(fd, b, 1u);
      if (nb<0) {
         if (talking==kFALSE) {
            Printf("receiving data...");
         }
         //fprintf(stderr,"%02X ",*b);
         b += -nb;
         talking=kTRUE;
      }
      if (talking && nb>0) {
         break;
      }
      gSystem->Sleep(1);
   }
   
   Printf("done receiving");
   
   try {
      Printf("Read status from buffer");
      TSnIOStatusFrame::ReadFrom(buf, status, evt);
   } catch (const std::exception& e) {
      Printf("status frame read except: %s", e.what());
      close(fd);
      return;
   }

   try {
      Printf("Read config from file");
      readConfig(cfn);
   } catch (const std::exception& e) {
      Printf("read config except: %s", e.what());
      close(fd);
      return;
   }
   
   Char_t* cb=buf;
   try {
      Printf("Write config to buffer");
      cb = TSnIOConfigFrame::WriteTo(buf, conf, runf);
   } catch (const std::exception& e) {
      Printf("write config except: %s", e.what());
      close(fd);
      return;
   }
   /*   
   while (1) {
      Int_t a=1;
      sendData(fd, &a, 4);
   }
   */
   Printf("Sending config (%u bytes)\n",UInt_t(cb-buf));
   nb = sendData(fd, buf, UInt_t(cb-buf));
   Printf("Sent %u bytes", nb);
   
   close(fd);
}
