#ifndef SNS_TSnConnection
#define SNS_TSnConnection

#include <TNamed.h>

class TSnConnection : public TNamed {
   
 public:
   TSnConnection() {}
   TSnConnection(const Char_t* name, const Char_t* title=0) :
      TNamed(name, title) {}
   virtual ~TSnConnection() {}
   
   virtual Bool_t   IsOpen() const  =0;
   virtual void     Disconnect()    =0;
   virtual Bool_t   Open()          =0;
   virtual void     Activate()      =0;
   virtual void     Deactivate()    =0;
   
   
   ClassDef(TSnConnection, 0);
};

#endif // SNS_TSnConnection
