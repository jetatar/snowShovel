#ifndef SNS_TSnCommAction
#define SNS_TSnCommAction

#include <TObject.h>
#include <TRef.h>
#include <TString.h>

class TSnCommObject;
class TSnCommActor;
class TSnCommIncoming;

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommAction
//
// A base class providing the interface for a communication action functor.
// Because these objects are functors, 
//
////////////////////////////////////////////////////////////////////////////////

class TSnCommAction : public TObject {
 private:
   TRef     fActor; // the parent TSnCommActor managing this action

   void     SetActor(TSnCommActor* a);

 protected:
   TSnCommAction() {}
   TSnCommAction(TSnCommActor* a);
   const TSnCommActor& GetActorSafe(const TClass* cls) const;

 public:
   virtual ~TSnCommAction() {}

   const TSnCommActor*    GetActorPtr() const;
         TSnCommActor*    GetActorPtr();

   virtual TSnCommObject* operator()(const TSnCommIncoming& in)=0;
   
   virtual TString        GetLabel() const=0;
   
         TSnCommActor&    GetActor();
   const TSnCommActor&    GetActorSafe() const;

   friend class TSnCommActionWhisperer; // to preserve encapsulation
   
   ClassDef(TSnCommAction, 1); // base of a communication response action functor
};

class TSnCommActionWhisperer {
   // simple interface to allow only TSnCommActor to set the Actor of
   // a TSnCommAction
   
   static
   void SetActor(TSnCommAction* action, TSnCommActor* a) {
      action->SetActor(a);
   }
   
   friend class TSnCommActor; // only TSnCommActor is allowed access
};

#endif // SNS_TSnCommAction
