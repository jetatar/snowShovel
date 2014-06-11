#ifndef SNS_TSnPQTransactors
#define SNS_TSnPQTransactors

#include <pqxx/transaction>
#include <pqxx/transactor>
#include <pqxx/result>

#include <Rtypes.h>

#include <stdexcept>

class TObject;

#include "NSnDatabase.h"
class TSnPQDbKey;

//////////////////////////////////////////////////////////////////////////
//
// TSnPQTransactor, TSnPQReadTransactor, TSnPQWriteTransactor
//
// A bit convoluted, but this allows the use of libpqxx's transactor
// framework, while not requiring a ton of transactor classes to be
// written (otherwise one for each SQL statement).
//
// Instead, generic read/write transactors are defined here. These
// transactors will not be seen by users making a TSnPQDbKey; instead
// they need only define GetFromPQDb, BuildObjFromResult and
// WriteToPQDb. The transactors are handled by the base class TSnPQDbKey.
//
// Note that this means there is one transactor for every SQL statement.
// That is, if a DbKey performs multiple transaction.exec() statements in
// a single GetFromPQDb, for example, and any one of them fails, then the
// entire GetFromPQDb will be repeated. This is most likely the desirable
// behavior for inserts and updates (to prevent race conditions), but
// may not necessarily be ideal for selects. It should be safe, however.
//
//////////////////////////////////////////////////////////////////////////

class TSnPQTransactor : public pqxx::transactor< pqxx::work > {
 protected:
   TSnPQDbKey*    fDbKey;
   
 public:
   TSnPQTransactor() : fDbKey(0) {}
   TSnPQTransactor(const Char_t* name, TSnPQDbKey* key) : 
      pqxx::transactor< pqxx::work >(name),
      fDbKey(key) {}
   virtual ~TSnPQTransactor() {}
   
   virtual void operator() (pqxx::work& trans)=0;
   
   Bool_t IsKeyOk() const {
      if (fDbKey!=0) {
         return kTRUE;
      } else {
         throw std::runtime_error("<TSnPQTransactor::IsKeyOk>: "
            "Null DB key.");
      }
      return kFALSE;
   }
   
};

class TSnPQReadTransactor : public TSnPQTransactor {
 private:
   TObject*&    fObj;
   pqxx::result fRes;
   
 public:
   TSnPQReadTransactor(const Char_t* name,
                       TSnPQDbKey* key,
                       TObject*& obj) :
      TSnPQTransactor(name, key),
      fObj(obj) {
   }
   
   void operator() (pqxx::work& trans);
   void on_commit();
   
};

class TSnPQWriteTransactor : public TSnPQTransactor {
 private:
   const TObject*          fObj;
   NSnDatabase::EWriteType& fWt;

 public:
   TSnPQWriteTransactor(const Char_t* name,
                        TSnPQDbKey* key,
                        const TObject& obj,
			NSnDatabase::EWriteType& wt) :
     TSnPQTransactor(name, key),
     fObj(&obj),
     fWt(wt) {
   }
   
   void operator() (pqxx::work& trans);

};

#endif // SNS_TSnPQTransactors
