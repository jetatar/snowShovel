#include "TSnCommQueue.h"

#include <TROOT.h>
#include <TClass.h>
#include <TMap.h>
#include <TObjString.h>
#include <TList.h>

#include "TSnCompMbed.h"
#include "TSnCommObject.h"

#include <stdexcept>

ClassImp(TSnCommQueue);

TSnCommQueue::TSnCommQueue(const Bool_t) :
   fQ(new TMap) {
   // the bool is used to differentiate this ctor, which allocates memory,
   // from the default ctor which does not. this prevents the streamer from
   // leaking memory
   fQ->SetOwnerKeyValue();
}

TSnCommQueue::~TSnCommQueue() {
   delete fQ;
}

void TSnCommQueue::Print(Option_t *option) const {
   // print all the comm objects for each station
   
   fQ->Print(option, 2);
}

void TSnCommQueue::ls(Option_t *option) const {
   // list the comm objects for each station, but don't print them out
   
   TIter nk(fQ);
   const TObject* k(0);
   while ( (k=static_cast<const TPair*>(nk()))!=0 ) {
      TROOT::IndentLevel();
      Printf("Queue for station [%s]:", k->GetName());
      TROOT::IncreaseDirLevel();
      const TList* stn = static_cast<const TList*>(fQ->GetValue(k));
      const TSnCommObject* co(0);
      TIter pc(stn, kIterBackward);
      UInt_t i(0);
      while ( (co=static_cast<const TSnCommObject*>(pc()))!=0 ) {
         TROOT::IndentLevel();
         Printf("%u) %s (%s)", ++i, co->GetLabel().Data(),
            co->IsA()->GetName());
      }
      TROOT::DecreaseDirLevel();
   }
}

TList* TSnCommQueue::GetOrMakeStnQueue(const Char_t* station) {
   TList* stn = dynamic_cast<TList*>(fQ->GetValue(station));
   if (stn==0) {
      stn = new TList;
      stn->SetOwner(kTRUE);
      fQ->Add(new TObjString(station), stn);
   }
   if (stn==0) {
      throw std::runtime_error(Form("<TSnCommQueue::GetOrMakeStnQueue>: "
                                    "Unable to make queue for [%s]. "
                                    "Out of memory?",
                                    station));
   }
   return stn;
}

void TSnCommQueue::AppendCommFor(const Char_t* station, 
                                 const TSnCommObject& comdat) {
   // adds a clone of the 'comdat' object to be stored in the
   // queue for the specified station
   //
   // this will place comdat last in the queue for its station
   //
   // this is assumed to be the normal way to add a comm for a station
   
   TList* stn = GetOrMakeStnQueue(station);
   stn->AddFirst(comdat.Clone());
}

void TSnCommQueue::PrependCommFor(const Char_t* station, 
                                  const TSnCommObject& comdat) {
   // adds a clone of the 'comdat' object to be stored in the
   // queue for the specified station
   //
   // this function forces comdat to the front of the queue,
   // breaking the FIFO behavior!
   //
   // for normal usage, see AppendCommFor

   TList* stn = GetOrMakeStnQueue(station);
   stn->AddLast(comdat.Clone());
}

Bool_t TSnCommQueue::HasAnyCommsFor(const Char_t* station) const {
   // check if any comm object is in the queue for the specified station
   Bool_t any=kFALSE;
   TList* stn = static_cast<TList*>(fQ->GetValue(station));
   if (stn!=0) {
      any = stn->GetSize()>0;
   }
   return any;
}

const TSnCommObject*
TSnCommQueue::PeekNextCommFor(const Char_t* station) const {
   // retrieve the next communication object from the queue if any
   // is present for the specified station.
   // the comm object remains in the queue.
   // returns 0 if no comm object is in the queue for the station.
   //
   // if there is no queue for the station, a std::out_of_range
   // exception will be thrown. use HasCommFor to avoid this.
   
   // we only allow certain types to be added, so we use the
   // faster static casts
   const TSnCommObject* co(0);
   const TList* stn = PeekCommsFor(station);
   if (stn!=0) {
      co = static_cast<const TSnCommObject*>(stn->Last());
   }
   return co;
}

const TList* TSnCommQueue::PeekCommsFor(const Char_t* station) const {
   // get a pointer to the FIFO queue for the specified station
   // the queue cannot (must not) be modified directly via this pointer
   //
   // note that the END of the list is the front of the stack.
   // thus TList::Last gives the next comm object and TList::First
   // gives the comm object furthest down in the queue
   //
   // if there is no queue for the station, a std::out_of_range
   // exception will be thrown. use HasCommFor to avoid this.
   
   const TList* stn = static_cast<const TList*>(fQ->GetValue(station));
   if (stn==0) {
      throw std::out_of_range(Form("No FIFO queue for [%s].",
                                   station));
   }
   return stn;
}

Bool_t TSnCommQueue::ClearAllCommsFor(const Char_t* station) {
   // clear the FIFO queue for the specified station and
   // remove the queue itself from the map
   //
   // returns true if the station had a FIFO queue and it was cleared
   // false if not
   
   Bool_t ret(kFALSE);
   TPair* pair = static_cast<TPair*>(fQ->GetTable()->FindObject(station));
   if (pair!=0) {
      ret = fQ->DeleteEntry(pair->Key());
   }
   return ret;
}

Bool_t TSnCommQueue::RemoveNextCommFor(const Char_t* station) {
   // find the next communication object queued up for the specified
   // station and remove it from the queue
   //
   // if this is the only comm object in the station's queue, the
   // FIFO queue will be cleared from the map as well
   //
   // returns true if the station had a comm and it was cleared
   // false if not
   
   Bool_t ret(kFALSE);
   TList* stn = static_cast<TList*>(fQ->GetValue(station));
   if (stn!=0) {
      if (stn->GetSize()<2) {
         ret = ClearAllCommsFor(station);
      } else {
         stn->RemoveLast();
         ret = kTRUE;
      }
   }
   return ret;

}

TSnCommObject* TSnCommQueue::PullNextCommFor(const Char_t* station) {
   // find the next communication object queued up for the specified
   // station, remove it from the queue and return it
   //
   // if no comm object is queued for the station, a std::out_of_range
   // exception will be thrown. use HasCommFor to avoid this.
   
   TSnCommObject* co(0);
   
   TList* stn = static_cast<TList*>(fQ->GetValue(station));
   if ((stn!=0) && (stn->GetSize()>0)) {
      co = static_cast<TSnCommObject*>(stn->Last());
   }
   
   // prevent RemoveNextCommFor from deleting the comm object upon removal
   const Bool_t iov = fQ->IsOwnerValue();
   fQ->SetOwnerValue(kFALSE);
   if (co!=0) {
      RemoveNextCommFor(station);
   } else {
      throw std::out_of_range(Form("<TSnCommQueue::PullNextCommFor>: "
                                   "No comm object for station [%s]",
                                   station));
   }
   // go back to owning everything
   fQ->SetOwnerValue(iov);
   
   return co;
}


