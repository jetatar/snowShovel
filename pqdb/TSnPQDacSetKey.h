#ifndef SNS_TSnPQDacSetKey
#define SNS_TSnPQDacSetKey

#include "TSnPQDbKey.h"
class TSnDacSet;

class TSnPQDacSetKey : public TSnPQDbKey {
 private:
   Int_t                fId;     //! the dac set id (defaults to -1, undef)
   
   // the dac table
   static const Char_t* kDCTbl;
   static const Char_t* kDCidCol;
   static const Char_t* kDCchanCol;
   static const Char_t* kDCvalCol;
   static const Char_t* kDCtypeCol;

   // the dac_set table
   static const Char_t* kDSTbl;
   static const Char_t* kDSidCol;

   // the dac_type table
   static const Char_t* kDTTbl;
   static const Char_t* kDTnumCol;
   static const Char_t* kDTdescCol;
   
 protected:
   virtual void      GetFromPQDb(pqxx::dbtransaction& trans,
                                 pqxx::result& res);
   virtual NSnDatabase::EWriteType
             WriteToPQDb(pqxx::dbtransaction& trans,
                         const TObject& obj);
   virtual TObject*  BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t    FetchPrimKey(pqxx::dbtransaction& trans);

   Bool_t    IsAlreadyInDb(pqxx::dbtransaction& trans,
                           const TSnDacSet& dacset);

#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQDacSetKey(const Int_t dacSetId=-1) : fId(dacSetId) {}
   virtual ~TSnPQDacSetKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;

   Int_t                 GetId() const { return fId; }
   
   virtual Bool_t        IsPKvalid() const { return fId>-1; }

   ClassDef(TSnPQDacSetKey, 1); // key to a DAC set in the db
};

#endif // SNS_TSnPQDacSetKey
