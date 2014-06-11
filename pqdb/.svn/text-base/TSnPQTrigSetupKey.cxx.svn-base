#include "TSnPQTrigSetupKey.h"

#include <TString.h>
#include <TClass.h>
#include <TMap.h>
#include <TParameter.h>

#include "TSnPQConnection.h"
#include "TSnTriggerSetup.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/connection>
#include <pqxx/prepared_statement>

#include <stdexcept>

ClassImp(TSnPQTrigSetupKey);

// the trigger_setup table
const Char_t* TSnPQTrigSetupKey::kTSTbl       = "trigger_setup";
const Char_t* TSnPQTrigSetupKey::kTSidCol     = "id";
const Char_t* TSnPQTrigSetupKey::kTSlogSetCol = "logic_set_id";
const Char_t* TSnPQTrigSetupKey::kTSThrotCol  = "throttle_period";
const Char_t* TSnPQTrigSetupKey::kTSThrmCol   = "thermal_enabled";
const Char_t* TSnPQTrigSetupKey::kTSForcedCol = "forced_period";
// the trigger_logic_set table
const Char_t* TSnPQTrigSetupKey::kTLSetTbl   = "trigger_logic_set";
const Char_t* TSnPQTrigSetupKey::kTLSetIdCol = "id";
// the trigger_logic table
const Char_t* TSnPQTrigSetupKey::kTLTbl     = "trigger_logic";
const Char_t* TSnPQTrigSetupKey::kTLidCol   = "set_id";
const Char_t* TSnPQTrigSetupKey::kTLtypeCol = "type";
const Char_t* TSnPQTrigSetupKey::kTLvalCol  = "val";
// the trigger_logic_type table
const Char_t* TSnPQTrigSetupKey::kTLTypeTbl    = "trigger_logic_type";
const Char_t* TSnPQTrigSetupKey::kTLTypeLblCol = "label";

#if PQXX_VERSION_MAJOR<4
void TSnPQTrigSetupKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQTrigSetupKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   pi(fId);
}

Bool_t TSnPQTrigSetupKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQTrigSetupKey* ck = 
         dynamic_cast<const TSnPQTrigSetupKey*>(obj);
      if ( ck->fId == fId ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQTrigSetupKey::AsString() const {
   TString s("trig setup, id=");
   s += fId;
   return s;
}


void TSnPQTrigSetupKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                    pqxx::result& res) {
   TString sql(Form("SELECT s.%s, s.%s, s.%s, "
                    "l.%s, l.%s "
                    "FROM %s s, %s l "
                    "WHERE s.%s=l.%s and s.%s=",
                    // trigger_setup
                    kTSThrotCol, kTSThrmCol, kTSForcedCol,
                    // trigger_logic
                    kTLtypeCol, kTLvalCol,
                    // from
                    kTSTbl, kTLTbl,
                    // where
                    kTSlogSetCol, kTLidCol, kTSidCol));
   AddVarToSQL(trans, sql, fId);
   
   res = trans.exec(sql.Data(), sql.Data());
}

TObject* TSnPQTrigSetupKey::BuildObjFromResult(const pqxx::result& res) {
   TSnTriggerSetup* ts(0);
   if (res.size()==1) {
      pqxx::result::const_iterator row = res.begin();
      ts = new TSnTriggerSetup;
      ts->SetThrottlePeriodms(row->at(kTSThrotCol).as<UShort_t>());
      ts->SetThermalTriggering(row->at(kTSThrmCol).as<Bool_t>());
      ts->SetForcedPeriod(row->at(kTSForcedCol).as<Float_t>());
      ts->SetTrigLogic(row->at(kTLtypeCol).as<const Char_t*>(),
                       row->at(kTLvalCol).as<UShort_t>());
   } else if (res.size()>1) {
      throw std::runtime_error(Form("<TSnPQTrigSetupKey::BuildObjFromResult>: "
                                    "Received unexpected number of rows [%u] "
                                    "for id=[%d]",
                                    static_cast<UInt_t>(res.size()), fId));
   } // else nothing found   
   return ts;
}

Bool_t TSnPQTrigSetupKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   return CheckUniquePrimKey(trans, kTSidCol, kTSTbl);
}


NSnDatabase::EWriteType TSnPQTrigSetupKey::WriteToPQDb(
                     pqxx::dbtransaction& trans, const TObject& obj) {
   
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnTriggerSetup::Class())) {
      
      const TSnTriggerSetup& trg = dynamic_cast<const TSnTriggerSetup&>(obj);
      if (trg.IsValidForDb()) {
         if (FetchPrimKey(trans)) {
            // update

            throw std::runtime_error(Form("<TSnPQTrigSetupKey::WriteToPQDb>: "
                                          "This key must not be used to update "
                                          "an existing trigger setup. "
                                          "If new values are needed, simply "
                                          "create a new trigger setup."));
         
         } else {
            // insert
            
            // check if the logics already exist
            const TMap* tl = trg.GetTrigLogic();
            TIter nl(tl->MakeIterator());
            TObject* key=0;
            // prepare a trigger logic type statement
            TSnPQConnection::PQxxConn_t* conn = GetConnection()->GetPQConn();
            conn->prepare("selTrigLogicType",
                          Form("SELECT %s FROM %s WHERE %s=$1",
                               kTLTypeLblCol, kTLTypeTbl, kTLTypeLblCol))
#if PQXX_VERSION_MAJOR<4
               ("varchar", pqxx::prepare::treat_string)
#endif
               ;
            // to check if the logic set already exists
            Bool_t newSet=kFALSE;
            UInt_t i=2;
            TString sql(Form("SELECT l1.%s FROM %s l1 ",
                             kTLidCol, kTLTbl));
            while ( (key=nl())!=0 ) {
               // the trigger logic type
               pqxx::result res = trans.prepared("selTrigLogicType")
                  (key->GetName()).exec();
               if (res.size()==0) {
                  // insert it
                  sql = Form("INSERT INTO %s (%s) VALUE ",
                             kTLTypeTbl, kTLTypeLblCol);
                  AddVarToSQL(trans, sql, key->GetName());
                  trans.exec(sql.Data(), sql.Data());
                  newSet = kTRUE;
               } else if (res.size()>1) {
                  throw std::runtime_error(Form(
                                           "<TSnPQTrigSetupKey::WriteToPQDb>: "
                                           "Invalid number of primary key rows "
                                           "selected [%u]",
                                           static_cast<UInt_t>(res.size())));
               } // already in the db
               
               // the trigger logic value(s)
               if (newSet==kFALSE) {
                  sql += Form(" JOIN %s l%u on l1.%s=l%u.%s and l%u.%s=",
                              kTLTbl, i, kTLidCol, i, kTLidCol, i, kTLtypeCol);
                  AddVarToSQL(trans, sql, key->GetName());
                  sql += Form(" and l%u.%s=", i, kTLvalCol);
                  const TParameter<UShort_t>* p = 
                     dynamic_cast<const TParameter<UShort_t>*>(
                        tl->GetValue(key));
                  if (p!=0) {
                     AddVarToSQL(trans, sql, p->GetVal());
                  } else {
                     throw std::runtime_error(Form(
                                          "<TSnPQTrigSetupKey::WriteToPQDb>: "
                                          "Could not get trigger value for "
                                          "type '%s'",key->GetName()));
                  }
                  ++i;
               }

            }
            
            if (newSet==kFALSE) {
               // check if the set exists already
               pqxx::result res = trans.exec(sql.Data(), sql.Data());
               newSet = (res.size() == 0);
            }
            
            if (newSet) {
               // insert it
               
               // first get the set id
               sql = Form("INSERT INTO %s (%s) VALUES (DEFAULT) RETURNING %s",
                          kTLSetTbl, kTLSetIdCol, kTLSetIdCol);
               pqxx::result res = trans.exec(sql.Data(), sql.Data());
               const Int_t id = res.begin()->at(kTLSetIdCol).as<Int_t>();
               
               // now insert the values
               conn->prepare("insTrigLogic",
                             Form("INSERT INTO %s (%s, %s, %s) "
                                  "VALUES ($1, $2, $3)",
                                  kTLTbl, kTLidCol, kTLtypeCol, kTLvalCol))
#if PQXX_VERSION_MAJOR<4
                  ("integer")
                  ("varchar", pqxx::prepare::treat_string)
                  ("smallint")
#endif
                  ;
               TIter ni(tl->MakeIterator());
               while ( (key=ni())!=0 ) {
                  const TParameter<UShort_t>* p = 
                     dynamic_cast<const TParameter<UShort_t>*>(
                        tl->GetValue(key));
                  trans.prepared("insTrigLogic")
                     (id)
                     (key->GetName())
                     (p->GetVal())
                     .exec();
               }
               
               fId = id;
            }
            
            trans.commit();
            wt = NSnDatabase::kInsert;

         }
      } else {
         throw std::runtime_error(Form("<TSnPQTrigSetupKey::WriteToPQDb>: "
            "Specified trigger setup is not valid for writing to DB."));
      }
   } else {
      throw std::runtime_error(Form("<TSnPQTrigSetupKey::WriteToPQDb>: "
                                    "Cannot write object of type [%s].",
                                    obj.IsA()->GetName()));
   }
   
   return wt;
}
