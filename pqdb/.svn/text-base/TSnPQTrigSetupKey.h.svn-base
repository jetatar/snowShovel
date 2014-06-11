#ifndef SNS_TSnPQTrigSetupKey
#define SNS_TSnPQTrigSetupKey

#include "TSnPQDbKey.h"


class TSnPQTrigSetupKey : public TSnPQDbKey {
 private:
   Int_t     fId;  //! the trigger setup id (defaults to -1, undef)

   // the trigger_setup table
   static const Char_t* kTSTbl;
   static const Char_t* kTSidCol;
   static const Char_t* kTSlogSetCol;
   static const Char_t* kTSThrotCol;
   static const Char_t* kTSThrmCol;
   static const Char_t* kTSForcedCol;
   // the trigger_logic_set table
   static const Char_t* kTLSetTbl;
   static const Char_t* kTLSetIdCol;
   // the trigger_logic table
   static const Char_t* kTLTbl;
   static const Char_t* kTLidCol;
   static const Char_t* kTLtypeCol;
   static const Char_t* kTLvalCol;
   // the trigger_logic_type table
   static const Char_t* kTLTypeTbl;
   static const Char_t* kTLTypeLblCol;

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
   TSnPQTrigSetupKey(const Int_t id=-1) : fId(id) {}
   virtual ~TSnPQTrigSetupKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   Int_t                 GetId() const { return fId; }

   virtual Bool_t        IsPKvalid() const { return fId>-1; }

   ClassDef(TSnPQTrigSetupKey, 1) // key to a trigger setup from the pqdb
};

#endif // SNS_TSnPQTrigSetupKey
