#ifndef SNS_TSnNetAdrSet
#define SNS_TSnNetAdrSet

#include <TNamed.h>

class TSnNetAdrSet : public TNamed {
 public:
   static const UChar_t kIPLen=16; // length of IP address strings
   static const Char_t* kIPDEF;    // default ip string

 private:
   Char_t   fRserv[TSnNetAdrSet::kIPLen];   //  the remote server
   UShort_t fRport;                         //  port on the remote server
   Char_t   fLocIp[TSnNetAdrSet::kIPLen];   //  the local ip
   Char_t   fLocMask[TSnNetAdrSet::kIPLen]; //  the local mask
   Char_t   fLocGate[TSnNetAdrSet::kIPLen]; //  the local gateway

   void ResetName();

 public:
   TSnNetAdrSet();
   TSnNetAdrSet(const Char_t* rserv, const UShort_t rport,
                const Char_t* locip, const Char_t* locmask,
                const Char_t* locgate);
   virtual ~TSnNetAdrSet() {}

   virtual void	       Print(Option_t* option = "") const;
   virtual Bool_t IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const;
   
   const Char_t*  GetRemoteServer() const { return fRserv; }
   UShort_t       GetRemotePort() const { return fRport; }
   const Char_t*  GetLocalIP() const { return fLocIp; }
   const Char_t*  GetLocalMask() const { return fLocMask; }
   const Char_t*  GetLocalGateway() const { return fLocGate; }
   
   void  SetRemoteServer(const Char_t* rs);
   void  SetRemotePort(const UShort_t rp) { fRport = rp; }
   void  SetLocalIP(const Char_t* s);
   void  SetLocalMask(const Char_t* s);
   void  SetLocalGateway(const Char_t* s);
   
   ClassDef(TSnNetAdrSet, 2); // store a set of network addresses
};

#endif // SNS_TSnNetAdrSet
