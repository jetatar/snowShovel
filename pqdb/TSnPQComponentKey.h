#ifndef SNS_TSnPQComponentKey
#define SNS_TSnPQComponentKey

#include "TSnPQDbKey.h"
class TSnComponent;

class TSnPQComponentKey : public TSnPQDbKey {
 public:
   static const Char_t* kCompTypeCol;
   static const Char_t* kDateInstCol;
   static const Char_t* kDateRemvCol;
   static const Char_t* kIdCol;
   static const Char_t* kCompTbl;

 private:
   Int_t          fCompID;       // component ID. must be obtained from DB
   pqxx::result*  fRes;          // query result of this base key
   
 protected:
   virtual void GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res)=0;
   virtual NSnDatabase::EWriteType
              WriteToPQDb(pqxx::dbtransaction& trans, const TObject& obj);
   virtual TObject* BuildObjFromResult(const pqxx::result& res)=0;
   virtual Bool_t   FetchPrimKey(pqxx::dbtransaction& trans)=0;
   
   // these store the results of the base key
   // to be called by derived keys
   void GetFromPQDb(pqxx::dbtransaction& trans);
   void BuildCompFromResult(TSnComponent& comp) const;
   
   // not to be called
   TObject* BuildObjFromResult();
   
   // intentionally not public
   Int_t  GetCompID()         const { return fCompID; }
   void   SetCompID(const Int_t id) { fCompID = id; SetPKcached(kTRUE); }
   Bool_t IsCompIDcached()    const { return IsPKcached(); }
   
#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQComponentKey() : fCompID(-1), fRes(0) {}
   virtual ~TSnPQComponentKey();
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;

   virtual Bool_t        IsPKvalid() const { return fCompID>-1; }
   
   virtual const Char_t* GetCompType() const=0;
   
   ClassDef(TSnPQComponentKey, 1); // ABC for a hardware component
   
};

#endif // SNS_TSnPQComponentKey
