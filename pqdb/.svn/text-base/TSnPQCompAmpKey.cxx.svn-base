#include "TSnPQCompAmpKey.h"

#include <TClass.h>
#include <TTimeStamp.h>

#include "TSnCompAmp.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>

ClassImp(TSnPQCompAmpKey);


const Char_t* TSnPQCompAmpKey::kIdCol        = "component_id";
const Char_t* TSnPQCompAmpKey::kNumCol       = "num";
const Char_t* TSnPQCompAmpKey::kDateCnstrCol = "date_constructed";
const Char_t* TSnPQCompAmpKey::kModelCol     = "model";
const Char_t* TSnPQCompAmpKey::kAmpTbl       = "component_amp";
const Char_t* TSnPQCompAmpKey::kCompType     = "amp";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompAmpKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCompAmpKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   pi(fNum);
}

Bool_t TSnPQCompAmpKey::IsEqual(const TObject* obj) const { 
  Bool_t eq = TSnPQComponentKey::IsEqual(obj);
  if (eq) {
    const TSnPQCompAmpKey* ak =
      dynamic_cast<const TSnPQCompAmpKey*>(obj);
    eq = (fNum == ak->fNum);
  }
  return eq;
}

TString TSnPQCompAmpKey::AsString() const {
  TString s(TSnPQComponentKey::AsString());
  s += ", num=";
  s += fNum;
  return s;
}

Bool_t TSnPQCompAmpKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  if (IsCompIDcached()) {
    return kTRUE;
  } else {
    // fetch from db
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
		     kIdCol, kAmpTbl, kNumCol));
    AddVarToSQL(trans, sql, fNum);
    pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
    if (pkr.size()==1) {
      SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
      return kTRUE;
    } else if (pkr.size()>1) {
      throw std::runtime_error(Form(
         "<TSnPQCompAmpKey::FetchPrimKey>: "
	 "Invalid number of primary key rows "
	 "selected [%u]",
	 static_cast<UInt_t>(pkr.size())));
    }
  }
  return kFALSE;
}

void TSnPQCompAmpKey::GetFromPQDb(pqxx::dbtransaction& trans,
				  pqxx::result& res) {
  // get component ID
  if (FetchPrimKey(trans)) {
    // use it to get the general component values
    TSnPQComponentKey::GetFromPQDb(trans);
    
    // now get the amp part
    TString sql(Form("SELECT %s, %s, %s FROM %s WHERE %s=",
		     kNumCol, kDateCnstrCol, kModelCol,
		     kAmpTbl, kIdCol));
    AddVarToSQL(trans, sql, GetCompID());
    res = trans.exec(sql.Data(), sql.Data());
  } else {
    // not in DB yet
    throw std::runtime_error(Form("<TSnPQCompAmpKey::GetFromPQDb>: "
				  "No component for this key: %s",
				  AsString().Data()));
  }
}

TObject* TSnPQCompAmpKey::BuildObjFromResult(const pqxx::result& res) {
  TSnCompAmp* amp=0;
  if (res.size()==1) {
    pqxx::result::const_iterator row = res.begin();
    // the component
    amp = new TSnCompAmp;
    
    // first build the genearl component parameters
    BuildCompFromResult(*amp);
    
    // build the rest
    amp->SetNum(row->at(kNumCol).as<Int_t>());
    amp->SetModel(row->at(kModelCol).as<const Char_t*>());    
    pqxx::result::field dcnstr = row->at(kDateCnstrCol);
    if (dcnstr.is_null()==false) {
       TTimeStamp datc;
       GetTimestampFromStr(datc, dcnstr.as<const Char_t*>());
       amp->SetDateConstructed(datc);
    }

  } else {
    throw std::runtime_error(
       Form("<TSnPQCompAmpKey::BuildObjFromResult>: "
	    "Unexpected number of rows in result [%u]",
	    static_cast<UInt_t>(res.size())));
  }
  return amp;
}

NSnDatabase::EWriteType TSnPQCompAmpKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompAmp::Class())) {
  
      const TSnCompAmp& amp = dynamic_cast<const TSnCompAmp&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update the amp table
         TString sql(Form("UPDATE %s SET %s=",
		          kAmpTbl, kNumCol));
         AddVarToSQL(trans, sql, amp.GetNum());
         sql += Form(", %s=",kModelCol);
         AddVarToSQL(trans, sql, amp.GetModel());
         if (amp.HasDateConstructed()) {
            sql += Form(", %s=",kDateCnstrCol);
            AddVarToSQL(trans, sql, 
                        GetSQLTimestampUTC(*(amp.GetDateConstructed())));
         }
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the amp table

         TString sql(Form("INSERT INTO %s (%s, %s, %s",
		          kAmpTbl, kIdCol, kNumCol, kModelCol));
         if (amp.HasDateConstructed()) {
            sql += Form(", %s", kDateCnstrCol);
         }
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, amp.GetNum());
         sql += ", ";
         AddVarToSQL(trans, sql, amp.GetModel());
         if (amp.HasDateConstructed()) {
            sql += ", ";
            AddVarToSQL(trans, sql,
                        GetSQLTimestampUTC(*(amp.GetDateConstructed())));
         }
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
           "<TSnPQCompAmpKey::WriteToPQDb>: "
           "Unknown write type [%d] from component key.",
           static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompAmpKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }
  
  return wt;
}



