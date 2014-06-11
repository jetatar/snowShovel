#include "TSnPQCompAfarKey.h"

#include <TClass.h>

#include "TSnCompAfar.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>

ClassImp(TSnPQCompAfarKey);

const Char_t* TSnPQCompAfarKey::kIdCol     = "component_id";
const Char_t* TSnPQCompAfarKey::kMacCol    = "mac_address";
const Char_t* TSnPQCompAfarKey::kIpCol     = "ip_address";
const Char_t* TSnPQCompAfarKey::kModelCol  = "model";
const Char_t* TSnPQCompAfarKey::kSerialCol = "serial";
const Char_t* TSnPQCompAfarKey::kAfarTbl   = "component_afar";
const Char_t* TSnPQCompAfarKey::kCompType  = "afar";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompAfarKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("varchar", pqxx::prepare::treat_string);
}
#endif

void TSnPQCompAfarKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   pi(fMacadr.Data());
}


Bool_t TSnPQCompAfarKey::IsEqual(const TObject* obj) const {
  Bool_t eq = TSnPQComponentKey::IsEqual(obj);
  if (eq) {
    const TSnPQCompAfarKey* ak =
      dynamic_cast<const TSnPQCompAfarKey*>(obj);
    eq = fMacadr.CompareTo(ak->fMacadr.Data())==0;
  }
  return eq;
}

TString TSnPQCompAfarKey::AsString() const {
  TString s(TSnPQComponentKey::AsString());
  s += ", mac=";
  s += fMacadr.Data();
  return s;
}

Bool_t TSnPQCompAfarKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  if (IsCompIDcached()) {
    return kTRUE;
  } else {
    // fetch it from the DB
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
		     kIdCol, kAfarTbl, kMacCol));
    AddVarToSQL(trans, sql, fMacadr);
    pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
    if (pkr.size()==1) {
      SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
      return kTRUE;
    } else if (pkr.size()>1) {
      throw std::runtime_error(Form(
         "<TSnPQCompAfarKey::FetchPrimKey>: "
	 "Invalid number of primary key rows "
	 "selected [%u]",
	 static_cast<UInt_t>(pkr.size())));
    }
  }
  return kFALSE;
}

void TSnPQCompAfarKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                   pqxx::result& res) {
   
   // get the component ID
   if (FetchPrimKey(trans)) {
      // first get the general component values
      TSnPQComponentKey::GetFromPQDb(trans);
      
      // now fetch the afar part
      TString sql(Form("SELECT %s, %s, %s FROM %s WHERE %s=",
		       kIpCol, kModelCol, kSerialCol,
		       kAfarTbl, kIdCol));
      AddVarToSQL(trans, sql, GetCompID());
      res = trans.exec(sql.Data(), sql.Data());
      
   } else {
      // not in the DB yet
      throw std::runtime_error(Form("<TSnPQCompAfarKey::GetFromPQDb>: "
         "No component for this key: %s",
         AsString().Data()));
   }
}

TObject* TSnPQCompAfarKey::BuildObjFromResult(const pqxx::result& res) {

  TSnCompAfar* afar=0;

  if (res.size()==1) {
    pqxx::result::const_iterator row = res.begin();
    // the afar component
    afar = new TSnCompAfar;
    
    // first build the genearl component parameters
    BuildCompFromResult(*afar);
    
    // build the afar part
    afar->SetMAC(row->at(kMacCol).as<const Char_t*>());
    
    pqxx::result::field ip = row->at(kIpCol);
    if (ip.is_null()==false) {
      afar->SetIP(ip.as<const Char_t*>());
    }
    
    pqxx::result::field model = row->at(kModelCol);
    if (model.is_null()==false) {
      afar->SetModel(model.as<const Char_t*>());
    }
    
    pqxx::result::field serial = row->at(kSerialCol);
    if (serial.is_null()==false) {
      afar->SetSerial(serial.as<const Char_t*>());
    }
    
  } else {
    throw std::runtime_error(
       Form("<TSnPQCompAfarKey::BuildObjFromResult>: "
	    "Unexpected number of rows in result [%u]",
	    static_cast<UInt_t>(res.size())));
  }

  return afar;
}

NSnDatabase::EWriteType TSnPQCompAfarKey::WriteToPQDb(
				   pqxx::dbtransaction& trans,
				   const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompAfar::Class())) {
      const TSnCompAfar& afar = dynamic_cast<const TSnCompAfar&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc = 
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update this table too
         TString sql(Form("UPDATE %s SET %s=", 
		          kAfarTbl, kMacCol));
         AddVarToSQL(trans, sql, afar.GetMAC());
         if (afar.HasIP()) {
            sql += Form(", %s=", kIpCol);
            AddVarToSQL(trans, sql, afar.GetIP());
         }
         if (afar.HasModel()) {
            sql += Form(", %s=", kModelCol);
            AddVarToSQL(trans, sql, afar.GetModel());
         }
         if (afar.HasSerial()) {
            sql += Form(", %s=", kSerialCol);
            AddVarToSQL(trans, sql, afar.GetSerial());
         }
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
        // insert to this table too

         TString sql(Form("INSERT INTO %s (%s, %s",
		          kAfarTbl, kIdCol, kMacCol));
         if (afar.HasIP()) {
            sql += Form(", %s", kIpCol);
         }
         if (afar.HasModel()) {
            sql += Form(", %s", kModelCol);
         }
         if (afar.HasSerial()) {
            sql += Form(", %s", kSerialCol);
         }
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans,sql,afar.GetMAC());
         if (afar.HasIP()) {
            AddVarToSQL(trans, sql, afar.GetIP());
         }
         if (afar.HasModel()) {
            AddVarToSQL(trans, sql, afar.GetModel());
         }
         if (afar.HasSerial()) {
            AddVarToSQL(trans, sql, afar.GetSerial());
         }
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
            "<TSnPQCompAfarKey::WriteToPQDb>: "
            "Unknown write type [%d] from component key.",
            static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompAfarKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }
   return wt;
}
