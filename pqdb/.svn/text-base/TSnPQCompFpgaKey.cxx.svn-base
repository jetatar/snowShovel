#include "TSnPQCompFpgaKey.h"

#include <TClass.h>

#include "TSnCompFpga.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>

ClassImp(TSnPQCompFpgaKey);

const Char_t* TSnPQCompFpgaKey::kIdCol    = "component_id";
const Char_t* TSnPQCompFpgaKey::kNumCol   = "num";
const Char_t* TSnPQCompFpgaKey::kModelCol = "model";
const Char_t* TSnPQCompFpgaKey::kFpgaTbl  = "component_fpga";
const Char_t* TSnPQCompFpgaKey::kCompType = "fpga";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompFpgaKey::AddPKToPrepSQL(
                              pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCompFpgaKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(fNum);
}

Bool_t TSnPQCompFpgaKey::IsEqual(const TObject* obj) const { 
  Bool_t eq = TSnPQComponentKey::IsEqual(obj);
  if (eq) {
    const TSnPQCompFpgaKey* ak =
      dynamic_cast<const TSnPQCompFpgaKey*>(obj);
    eq = (fNum == ak->fNum);
  }
  return eq;
}

TString TSnPQCompFpgaKey::AsString() const {
   TString s(TSnPQComponentKey::AsString());
   s += ", num=";
   s += fNum;
   return s;
}

Bool_t TSnPQCompFpgaKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  if (IsCompIDcached()) {
    return kTRUE;
  } else {
    // fetch from db
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
		     kIdCol, kFpgaTbl, kNumCol));
    AddVarToSQL(trans, sql, fNum);
    pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
    if (pkr.size()==1) {
      SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
      return kTRUE;
    } else if (pkr.size()>1) {
      throw std::runtime_error(Form(
         "<TSnPQCompFpgaKey::FetchPrimKey>: "
	 "Invalid number of primary key rows "
	 "selected [%u]",
	 static_cast<UInt_t>(pkr.size())));
    }
  }
  return kFALSE;
}

void TSnPQCompFpgaKey::GetFromPQDb(pqxx::dbtransaction& trans,
				   pqxx::result& res) {
  // get component ID
  if (FetchPrimKey(trans)) {
    // use it to get the general component values
    TSnPQComponentKey::GetFromPQDb(trans);
    
    // now get the amp part
    TString sql(Form("SELECT %s, %s FROM %s WHERE %s=",
		     kNumCol, kModelCol,
		     kFpgaTbl, kIdCol));
    AddVarToSQL(trans, sql, GetCompID());
    res = trans.exec(sql.Data(), sql.Data());
  } else {
    // not in DB yet
    throw std::runtime_error(Form("<TSnPQCompFpgaKey::GetFromPQDb>: "
				  "No component for this key: %s",
				  AsString().Data()));
  }
}

TObject* TSnPQCompFpgaKey::BuildObjFromResult(const pqxx::result& res) {
  TSnCompFpga* fpga=0;
  if (res.size()==1) {
    pqxx::result::const_iterator row = res.begin();
    // the component
    fpga = new TSnCompFpga;
    
    // first build the genearl component parameters
    BuildCompFromResult(*fpga);
    
    // build the rest
    fpga->SetNum(row->at(kNumCol).as<Int_t>());
    fpga->SetModel(row->at(kModelCol).as<const Char_t*>());    

  } else {
    throw std::runtime_error(
       Form("<TSnPQCompFpgaKey::BuildObjFromResult>: "
	    "Unexpected number of rows in result [%u]",
	    static_cast<UInt_t>(res.size())));
  }
  return fpga;
}

NSnDatabase::EWriteType TSnPQCompFpgaKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompFpga::Class())) {
      const TSnCompFpga& fpga = dynamic_cast<const TSnCompFpga&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update the table
         TString sql(Form("UPDATE %s SET %s=",
		          kFpgaTbl, kNumCol));
         AddVarToSQL(trans, sql, fpga.GetNum());
         sql += Form(", %s=",kModelCol);
         AddVarToSQL(trans, sql, fpga.GetModel());
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the table

         TString sql(Form("INSERT INTO %s (%s, %s, %s",
		          kFpgaTbl, kIdCol, kNumCol, kModelCol));
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, fpga.GetNum());
         sql += ", ";
         AddVarToSQL(trans, sql, fpga.GetModel());
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
            "<TSnPQCompFpgaKey::WriteToPQDb>: "
            "Unknown write type [%d] from component key.",
            static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompFpgaKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }
  
  return wt;
}


