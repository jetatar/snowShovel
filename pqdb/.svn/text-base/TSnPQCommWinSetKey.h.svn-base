#ifndef SNS_TSnPQCommWinSetKey
#define SNS_TSnPQCommWinSetKey

#include "TSnPQDbKey.h"
class TSnCommWinSet;

class TSnPQCommWinSetKey : public TSnPQDbKey {
 private:
   Int_t     fId;  //! the comm win set id (defaults to -1, undef)
   
   // the comm window table
   static const Char_t* kCWTbl;
   static const Char_t* kCWidCol;
   static const Char_t* kCWdurCol;
   static const Char_t* kCWperCol;
   static const Char_t* kCWsendDataCol;
   
 protected:
   virtual void      GetFromPQDb(pqxx::dbtransaction& trans,
                                 pqxx::result& res);
   virtual NSnDatabase::EWriteType
             WriteToPQDb(pqxx::dbtransaction& trans,
                         const TObject& obj);
   virtual TObject*  BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t    FetchPrimKey(pqxx::dbtransaction& trans);

#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQCommWinSetKey(const Int_t id=-1) : fId(id) {}
   virtual ~TSnPQCommWinSetKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   Int_t                 GetId() const { return fId; }

   virtual Bool_t        IsPKvalid() const { return fId>-1; }
   
   ClassDef(TSnPQCommWinSetKey, 1); // key to a communication window in the db
};

#endif // TSnPQCommWinSetKey
