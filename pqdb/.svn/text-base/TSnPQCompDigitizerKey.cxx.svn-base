#include "TSnPQCompDigitizerKey.h"

#include <TClass.h>

#include "TSnCompDigitizer.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>


ClassImp(TSnPQCompDigitizerKey);


const Char_t* TSnPQCompDigitizerKey::kIdCol     = "component_id";
const Char_t* TSnPQCompDigitizerKey::kNumCol    = "num";
const Char_t* TSnPQCompDigitizerKey::kModelCol  = "model";
const Char_t* TSnPQCompDigitizerKey::kDigiTbl   = "component_digitizer";
const Char_t* TSnPQCompDigitizerKey::kCompType  = "digitizer";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompDigitizerKey::AddPKToPrepSQL(
                              pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCompDigitizerKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(fNum);
}

Bool_t TSnPQCompDigitizerKey::IsEqual(const TObject* obj) const { 
  Bool_t eq = TSnPQComponentKey::IsEqual(obj);
  if (eq) {
    const TSnPQCompDigitizerKey* ak =
      dynamic_cast<const TSnPQCompDigitizerKey*>(obj);
    eq = (fNum == ak->fNum);
  }
  return eq;
}

TString TSnPQCompDigitizerKey::AsString() const {
   TString s(TSnPQComponentKey::AsString());
   s += ", num=";
   s += fNum;
   return s;
}

Bool_t TSnPQCompDigitizerKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  if (IsCompIDcached()) {
    return kTRUE;
  } else {
    // fetch from db
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
		     kIdCol, kDigiTbl, kNumCol));
    AddVarToSQL(trans, sql, fNum);
    pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
    if (pkr.size()==1) {
      SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
      return kTRUE;
    } else if (pkr.size()>1) {
      throw std::runtime_error(Form(
         "<TSnPQCompDigitizerKey::FetchPrimKey>: "
	 "Invalid number of primary key rows "
	 "selected [%u]",
	 static_cast<UInt_t>(pkr.size())));
    }
  }
  return kFALSE;
}

void TSnPQCompDigitizerKey::GetFromPQDb(pqxx::dbtransaction& trans,
				        pqxx::result& res) {
  // get component ID
  if (FetchPrimKey(trans)) {
    // use it to get the general component values
    TSnPQComponentKey::GetFromPQDb(trans);
    
    // now get the amp part
    TString sql(Form("SELECT %s, %s FROM %s WHERE %s=",
		     kNumCol, kModelCol,
		     kDigiTbl, kIdCol));
    AddVarToSQL(trans, sql, GetCompID());
    res = trans.exec(sql.Data(), sql.Data());
  } else {
    // not in DB yet
    throw std::runtime_error(Form("<TSnPQCompDigitizerKey::GetFromPQDb>: "
				  "No component for this key: %s",
				  AsString().Data()));
  }
}

TObject* TSnPQCompDigitizerKey::BuildObjFromResult(const pqxx::result& res) {
  TSnCompDigitizer* digi=0;
  if (res.size()==1) {
    pqxx::result::const_iterator row = res.begin();
    // the component
    digi = new TSnCompDigitizer;
    
    // first build the genearl component parameters
    BuildCompFromResult(*digi);
    
    // build the rest
    digi->SetNum(row->at(kNumCol).as<Int_t>());
    digi->SetModel(row->at(kModelCol).as<const Char_t*>());    
    
  } else {
    throw std::runtime_error(
       Form("<TSnPQCompDigitizerKey::BuildObjFromResult>: "
	    "Unexpected number of rows in result [%u]",
	    static_cast<UInt_t>(res.size())));
  }
  return digi;
}

NSnDatabase::EWriteType TSnPQCompDigitizerKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompDigitizer::Class())) {
      const TSnCompDigitizer& amp = dynamic_cast<const TSnCompDigitizer&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update the amp table
         TString sql(Form("UPDATE %s SET %s=",
		          kDigiTbl, kNumCol));
         AddVarToSQL(trans, sql, amp.GetNum());
         sql += Form(", %s=",kModelCol);
         AddVarToSQL(trans, sql, amp.GetModel());
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());
         
         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the amp table

         TString sql(Form("INSERT INTO %s (%s, %s, %s",
		          kDigiTbl, kIdCol, kNumCol, kModelCol));
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, amp.GetNum());
         sql += ", ";
         AddVarToSQL(trans, sql, amp.GetModel());
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
           "<TSnPQCompDigitizerKey::WriteToPQDb>: "
           "Unknown write type [%d] from component key.",
           static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompDigitizerKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }

  return wt;
}



