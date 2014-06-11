#ifndef SNS_TSnPQDAQConfigKey
#define SNS_TSnPQDAQConfigKey

#include "TSnPQDbKey.h"

class TSnPQDAQConfigKey : public TSnPQDbKey {
 private:
   TString           fLabel;  //! the primary key, daq config label
   
   // the daq_config table
   static const Char_t* kDConfTbl;
   static const Char_t* kLblCol;
   static const Char_t* kDateBltCol;
   static const Char_t* kUsageCol;
   static const Char_t* kUserCol;
   static const Char_t* kDacIdCol;
   static const Char_t* kPlaIdCol;
   static const Char_t* kTrgIdCol;
   static const Char_t* kCommIdCol;
   static const Char_t* kAmpIdCol;
   static const Char_t* kDescCol;
   static const Char_t* kLPCommIdCol;
   static const Char_t* kRunModCol;
   static const Char_t* kHrtBtCol;
   static const Char_t* kStrmHiLoCol;
   static const Char_t* kWvLoseLSBCol;
   static const Char_t* kWvLoseMSBCol;
   static const Char_t* kWvBaselineCol;
   static const Char_t* kDatPackCol;
   static const Char_t* kPwrModeCol;
   static const Char_t* kBVToLowPwrCol;
   static const Char_t* kBVFromLowPwrCol;
   static const Char_t* kWchDogCol;
   static const Char_t* kVoltChkPerCol;
   
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
   TSnPQDAQConfigKey() {}
   TSnPQDAQConfigKey(const Char_t* lbl) : fLabel(lbl) {}
   virtual ~TSnPQDAQConfigKey() {}
   
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return !(fLabel.IsNull()); }
   
   ClassDef(TSnPQDAQConfigKey, 1); // key to a DAQ config in the db
};

#endif // SNS_TSnPQDAQConfigKey
