#ifndef SNS_TSnPQCompAfarKey
#define SNS_TSnPQCompAfarKey

#include "TString.h"

#include "TSnPQComponentKey.h"


class TSnPQCompAfarKey : public TSnPQComponentKey {
 private:
   TString              fMacadr; // the Afar's mac address
   
   static const Char_t* kIdCol;
   static const Char_t* kMacCol;
   static const Char_t* kIpCol;
   static const Char_t* kModelCol;
   static const Char_t* kSerialCol;
   static const Char_t* kAfarTbl;
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
   TSnPQCompAfarKey() {}
   TSnPQCompAfarKey(const Char_t* macadr) : fMacadr(macadr) {}
   virtual ~TSnPQCompAfarKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return !(fMacadr.IsNull()); }

   virtual const Char_t* GetCompType() const { return kCompType; }

   ClassDef(TSnPQCompAfarKey, 1); // key to an afar component db entry
};

#endif // SNS_TSnPQCompAfarKey
