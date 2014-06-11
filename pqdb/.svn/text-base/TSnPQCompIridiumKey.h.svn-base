#ifndef SNS_TSnPQCompIridiumKey
#define SNS_TSnPQCompIridiumKey

#include <TString.h>

#include "TSnPQComponentKey.h"


class TSnPQCompIridiumKey : public TSnPQComponentKey {
 private:
   TString        fIMEI;
   
   static const Char_t* kIdCol;
   static const Char_t* kModelCol;
   static const Char_t* kEmailCol;
   static const Char_t* kIMEICol;
   static const Char_t* kIrdTbl;
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
   TSnPQCompIridiumKey() {}
   TSnPQCompIridiumKey(const Char_t* imei) : fIMEI(imei) {}
   virtual ~TSnPQCompIridiumKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return !(fIMEI.IsNull()); }

   virtual const Char_t* GetCompType() const { return kCompType; }
   
   ClassDef(TSnPQCompIridiumKey, 1); // key to an iridium component in the db
   
};

#endif // SNS_TSnPQCompIridiumKey
