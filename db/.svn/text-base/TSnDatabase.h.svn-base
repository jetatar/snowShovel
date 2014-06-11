#ifndef SNS_TSnDatabase
#define SNS_TSnDatabase

#include <Rtypes.h>
class THashTable;
class TMap;

#include "NSnDatabase.h"
class TSnConnection;
class TSnDbKey;

class TSnDatabase {
 private:
   THashTable* fConnections;     // the list of connections
   TMap*       fCache;           // cache of data already obtained from DB
   
 protected:
   void                    AddToCache(const TSnDbKey& key, TObject& obj);
   
 public:
   TSnDatabase() : fConnections(0), fCache(0) {}
   TSnDatabase(const Bool_t); // this one allocates memory
   virtual ~TSnDatabase();
   
   
   Bool_t                  OpenConnection(TSnConnection* c);
   const TSnConnection*    GetConnection(const Char_t* cname) const;
         TSnConnection*    GetConnection(const Char_t* cname);
   void                    CloseConnection(const Char_t* cname);
   
   // intentionally only a const-object getter
   const TObject&          Get(TSnDbKey& key);
   NSnDatabase::EWriteType WriteNoCache(TSnDbKey& key, const TObject& obj);
   NSnDatabase::EWriteType Write(TSnDbKey& key, TObject& obj,
				 const Bool_t cache=kTRUE);
   
   TObject*                Reclaim(TSnDbKey& key);
   
   
   static TSnDatabase&     GetDB() {
      // return the database object
      // only initialize once, using the specified type of DB
      static TSnDatabase* theDB = new TSnDatabase(kTRUE);
      return *theDB;
   }
   
   ClassDef(TSnDatabase, 1); // a database
   
};


#endif // SNS_TSnDatabase
