#ifndef SNS_TSnPQPlaSetKey
#define SNS_TSnPQPlaSetKey

#include "TSnPQDbKey.h"
class TSnPlaSet;

class TSnPQPlaSetKey : public TSnPQDbKey {
 private:
   Int_t          fId;  //! cached pla set id (defaults to -1, meaning undef)
   
   // the pla table
   static const Char_t* kPTbl;
   static const Char_t* kPidCol;
   static const Char_t* kPnumCol;
   static const Char_t* kPvalCol;

   // the pla_num table
   static const Char_t* kPNTbl;
   static const Char_t* kPNnumCol;

   // the pla_set table
   static const Char_t* kPSTbl;
   static const Char_t* kPSidCol;

 protected:
   virtual void      GetFromPQDb(pqxx::dbtransaction& trans,
                                 pqxx::result& res);
   virtual NSnDatabase::EWriteType
             WriteToPQDb(pqxx::dbtransaction& trans,
                         const TObject& obj);
   virtual TObject*  BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t    FetchPrimKey(pqxx::dbtransaction& trans);

   Bool_t    IsAlreadyInDb(pqxx::dbtransaction& trans,
                           const TSnPlaSet& dacset);

#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQPlaSetKey(const Int_t plaSetId=-1) : fId(plaSetId) {}
   virtual ~TSnPQPlaSetKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;

   Int_t                 GetId() const { return fId; }
   
   virtual Bool_t        IsPKvalid() const { return fId>-1; }

   ClassDef(TSnPQPlaSetKey, 1); // key to a pla set in the db
};

#endif // SNS_TSnPQPlaSetKey
