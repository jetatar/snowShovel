#ifndef SNS_TSnPQAmpOnSetKey
#define SNS_TSnPQAmpOnSetKey

#include "TSnPQDbKey.h"
class TSnAmpOnSet;

class TSnPQAmpOnSetKey : public TSnPQDbKey {
 private:
   Int_t       fId;  //! the amp on set id (defaults to -1, undef)
   
   // the amp_on table
   static const Char_t* kATbl;
   static const Char_t* kAidCol;
   static const Char_t* kAchanCol;
   static const Char_t* kAvalCol;
   
   // the amp_on_set table
   static const Char_t* kASTbl;
   static const Char_t* kASidCol;

 protected:
   virtual void      GetFromPQDb(pqxx::dbtransaction& trans,
                                 pqxx::result& res);
   virtual NSnDatabase::EWriteType
             WriteToPQDb(pqxx::dbtransaction& trans,
                         const TObject& obj);
   virtual TObject*  BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t    FetchPrimKey(pqxx::dbtransaction& trans);

   Bool_t    IsAlreadyInDb(pqxx::dbtransaction& trans,
                           const TSnAmpOnSet& dacset);

#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQAmpOnSetKey(const Int_t id=-1) : fId(id) {}
   virtual ~TSnPQAmpOnSetKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;

   Int_t                 GetId() const { return fId; }
   
   virtual Bool_t        IsPKvalid() const { return fId>-1; }   
   
   ClassDef(TSnPQAmpOnSetKey, 1); // a key to an amp on set
};

#endif // SNS_TSnPQAmpOnSetKey
