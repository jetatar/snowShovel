#include "TSnPQCompAntennaKey.h"

#include <TString.h>
#include <TClass.h>

#include "TSnCompAntenna.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>


ClassImp(TSnPQCompAntennaKey);

const Char_t* TSnPQCompAntennaKey::kIdCol      = "component_id";
const Char_t* TSnPQCompAntennaKey::kNumCol     = "num";
const Char_t* TSnPQCompAntennaKey::kAntennaTbl = "component_antenna";
const Char_t* TSnPQCompAntennaKey::kCompType   = "antenna";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompAntennaKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCompAntennaKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(fNum);
}

Bool_t TSnPQCompAntennaKey::IsEqual(const TObject* obj) const { 
  Bool_t eq = TSnPQComponentKey::IsEqual(obj);
  if (eq) {
    const TSnPQCompAntennaKey* ak =
      dynamic_cast<const TSnPQCompAntennaKey*>(obj);
    eq = (fNum == ak->fNum);
  }
  return eq;
}

TString TSnPQCompAntennaKey::AsString() const {
  TString s(TSnPQComponentKey::AsString());
  s += ", num=";
  s += fNum;
  return s;
}

Bool_t TSnPQCompAntennaKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  if (IsCompIDcached()) {
    return kTRUE;
  } else {
    // fetch from db
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
                     kIdCol, kAntennaTbl, kNumCol));
    AddVarToSQL(trans, sql, fNum);
    pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
    if (pkr.size()==1) {
      SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
      return kTRUE;
    } else if (pkr.size()>1) {
      throw std::runtime_error(Form(
         "<TSnPQCompAntennaKey::FetchPrimKey>: "
	 "Invalid number of primary key rows "
	 "selected [%u]",
         static_cast<UInt_t>(pkr.size())));
    }
  }
  return kFALSE;
}

void TSnPQCompAntennaKey::GetFromPQDb(pqxx::dbtransaction& trans,
				      pqxx::result& res) {
  // get component ID
  if (FetchPrimKey(trans)) {
    // use it to get the general component values
    TSnPQComponentKey::GetFromPQDb(trans);

    // now get the antenna part
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
		     kNumCol, kAntennaTbl, kIdCol));
    AddVarToSQL(trans, sql, GetCompID());
    res = trans.exec(sql.Data(), sql.Data());
  } else {
    // not in DB yet
    throw std::runtime_error(Form(
       "<TSnPQCompAntennaKey::GetFromPQDb>: "
       "No component for this key: %s",
       AsString().Data()));
  }
}

TObject* TSnPQCompAntennaKey::BuildObjFromResult(const pqxx::result& res) {
  TSnCompAntenna* ant=0;
  if (res.size()==1) {
    pqxx::result::const_iterator row = res.begin();
    // the antenna component
    ant  = new TSnCompAntenna;
    
    // first build the genearl component parameters
    BuildCompFromResult(*ant);
    
    // build the rest
    ant->SetNum(row->at(kNumCol).as<Int_t>());
    
  } else {
    throw std::runtime_error(
       Form("<TSnPQCompAntennaKey::BuildObjFromResult>: "
	    "Unexpected number of rows in result [%u]",
	    static_cast<UInt_t>(res.size())));

  }
  return ant;
}

NSnDatabase::EWriteType TSnPQCompAntennaKey::WriteToPQDb(
					 pqxx::dbtransaction& trans,
					 const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompAntenna::Class())) {
  
      const TSnCompAntenna& ant = dynamic_cast<const TSnCompAntenna&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update antenna table
         TString sql(Form("UPDATE %s SET %s=",
		          kAntennaTbl, kNumCol));
         AddVarToSQL(trans, sql, ant.GetNum());
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert into the antenna table
         TString sql(Form("INSERT INTO %s (%s, %s) VALUES "
		          "(DEFAULT, ",
		          kAntennaTbl, kIdCol, kNumCol));
         sql += ant.GetNum();
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
        throw std::runtime_error(Form(
           "<TSnPQCompAntennaKey::WriteToPQDb>: "
           "Unknown write type [%d] from component key.",
           static_cast<Int_t>(wc)));

      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompAntennaKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }
  
  
  return wt;
}



