#include "TSnPQCompMbedKey.h"

#include <TClass.h>

#include "TSnCompMbed.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>

ClassImp(TSnPQCompMbedKey);

const Char_t* TSnPQCompMbedKey::kIdCol    = "component_id";
const Char_t* TSnPQCompMbedKey::kMacCol   = "mac_address";
const Char_t* TSnPQCompMbedKey::kModelCol = "model";
const Char_t* TSnPQCompMbedKey::kMbedTbl  = "component_mbed";
const Char_t* TSnPQCompMbedKey::kCompType = "mbed";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompMbedKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("varchar", pqxx::prepare::treat_string);
}
#endif

void TSnPQCompMbedKey::AddPKValToPrepSQL(
                                pqxx::prepare::invocation& pi) const {
   pi(fMac.Data());
}

Bool_t TSnPQCompMbedKey::IsEqual(const TObject* obj) const { 
   Bool_t eq = TSnPQComponentKey::IsEqual(obj);
   if (eq) {
      const TSnPQCompMbedKey* ak =
         dynamic_cast<const TSnPQCompMbedKey*>(obj);
      eq = fMac.CompareTo(ak->fMac)==0;
   }
   return eq;
}

TString TSnPQCompMbedKey::AsString() const {
   TString s(TSnPQComponentKey::AsString());
   s += ", MAC=";
   s += fMac.Data();
   return s;
}

 Bool_t TSnPQCompMbedKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   if (IsCompIDcached()) {
      return kTRUE;
   } else {
      // fetch from db
      TString sql(Form("SELECT %s FROM %s WHERE %s=",
		       kIdCol, kMbedTbl, kMacCol));
      AddVarToSQL(trans, sql, fMac.Data());
      pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
      if (pkr.size()==1) {
         SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
         return kTRUE;
      } else if (pkr.size()>1) {
         throw std::runtime_error(Form(
            "<TSnPQCompMbedKey::FetchPrimKey>: "
	    "Invalid number of primary key rows "
	    "selected [%u]",
	    static_cast<UInt_t>(pkr.size())));
      }
   }
   return kFALSE;
}

void TSnPQCompMbedKey::GetFromPQDb(pqxx::dbtransaction& trans,
				   pqxx::result& res) {
   // get component ID
   if (FetchPrimKey(trans)) {
      // use it to get the general component values
      TSnPQComponentKey::GetFromPQDb(trans);

      // now get the rest
      TString sql(Form("SELECT %s, %s FROM %s WHERE %s=",
		       kMacCol, kModelCol,
		       kMbedTbl, kIdCol));
      AddVarToSQL(trans, sql, GetCompID());
      res = trans.exec(sql.Data(), sql.Data());
   } else {
      // not in DB yet
      throw std::runtime_error(Form("<TSnPQCompMbedKey::GetFromPQDb>: "
				    "No component for this key: %s",
				    AsString().Data()));
   }
}

TObject* TSnPQCompMbedKey::BuildObjFromResult(const pqxx::result& res) {
   TSnCompMbed* mb=0;
   if (res.size()==1) {
      pqxx::result::const_iterator row = res.begin();
      // the component
      mb = new TSnCompMbed;

      // first build the genearl component parameters
      BuildCompFromResult(*mb);

      // build the rest
      mb->SetMAC(row->at(kMacCol).as<const Char_t*>());
      mb->SetModel(row->at(kModelCol).as<const Char_t*>());
   
   } else {
      throw std::runtime_error(
         Form("<TSnPQCompMbedKey::BuildObjFromResult>: "
	      "Unexpected number of rows in result [%u]",
	      static_cast<UInt_t>(res.size())));
   }
   return mb;
}

NSnDatabase::EWriteType TSnPQCompMbedKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompMbed::Class())) {
      const TSnCompMbed& mb = dynamic_cast<const TSnCompMbed&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update the table
         TString sql(Form("UPDATE %s SET %s=",
		          kMbedTbl, kModelCol));
         AddVarToSQL(trans, sql, mb.GetModel());
         sql += Form(", %s=",kMacCol);
         AddVarToSQL(trans, sql, mb.GetMAC());
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the table

         TString sql(Form("INSERT INTO %s (%s, %s, %s",
		          kMbedTbl, kIdCol, kModelCol, kMacCol));
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, mb.GetModel());
         sql += ", ";
         AddVarToSQL(trans, sql, mb.GetMAC());
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
            "<TSnPQCompMbedKey::WriteToPQDb>: "
            "Unknown write type [%d] from component key.",
            static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompMbedKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }

   return wt;
}


  
