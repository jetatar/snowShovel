#include "TSnCommAction.h"

#include <TClass.h>

#include "TSnCommActor.h"

#include <stdexcept>

ClassImp(TSnCommAction);

TSnCommAction::TSnCommAction(TSnCommActor* a) :
   fActor(a) {
}

void TSnCommAction::SetActor(TSnCommActor* a) {
   fActor = a;
}

const TSnCommActor* TSnCommAction::GetActorPtr() const {
   // returns the actor with no checks that it's not null or somehow
   // not actually a TSnCommActor
   return dynamic_cast<const TSnCommActor*>(fActor.GetObject());
}

TSnCommActor* TSnCommAction::GetActorPtr() {
   // returns the actor with no checks that it's not null or somehow
   // not actually a TSnCommActor
   return dynamic_cast<TSnCommActor*>(fActor.GetObject());
}

TSnCommActor& TSnCommAction::GetActor() {
   return const_cast<TSnCommActor&>(
      const_cast<const TSnCommAction*>(this)->GetActorSafe() );
}

const TSnCommActor& TSnCommAction::GetActorSafe() const {
   return GetActorSafe(TSnCommActor::Class());
}

const TSnCommActor& TSnCommAction::GetActorSafe(const TClass* cls) const {
   // return the TSnCommActor. throws an exception if either
   // the actor is null or not actually the specified class
   
   const TObject* o = fActor.GetObject();
   if (o==0) {
      throw std::runtime_error(Form("<TSnCommAction::GetActorSafe> : "
                                    "Actor for action [%s] is not set.",
                                    IsA()->GetName()));
   } else if (cls->InheritsFrom(TSnCommActor::Class())) {
      if (o->IsA()->InheritsFrom(cls)==kFALSE) {
         throw std::runtime_error(Form("<TSnCommAction::GetActorSafe> : "
                                       "Actor is of type [%s] instead of "
                                       "[%s].",
                                       o->IsA()->GetName(),
                                       cls->GetName()));
      }
   } else {
      throw std::runtime_error(Form("<TSnCommAction::GetActorSafe> : "
                                    "Cannot cast actor to [%s]",
                                    cls->GetName()));
   }
   return static_cast<const TSnCommActor&>(*o);
}
   

