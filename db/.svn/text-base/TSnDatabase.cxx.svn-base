#include "TSnDatabase.h"

#include "TSnConnection.h"
#include "TSnDbKey.h"

#include <THashTable.h>
#include <TError.h>
#include <TMap.h>

#include <stdexcept>

ClassImp(TSnDatabase)

TSnDatabase::TSnDatabase(const Bool_t) : 
   fConnections(new THashTable),
   fCache(new TMap) {
   // the bool is only used to differentiate this ctor, which allocates
   // memory, from the default one, which doesn't -- so that its streamer
   // will not leak memory
   fCache->SetOwnerKeyValue();
}

TSnDatabase::~TSnDatabase() {
   delete fConnections;
   delete fCache;
}

Bool_t TSnDatabase::OpenConnection(TSnConnection* c) {
   TObject* cc = fConnections->FindObject(c);
   if (cc==0) {
      fConnections->Add(c);
      return c->Open();
   } else if (c->IsOpen()==kFALSE) {
      return c->Open();
   } else {
      Error("NewConnection","Connection [%s] already open.",
         c->GetName());
   }
   
   return kFALSE;
}

const TSnConnection* TSnDatabase::GetConnection(const Char_t* cname) const {
   return dynamic_cast<const TSnConnection*>(fConnections->FindObject(cname));
}

TSnConnection* TSnDatabase::GetConnection(const Char_t* cname) {
   return dynamic_cast<TSnConnection*>(fConnections->FindObject(cname));
}

void  TSnDatabase::CloseConnection(const Char_t* cname) {
   TObject* cc = fConnections->FindObject(cname);
   if (cc!=0) {
      delete ( fConnections->Remove(cc) );
   }
}

const TObject& TSnDatabase::Get(TSnDbKey& key) {
   // try to get the key from the cache
   // if it's not in the cache yet, ask the database for it
   //
   // only return a constant object to preserve objects in the cache.
   // this avoids the following situation:
   //    (a) key #1 selects object O using parmater X
   //    (b) user modifies object O to P
   //    (c) key #2 uses same parameter X, but now fetches P from the
   //        cache, instead of the original object O as it is in the DB
   //
   // keys using sub-keys to build objects that reference other db objects
   // should use the TSnDbKeyWhisperer to get a non-const object
   //
   // if not in the cache and couldn't make the object from the key,
   // will throw a runtime_error
   
   const TPair* p = dynamic_cast<TPair*>(fCache->FindObject(&key));
   if (p!=0) {
      return *(p->Value());
   } else {
      TObject* obj = TSnDbKeyWhisperer::GetFromDb(key, *this);
      if (obj!=0) {
         // cache the newly made object
         AddToCache(key, *obj);
      } else {
         throw std::runtime_error(Form("<TSnDatabase::Get>: "
            "Could not get object from db using key [%s]",
            key.AsString().Data()));
      }
      return *obj;
   }
}

NSnDatabase::EWriteType TSnDatabase::WriteNoCache(TSnDbKey&      key, 
                                                  const TObject& obj) {
   // write the object to the database using the specified key
   // this (const object) version does NOT cache the object!
   
   NSnDatabase::EWriteType wt = TSnDbKeyWhisperer::WriteToDb(key, *this, obj);
   return wt;
}

NSnDatabase::EWriteType TSnDatabase::Write(TSnDbKey&    key, 
					   TObject&     obj,
					   const Bool_t store) {
   // write the object to the database using the specified key
   // 'store' = whether to store the  key/object pair in the cache
   
  NSnDatabase::EWriteType wt = 
    TSnDbKeyWhisperer::WriteToDb(key, *this, obj);
   if (store) {
      AddToCache(key, obj);
   }
   return wt;
}

void TSnDatabase::AddToCache(const TSnDbKey& key, TObject& obj) {
   // add a copy of the key and add the object itself to the cache
   // the cache will the own the object (unless it is reclaimed)
   
   fCache->Add(key.Clone(), &obj);
}

TObject* TSnDatabase::Reclaim(TSnDbKey& key) {
   // removes the object from the cache, but doesn't delete it.
   // care must be taken to ensure no dangling pointers, so we pass that
   // worry on to the caller
   //
   // returns 0 if no object found in the cache
   //
   // this function probably won't get called, but if memory becomes an
   // issue, it is available
   
   TObject* o = 0;
   TPair* kv = fCache->RemoveEntry(&key);
   if (kv!=0) {
      o = kv->Value();
      delete kv->Key();
      delete kv;
   }
   return o;
}


