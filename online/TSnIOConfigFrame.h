#ifndef SNS_TSnIOConfigFrame
#define SNS_TSnIOConfigFrame

#include "TSnIOObject.h"
#include "TSnDAQConfig.h"
#include "TSnRunInfo.h"
#include "TSnNetAdrSet.h"
#include "NSnConstants.h"
#include "TSnCommWinSet.h"
class TSnIOBuffer;

class TSnIOConfigFrame : public TSnIOObject {
 public:
   static const UChar_t    kIOvers;
   static const UInt_t     kMaxSizeOfV1;
   static const UInt_t     kMaxSizeOfV2;
   static const UInt_t     kMaxSizeOfV3;
   static const UInt_t     kMaxSizeOfV4;
   static const UInt_t     kMaxSizeOfV5;
   static const UInt_t     kMaxSizeOfV6;
   static const UInt_t     kMaxSizeOfV7;
   static const UInt_t     kMaxSizeOfV8;
   static const UInt_t     kMaxSizeOf;

   static const UChar_t    k2012to2013IOVers = 5; // i/o vers 2012-2013

 private:
   
   static const UChar_t _kMaxUChar = ~0;
   
   static
   UInt_t   SizeOf(const UChar_t rv,
                   const Bool_t streamHiLoPlas,
                   const UChar_t nplas,
                   const UChar_t lblLen);

 public:
   TSnIOConfigFrame() {}
   virtual ~TSnIOConfigFrame() {}
   
   static
   UInt_t   GetMaxSizeOf(const UChar_t rv) {
      if (rv==1) {
         return kMaxSizeOfV1;
      } else if (rv==2) {
         return kMaxSizeOfV2;
      } else if (rv==3) {
         return kMaxSizeOfV3;
      } else if (rv==4) {
         return kMaxSizeOfV4;
      } else if (rv==5) {
         return kMaxSizeOfV5;
      } else if (rv==6) {
         return kMaxSizeOfV6;
      } else if (rv==7) {
         return kMaxSizeOfV7;
      } else if (rv==8) {
         return kMaxSizeOfV8;
     } else {
         return kMaxSizeOf;
      }
   }

   template<typename T>
   static
   UChar_t ReadFrom(T& b,
                    TSnDAQConfig& conf,
                    TSnRunInfo& runf,
                    TSnNetAdrSet& nets) {
      // read the config frame fro the buffer
      //
      // returns the IO version read from the buffer
      const UChar_t Rv = TSnIOObject::ReadFrom<UChar_t>(b);
      if (Rv>0) {

         runf.SetStationLabel(""); // not sent by config frame

         const UInt_t llen    = TSnIOObject::ReadFrom<UInt_t>(b);
         const TString clbl   = TSnIOObject::ReadStrFrom(b, llen);
         conf.SetLabel(clbl.Data());
         runf.SetConfLabel(clbl.Data());

         conf.SetDateBuilt(TSnIOObject::ReadTimeFrom(b));

         runf.SetRun(TSnIOObject::ReadFrom<UInt_t>(b));
         if (Rv>7) {
            runf.SetFirstSeq(TSnIOObject::ReadFrom<UShort_t>(b));
         } else {
            runf.SetObsFirstEvt(TSnIOObject::ReadFrom<UInt_t>(b));
         }
      
         if (Rv>1) {
            runf.SetEvtsPerSeq(TSnIOObject::ReadFrom<UInt_t>(b));
            conf.SetRunMode(TSnIOObject::ReadFrom<UChar_t>(b));
         }
         
         conf.SetStreamHiLoPlas(TSnIOObject::ReadFrom<UChar_t>(b));
         conf.SetWvLoseLSB(TSnIOObject::ReadFrom<UChar_t>(b));
         conf.SetWvLoseMSB(TSnIOObject::ReadFrom<UChar_t>(b));
         conf.SetWvBaseline(TSnIOObject::ReadFrom<UShort_t>(b));
         conf.SetDatPackType(TSnIOObject::ReadFrom<UChar_t>(b));

         for (UChar_t ch=0; ch<NSnConstants::kNchans; ch++) {
            for (UChar_t dc=0; dc<NSnConstants::kNdacs; dc++) {
               conf.SetDac(ch, dc, TSnIOObject::ReadFrom<UShort_t>(b));
            }
         }

         const UChar_t nplas = TSnIOObject::ReadFrom<UChar_t>(b);
         for (UChar_t i=0; i<nplas; i++) {
            conf.SetPla(i, TSnIOObject::ReadFrom<UShort_t>(b));
         }

         conf.SetNumCardsMajLog(TSnIOObject::ReadFrom<UChar_t>(b));
         conf.SetThermalTriggering(TSnIOObject::ReadFrom<UChar_t>(b)!=0);
	 if (Rv>3) {
	    conf.SetForcedPeriod(TSnIOObject::ReadFrom<Float_t>(b));
	 } else {
	    conf.SetForcedPeriod(TSnIOObject::ReadFrom<UShort_t>(b));
	 }
         conf.SetHeartbeatPeriod(TSnIOObject::ReadFrom<UShort_t>(b));

         const UChar_t ampson = TSnIOObject::ReadFrom<UChar_t>(b);
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ch++) {
            conf.SetAmpOn(ch, (ampson & BIT(ch))!=0);
         }

         conf.SetThrottlePeriodms(TSnIOObject::ReadFrom<UShort_t>(b));
         conf.SetPowerMode(TSnIOObject::ReadFrom<UChar_t>(b));
         if (Rv<6) {
            conf.SetBatVoltToLowPwr(TSnIOObject::ReadFrom<Short_t>(b));
         } else {
            conf.SetBatVoltToLowPwr(TSnIOObject::ReadFrom<UShort_t>(b));
            conf.SetBatVoltFromLowPwr(TSnIOObject::ReadFrom<UShort_t>(b));
         }
         if (Rv>2) {
            conf.SetVoltCheckPeriod(TSnIOObject::ReadFrom<UShort_t>(b));
         }
         conf.SetCommWinPeriod(TSnIOObject::ReadFrom<UInt_t>(b));
         conf.SetCommWinDuration(TSnIOObject::ReadFrom<UInt_t>(b));
         conf.SetCommWinSendData(TSnIOObject::ReadFrom<Short_t>(b));
         conf.SetCommWinPerLowPower(TSnIOObject::ReadFrom<UInt_t>(b));
         conf.SetCommWinDurLowPower(TSnIOObject::ReadFrom<UInt_t>(b));
         if (Rv>6) {
            conf.SetCommWinConnectTOmin(TSnIOObject::ReadFrom<UChar_t>(b));
            conf.SetCommWinListenTOmin(TSnIOObject::ReadFrom<UChar_t>(b));
         }
         if (Rv>2) {
            nets.SetRemoteServer(
               TSnIOObject::ReadStrFrom(b,TSnNetAdrSet::kIPLen));
            nets.SetRemotePort(TSnIOObject::ReadFrom<UShort_t>(b));
            nets.SetLocalIP(TSnIOObject::ReadStrFrom(b,TSnNetAdrSet::kIPLen));
            nets.SetLocalMask(TSnIOObject::ReadStrFrom(b,TSnNetAdrSet::kIPLen));
            nets.SetLocalGateway(
               TSnIOObject::ReadStrFrom(b,TSnNetAdrSet::kIPLen));
         }
         conf.SetWatchdogPeriod(TSnIOObject::ReadFrom<UInt_t>(b));

         if (conf.IsStreamingHiLoPlas()) {
	     UShort_t hi, lo, chi, clo;
             for (UChar_t j=0; j<nplas; j++) {
                 hi = TSnIOObject::ReadFrom<UShort_t>(b);
                 lo = TSnIOObject::ReadFrom<UShort_t>(b);
                 // check that they match GetHiLoPlas...
		 // need to do something to prevent this being
		 // optimized away and the buffer point not moving up
		 TSnPlaSet::GetHiLoPlas(conf.GetPla(j),chi,clo);
		 if ( (hi!=chi) || (lo!=clo)) {
		   throw std::runtime_error(
		      Form("<TSnIOConfigFrame::ReadFrom>: "
			   "Hi/Lo PLA stream mismatch. "
			   "hi=%hu, chi=%hu ; lo=%hu, clo=%hu",
			   hi, chi, lo, clo));
		 }
             }
         }
      } else {
         throw std::runtime_error(Form("<TSnIOConfigFrame::ReadFrom>: "
            "Unknown I/O version [%hhu]",Rv));
      }
      return Rv;
   }
   
   template<typename T>
   static
   void WriteTo(T& b,
                const TSnDAQConfig& conf,
                const TSnRunInfo& runf,
                const TSnNetAdrSet& nets,
                const UChar_t Rv=TSnIOConfigFrame::kIOvers) {
      if (Rv>0) {
         TSnIOObject::WriteTo(b, Rv);
         // ensure the /0 gets written to the stream
         // note that as of Rev30, the MBED does not do this
         UInt_t lbllen = conf.GetLabelLen();
         static const UInt_t maxlln = TSnDAQConfig::kMaxLblLen-1;
         if (lbllen > maxlln) {
            lbllen = maxlln;
         }
         TSnIOObject::WriteTo(b, lbllen+1);
         TSnIOObject::WriteStrTo(b, conf.GetLabel(), lbllen);
         TSnIOObject::WriteTo(b, Char_t('\0'));
         TSnIOObject::WriteTimeTo(b, conf.GetDateBuilt());
         TSnIOObject::WriteTo(b, runf.GetRun());
         if (Rv>7) {
            TSnIOObject::WriteTo(b, runf.GetFirstSeq());
         } else {
            TSnIOObject::WriteTo(b, runf.GetObsFirstEvt());
         }
         if (Rv>1) {
            TSnIOObject::WriteTo(b, runf.GetEvtsPerSeq());
            TSnIOObject::WriteTo(b, conf.GetRunMode());
         }
         TSnIOObject::WriteTo(b, static_cast<UChar_t>(
                                 conf.IsStreamingHiLoPlas()));
         TSnIOObject::WriteTo(b, conf.GetWvLoseLSB());
         TSnIOObject::WriteTo(b, conf.GetWvLoseMSB());
         TSnIOObject::WriteTo(b, conf.GetWvBaseline());
         TSnIOObject::WriteTo(b, conf.GetDatPackType());
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ch++) {
            for (UChar_t dc=0; dc<NSnConstants::kNdacs; dc++) {
               TSnIOObject::WriteTo<UShort_t>(b, conf.GetDac(ch, dc));
            }
         }
         if (conf.GetPlaSet().GetNplas()>_kMaxUChar) {
            throw std::runtime_error(
               Form("<TSnIOConfigFrame::WriteTo>: "
                    "Number of plas [%u] too large to fit in UChar_t.",
                    static_cast<UInt_t>(conf.GetPlaSet().GetNplas())));
         }
         const UChar_t nplas = conf.GetPlaSet().GetNplas();
         TSnIOObject::WriteTo(b, nplas);
         for (UChar_t i=0; i<nplas; i++) {
            TSnIOObject::WriteTo(b, conf.GetPla(i));
         }
         TSnIOObject::WriteTo(b, conf.GetNumCardsMajLog());
         TSnIOObject::WriteTo(b, static_cast<UChar_t>(
                                 conf.IsThermalTriggering()));
         if (Rv>3) {
            TSnIOObject::WriteTo(b, conf.GetForcedPeriod());
         } else {
            TSnIOObject::WriteTo(b, conf.GetObsForcedPeriodUShort());
         }
         TSnIOObject::WriteTo(b, conf.GetHeartBeatPeriod());
         TSnIOObject::WriteTo(b, conf.GetAmpOnSet().GetAmpOnBitword<UChar_t>());
         TSnIOObject::WriteTo(b, conf.GetThrottlePeriodms());
         TSnIOObject::WriteTo(b, conf.GetPowerMode());
         if (Rv<6) {
            TSnIOObject::WriteTo(b, conf.GetObsBatVoltToLowPwrShort());
         } else {
            TSnIOObject::WriteTo(b, conf.GetBatVoltToLowPwr());
            TSnIOObject::WriteTo(b, conf.GetBatVoltFromLowPwr());
         }
         if (Rv>2) {
            TSnIOObject::WriteTo(b, conf.GetVoltCheckPeriod());
         }
         TSnIOObject::WriteTo(b, conf.GetCommWinPeriod());
         TSnIOObject::WriteTo(b, conf.GetCommWinDuration());
         if (Rv>6) {
            TSnIOObject::WriteTo(b, conf.GetCommWinSendData());
         } else {
            Short_t csd = conf.GetCommWinSendData();
            if ( (csd & TSnCommWinSet::kUseBits)!=0 ) {
               // unset bit 14, set bit 15
               csd &= ~TSnCommWinSet::kUseBits;
               csd |=  TSnCommWinSet::kUseBits2012;
            }
            if ( (csd & TSnCommWinSet::kHnShBefSendDat)!=0 ) {
               csd &= ~TSnCommWinSet::kHnShBefSendDat;
            }
            TSnIOObject::WriteTo(b, csd);
         }
         TSnIOObject::WriteTo(b, conf.GetCommWinPerLowPower());
         TSnIOObject::WriteTo(b, conf.GetCommWinDurLowPower());
         if (Rv>6) {
            TSnIOObject::WriteTo(b, conf.GetCommWinConnectTOmin());
            TSnIOObject::WriteTo(b, conf.GetCommWinListenTOmin());
         }
         if (Rv>2) {
            TSnIOObject::WriteStrTo(b, nets.GetRemoteServer(), 
                                    TSnNetAdrSet::kIPLen);
            TSnIOObject::WriteTo(b, nets.GetRemotePort());
            TSnIOObject::WriteStrTo(b, nets.GetLocalIP(),
                                    TSnNetAdrSet::kIPLen);
            TSnIOObject::WriteStrTo(b, nets.GetLocalMask(),
                                    TSnNetAdrSet::kIPLen);
            TSnIOObject::WriteStrTo(b, nets.GetLocalGateway(),
                                    TSnNetAdrSet::kIPLen);
         }
         TSnIOObject::WriteTo(b, conf.GetWatchdogPeriod());
         if (conf.IsStreamingHiLoPlas()) {
            UShort_t hi, lo;
            for (UChar_t i=0; i<nplas; i++) {
               TSnPlaSet::GetHiLoPlas(conf.GetPla(i), hi, lo);
               TSnIOObject::WriteTo(b, hi);
               TSnIOObject::WriteTo(b, lo);
            }
         }
      } else {
         throw std::runtime_error(Form("<TSnIOConfigFrame::WriteTo>: "
            "Unknown I/O version [%hhu]",Rv));
      }
   }
   
   static
   UInt_t BytesReadFrom(const TSnIOBuffer& buf,
                        TSnDAQConfig& conf,
                        TSnRunInfo& runf,
                        TSnNetAdrSet& nets,
                        Int_t& Rv) {
      const TSnIOBuffer::Pos_t op = buf.GetPosVal();
      Rv = ReadFrom(buf, conf, runf, nets);
      return buf.GetPosVal() - op;
   }

   inline
   static
   TSnIOBuffer GetIOByteArray(const TSnDAQConfig& conf,
                              const TSnRunInfo& runf,
                              const TSnNetAdrSet& nets,
                              const UChar_t Rv=TSnIOConfigFrame::kIOvers) {
      // make an IO buffer and fill it.
      TSnIOBuffer b;
      WriteTo(b, conf, runf, nets, Rv);
      return b;
   }
   
   static
   UInt_t   SizeOf(const UChar_t rv, const TSnDAQConfig& conf);


   // rootcint can't parse the templated functions, so wrap them
   static
   UChar_t ReadFromW(const TSnIOBuffer& b, TSnDAQConfig& conf,
                     TSnRunInfo& runf, TSnNetAdrSet& nets);
   static
   UChar_t ReadFromW(const Char_t*& b, TSnDAQConfig& conf,
                     TSnRunInfo& runf, TSnNetAdrSet& nets);
   static
   UChar_t ReadFromW(FILE*& b, TSnDAQConfig& conf, TSnRunInfo& runf,
                     TSnNetAdrSet& nets);
   static
   void WriteToW(TSnIOBuffer& b, const TSnDAQConfig& conf,
                 const TSnRunInfo& runf, const TSnNetAdrSet& nets,
                 const UChar_t Rv=TSnIOConfigFrame::kIOvers);
   static
   void WriteToW(Char_t*& b, const TSnDAQConfig& conf,
                 const TSnRunInfo& runf, const TSnNetAdrSet& nets,
                 const UChar_t Rv=TSnIOConfigFrame::kIOvers);
   static
   void WriteToW(FILE*& b, const TSnDAQConfig& conf,
                 const TSnRunInfo& runf, const TSnNetAdrSet& nets,
                 const UChar_t Rv=TSnIOConfigFrame::kIOvers);
   
   ClassDef(TSnIOConfigFrame, 3); // read/write a config frame from/to a stream of bytes
};

#endif // SNS_TSnIOConfigFrame
