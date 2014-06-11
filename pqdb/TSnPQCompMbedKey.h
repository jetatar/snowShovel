#ifndef SNS_TSnPQCompMbedKey
#define SNS_TSnPQCompMbedKey

#include <TString.h>

#include "TSnPQComponentKey.h"

class TSnPQCompMbedKey : public TSnPQComponentKey {
 private:
   TString     fMac;

   static const Char_t* kIdCol;
   static const Char_t* kMacCol;
   static const Char_t* kModelCol;
   static const Char_t* kMbedTbl;
   static const Char_t* kCompType;
   
 protected:
   virtual void GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res);
   virtual NSnDatabase::EWriteType WriteToPQDb(pqxx::dbtransaction& trans, 
				       const TObject& obj);
   virtual TObject* BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t   FetchPrimKey(pqxx::dbtransaction& trans);
   
#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQCompMbedKey() {}
   TSnPQCompMbedKey(const Char_t* mac) : fMac(mac) {}
   virtual ~TSnPQCompMbedKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return !(fMac.IsNull()); }

   virtual const Char_t* GetCompType() const { return kCompType; }
   
   
   ClassDef(TSnPQCompMbedKey, 1); // key to an mbed component in the db
};

#endif // SNS_TSnPQCompMbedKey
