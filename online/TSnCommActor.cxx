#include "TSnCommActor.h"

#include <TString.h>
#include <TClass.h>

#include "TSnCommAction.h"
#include "TSnCommObject.h"
#include "TSnCommConfPack.h"
#include "TSnCommIncoming.h"

#include <stdexcept>

ClassImp(TSnCommActor);

TSnCommActor::~TSnCommActor() {
   fActs.Delete();
   fComQue.Delete();
   fLastSent.Delete();
}

TSnCommAction* TSnCommActor::GetAction(const Char_t* actionClassName) {
   // since we know nothing else is in the list, use static cast
   return static_cast<TSnCommAction*>(fActs.FindObject(actionClassName));
}

const TSnCommAction* TSnCommActor::GetAction(
                                     const Char_t* actionClassName) const {
   // since we know nothing else is in the list, use static cast
   return static_cast<const TSnCommAction*>(fActs.FindObject(actionClassName));
}

TClass* TSnCommActor::GetClassSafe(const Char_t* cn) {
   TClass* c = TClass::GetClass(cn);
   if (c==0) {
      throw std::runtime_error(Form("<TSnCommActor::GetClassFor> : "
                                    "Could not get TClass for class [%s]",cn));
   } else if (c->InheritsFrom(TSnCommAction::Class())==kFALSE) {
      throw std::runtime_error(Form("TSnCommActor::GetClassFor> : "
                                    "Specified class [%s] is not a "
                                    "TSnCommAction.",cn));
      c=0;
   }
   return c;
}

TSnCommAction* TSnCommActor::NewAction(const Char_t* acn) {
   TClass* c = GetClassSafe(acn); // checks that it's a TSnCommAction
   TSnCommAction* a = static_cast<TSnCommAction*>(c->New());
   TSnCommActionWhisperer::SetActor(a, this);
   return a;
}

TObjLink* TSnCommActor::FindLinkSlow(const Char_t* acn) {
   TObjLink* lnk = fActs.FirstLink();
   while (lnk!=0) {
      TObject* o = lnk->GetObject();
      if (o!=0) {
         TClass* c = o->IsA();
         if (c!=0) {
            if (strcmp(c->GetName(),acn)==0) {
               break;
            }
         }
      }
      lnk = lnk->Next();
   }
   return lnk;
}

TObjLink* TSnCommActor::FindLinkSafe(const Char_t* acn) {
   TObjLink* lnk = FindLinkSlow(acn);
   if (lnk==0) {
      throw std::runtime_error(Form("<TSnCommActor::FindLinkSafe> : "
                                    "No link for object of class [%s]",
                                    acn));
   }
   return lnk;
}

void TSnCommActor::AddFirst(const Char_t* actionClassName) {
   fActs.AddFirst( NewAction(actionClassName) );
}

void TSnCommActor::AddLast(const Char_t* actionClassName) {
   fActs.AddLast( NewAction(actionClassName) );
}

void TSnCommActor::AddBefore(const Char_t* actionClassNameBefore,
                             const Char_t* actionClassName) {
   TObjLink* lnk = FindLinkSafe(actionClassNameBefore);
   if (lnk!=0) {
      fActs.AddBefore(lnk, NewAction(actionClassName) );
   }
}

void TSnCommActor::AddAfter(const Char_t* actionClassNameAfter, 
                            const Char_t* actionClassName) {
   TObjLink* lnk = FindLinkSafe(actionClassNameAfter);
   if (lnk!=0) {
      fActs.AddAfter(lnk, NewAction(actionClassName) );
   }
}

void TSnCommActor::Remove(const Char_t* actionClassName) {
   TObjLink* lnk = FindLinkSafe(actionClassName);
   if (lnk!=0) {
      TObject* o = fActs.Remove(lnk);
      delete o;
   }
}

const TSnCommAction* TSnCommActor::FirstAction() const {
   return static_cast<const TSnCommAction*>(fActs.First());
}

const TSnCommAction* TSnCommActor::LastAction() const {
   return static_cast<const TSnCommAction*>(fActs.Last());
}

const TSnCommAction* TSnCommActor::NextAction(const TSnCommAction* a) const {
   return static_cast<const TSnCommAction*>(fActs.After(a));
}

const TSnCommAction* TSnCommActor::PreviousAction(
                                         const TSnCommAction* a) const {
   return static_cast<const TSnCommAction*>(fActs.Before(a));
}

const TSnCommAction* TSnCommActor::FindAction(
                                         const Char_t* actionClassName) const {
   return static_cast<const TSnCommAction*>(fActs.FindObject(actionClassName));
}

void TSnCommActor::ReplaceLastSent(const TSnCommObject& o) {
   TClass* c = o.IsA();
   if (c!=0) {
      TPair* p = dynamic_cast<TPair*>(fLastSent.FindObject(c->GetName()));
      if (p!=0) {
         // remove previous last sent of this type
         fLastSent.DeleteEntry(p->Key());
      }
      fLastSent.Add(new TObjString(c->GetName()), o.NewClone());
   } else {
      throw std::runtime_error(Form("<TSnCommActor::ReplaceLastSent>: "
                                    "Could not get class for obj [%s]",
                                    o.GetName()));
   }
}

const TSnCommObject* TSnCommActor::GetLastSent(const Char_t* classname) const {
   TPair* kv = dynamic_cast<TPair*>(fLastSent.FindObject(classname));
   TSnCommObject* o(0);
   if (kv!=0) {
      o = dynamic_cast<TSnCommObject*>(kv->Value());
   }
   return o;
}

const TSnCommConfPack* TSnCommActor::GetLastConf() const {
   const TSnCommObject* c = GetLastSent("TSnCommConfPack");
   if (c!=0) {
      return dynamic_cast<const TSnCommConfPack*>(c);
   }
   return 0;
}

void TSnCommActor::AppendComm(const TSnCommObject& co) {
   // adds a clone of the 'comdat' object to be stored in the queue
   //
   // this will place comdat last in the queue for its station
   //
   // this is assumed to be the normal way to add a comm for a station
   
   // objs pulled from the back, so this puts it "last"
   fComQue.AddFirst(co.NewClone());
}

void TSnCommActor::PrependComm(const TSnCommObject& co) {
   // adds a clone of the 'comdat' object to be stored in the queue
   //
   // this function forces comdat to the front of the queue,
   // breaking the FIFO behavior!
   //
   // for normal usage, see AppendCommFor

   fComQue.AddLast(co.NewClone());
}

Bool_t TSnCommActor::RemoveNextComm() {
   // find the next communication object queued up for the
   // station and remove it from the queue
   //
   // returns true if the station had a comm and it was cleared
   // false if not

   Bool_t ret(kFALSE);
   TObjLink* l = fComQue.LastLink();
   if (l!=0) {
      delete fComQue.Remove(l);
      ret = kTRUE;
   }
   return ret;
}

Bool_t TSnCommActor::ClearAllComms() {
   // clear the FIFO comms queue
   //
   // returns true if the queue was non-empty and false if it was
   // already empty

   const Bool_t ret = HasComms();
   fComQue.Delete();
   return ret;
}

const TSnCommObject* TSnCommActor::PeekNextComm() const {
   // retrieve the next communication object from the queue if any
   // is present
   // the comm object remains in the queue.
   // returns 0 if no comm object is in the queue for the station.
   
   // we only allow certain types to be added, so we use the
   // faster static casts
   return static_cast<const TSnCommObject*>(fComQue.Last());
}

TSnCommObject* TSnCommActor::PullNextComm() {
   // find the next communication object queued up for the
   // station, remove it from the queue and return it
   //
   // if no comm object is queued for the station, returns 0
   
   TSnCommObject* co(0);
   
   TObjLink* l = fComQue.LastLink();
   if (l!=0) {
      co = static_cast<TSnCommObject*>(fComQue.Remove(l));
   }
   return co;
   
}

TSnCommObject* TSnCommActor::GetResponseFor(const TSnCommIncoming& in) {
   // Loop over the status actions (in order), providing access to the specified
   // status, power reading and comm time, and return the a TSnCommObject*
   // as a response. In principle, this response can be 0, but this is
   // probably never desired.

   TSnCommObject* response(0);

   TIter next(NewActionIter());
   TSnCommAction* action(0);
   while ( (action = static_cast<TSnCommAction*>(next()))!=0 ) {
      response = (*action)(in);
      if (response!=0) {
         if (response->IsOkForStatusVersion(in.GetStatusIOversion())) {
            SetLastAction(action);
            ReplaceLastSent(*response);
            break;
         } else {
            Error("GetResponseFor",
                  "Action [%s] gave response [%s] that is NOT ok to send "
                  "to a station reporting a status i/o version [%d]",
                  action->GetLabel().Data(), response->GetLabel().Data(),
                  in.GetStatusIOversion());
            delete response;
            response = 0;
         }
      }
   }

   return response;   
}

void TSnCommActor::Print(Option_t* option) const {
   // no option specified will print everything. otherwise:
   // if option contains "all", print everything
   // if option contains "lastsent", print last sent
   // if option contains "queue", print comm obj queue
   // if option contains "action", print actions
   TNamed::Print(option);
   TString opt(option);
   opt.ToLower();
   const Bool_t pal = (opt.Length()==0) || opt.Contains("all");
   const Bool_t pls = opt.Contains("lastsent");
   const Bool_t pqu = opt.Contains("queue");
   const Bool_t pac = opt.Contains("action");
   
   if (pal) {
      Printf("========= UseDB = %s", (fUseDB) ? "True" : "False");
   }
   if (pal || pls) {
      Printf("========= Last Sent:");
      if (fLastSent.IsEmpty()) {
         Printf("------ [empty]");
      } else {
         TIter nls(fLastSent.MakeIterator());
         TObject* key(0);
         while ( (key=nls())!=0 ) {
            Printf("------ [%s] Last Sent",key->GetName());
            TObject* v = fLastSent.GetValue(key);
            if (v!=0) {
               v->Print(option);
            }
         }
      }
   }
   if (pal || pqu) {
      Printf("========= Queue:");
      if (HasComms()) {
         // loop over the queue backwards, since the stuff at the end will
         // be sent first
         TIter nq(fComQue.MakeIterator(kIterBackward));
         const TObject* c(0);
         UInt_t i=0;
         while ( (c=nq())!=0 ) {
            Printf("------ #%d: %s",++i,c->GetName());
            c->Print(option);
         }
      } else {
         Printf("------ [empty]");
      }
   }
   if (pal || pac) {
      Printf("========= Actions:");
      const TSnCommAction* a = FirstAction();
      const TClass* acl;
      Int_t i=0;
      do {
         acl = a->IsA();
         Printf("------ #%d: %s (%s)", ++i,
                a->GetLabel().Data(),
                (acl!=0) ? acl->GetName() : "<unknown>");
      } while ( (a=NextAction(a))!=0 );
   }
}

