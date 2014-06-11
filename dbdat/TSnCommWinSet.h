#ifndef SNS_TSnCommWinSet
#define SNS_TSnCommWinSet

#include <TObject.h>

class TSnCommWinSet : public TObject {
 public:
   enum ESendDataBit {
      // must match the bits on the mbed!
      // don't use BIT(0)! (-0 = 0 => send nothing)
      kAllFiles       =  BIT(1),  // if bit=0 => send most recent file
      kTimeout        =  BIT(2),  // if bit=0 => ignore timeout
      kDelete         =  BIT(3),  // if bit=0 => do not delete sent files
      kForceSBDdata   =  BIT(4),  // if bit=0 => do not send data out over SBD other than status update + 1 event
      kHnShBefSendDat = BIT(5), // if bit=0 => do not wait for a handshake after GetConfig before sending data from station
      kUseBits        = static_cast<Short_t>(-BIT(14)), // useful to initialize fCommSendData as a bit word
      kUseBits2012    = static_cast<Short_t>(-BIT(15))  // useful to initialize fCommSendData as a bit word
   };

 private:
   UInt_t  fPer; // the comm win period (s) (default 600s)
   UInt_t  fDur; // the comm win duration (s) (default 420s)
   Short_t fSendData; // data to send during comm win (0: none, >0: send up to X events from last file until window closes, <0: see ESendDataBit) (default 0)
   UChar_t fConnectTOmin; // connection timeout (minutes) (default 3)
   UChar_t fListenTOmin; // listen timeout (minutes) (default 3)

 public:
   TSnCommWinSet(const UInt_t per=600, const UInt_t dur=420,
                 const Short_t send=0, const UChar_t connectMin=3,
                 const UChar_t listenMin=3) :
      fPer(per), fDur(dur), fSendData(send),
      fConnectTOmin(connectMin), fListenTOmin(listenMin) {}
   virtual ~TSnCommWinSet() {}
   
   Bool_t   IsValidForDb() const { return fPer>0; }
   void     Print(Option_t* option="") const;
   
   UInt_t  GetPeriod() const { return fPer; }
   UInt_t  GetDuration() const { return fDur; }
   Short_t GetSendData() const { return fSendData; }
   UChar_t GetConnectTOmin() const { return fConnectTOmin; }
   UInt_t  GetConnectIOsec() const {
      return static_cast<UInt_t>(fConnectTOmin)*60;
   }
   UChar_t GetListenTOmin() const { return fListenTOmin; }
   UInt_t  GetListenIOsec() const {
      return static_cast<UInt_t>(fListenTOmin)*60;
   }

   void    SetPeriod(const UInt_t p) { fPer = p; }
   void    SetDuration(const UInt_t d) { fDur = d; }
   void    SetSendData(const Short_t s) { fSendData = s; }
   void    SetConnectTOmin(const UChar_t t) { fConnectTOmin = t; }
   void    SetListenTOmin(const UChar_t t) { fListenTOmin = t; }
   
   Bool_t  IsSendingAllFiles() const
      { return (fSendData<0) && ((fSendData & kAllFiles)!=0); }
   Bool_t  IsObeyingTimeout() const
      { return (fSendData<0) && ((fSendData & kTimeout)!=0); }
   Bool_t  IsDeletingFiles() const
      { return (fSendData<0) && ((fSendData & kDelete)!=0); }
   Bool_t  IsForcingSBDdata() const
      { return (fSendData<0) && ((fSendData & kForceSBDdata)!=0); }
   Bool_t  IsWaitingHndShkBeforeSendData() const
      { return (fSendData<0) && ((fSendData & kHnShBefSendDat)!=0); }

   void    EnableSendDataOpt(const ESendDataBit b)
      { fSendData |= (b | kUseBits); }
   void    DisableSendDataOpt(const ESendDataBit b) {
      // disable the bit. if all bits disabled, set to 0. does not
      // allow disabling the "kUseBits" identifier
      if (b!=kUseBits) {
         fSendData &= ~b;
         if (fSendData==kUseBits) {
            fSendData=0; 
         }
      }
   }
   void   ToggleSendDataOpt(const ESendDataBit b) {
      // toggle the bit. if all bits disabled, set to 0. does not
      // allow toggling the "kUseBits" identifier
      if (b!=kUseBits) {
         fSendData ^= b;
         if (fSendData==kUseBits) {
            fSendData=0; 
         }
      }
   }

   ClassDef(TSnCommWinSet, 2); // stores the comm win properties
};

#endif // SNS_TSnCommWinSet
