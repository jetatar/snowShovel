#include "TSnPQCompIridiumKey.h"

#include <TClass.h>

#include "TSnCompIridium.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>


ClassImp(TSnPQCompIridiumKey);

const Char_t* TSnPQCompIridiumKey::kIdCol    = "component_id";
const Char_t* TSnPQCompIridiumKey::kModelCol = "model";
const Char_t* TSnPQCompIridiumKey::kEmailCol = "receiving_email";
const Char_t* TSnPQCompIridiumKey::kIMEICol  = "imei";
const Char_t* TSnPQCompIridiumKey::kIrdTbl   = "component_iridium";
const Char_t* TSnPQCompIridiumKey::kCompType = "iridium";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompIridiumKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("varchar", pqxx::prepare::treat_string);
}
#endif

void TSnPQCompIridiumKey::AddPKValToPrepSQL(
                                pqxx::prepare::invocation& pi) const {
   pi(fIMEI.Data());
}

Bool_t TSnPQCompIridiumKey::IsEqual(const TObject* obj) const { 
   Bool_t eq = TSnPQComponentKey::IsEqual(obj);
   if (eq) {
      const TSnPQCompIridiumKey* ak =
         dynamic_cast<const TSnPQCompIridiumKey*>(obj);
      eq = fIMEI.CompareTo(ak->fIMEI)==0;
   }
   return eq;
}

TString TSnPQCompIridiumKey::AsString() const {
   TString s(TSnPQComponentKey::AsString());
   s += ", IMEI=";
   s += fIMEI.Data();
   return s;
}

Bool_t TSnPQCompIridiumKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   if (IsCompIDcached()) {
      return kTRUE;
   } else {
      // fetch from db
      TString sql(Form("SELECT %s FROM %s WHERE %s=",
		       kIdCol, kIrdTbl, kIMEICol));
      AddVarToSQL(trans, sql, fIMEI.Data());
      pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
      if (pkr.size()==1) {
         SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
         return kTRUE;
      } else if (pkr.size()>1) {
         throw std::runtime_error(Form(
            "<TSnPQCompIridiumKey::FetchPrimKey>: "
	    "Invalid number of primary key rows "
	    "selected [%u]",
	    static_cast<UInt_t>(pkr.size())));
      }
   }
   return kFALSE;
}

void TSnPQCompIridiumKey::GetFromPQDb(pqxx::dbtransaction& trans,
				      pqxx::result& res) {
   // get component ID
   if (FetchPrimKey(trans)) {
      // use it to get the general component values
      TSnPQComponentKey::GetFromPQDb(trans);

      // now get the rest
      TString sql(Form("SELECT %s, %s, %s FROM %s WHERE %s=",
		       kModelCol, kEmailCol, kIMEICol,
		       kIrdTbl, kIdCol));
      AddVarToSQL(trans, sql, GetCompID());
      res = trans.exec(sql.Data(), sql.Data());
   } else {
      // not in DB yet
      throw std::runtime_error(Form("<TSnPQCompIridiumKey::GetFromPQDb>: "
				    "No component for this key: %s",
				    AsString().Data()));
   }
}

TObject* TSnPQCompIridiumKey::BuildObjFromResult(const pqxx::result& res) {
   TSnCompIridium* ird=0;
   if (res.size()==1) {
      pqxx::result::const_iterator row = res.begin();
      // the component
      ird = new TSnCompIridium;

      // first build the genearl component parameters
      BuildCompFromResult(*ird);

      // build the rest
      ird->SetModel(row->at(kModelCol).as<const Char_t*>());    
      ird->SetEmail(row->at(kEmailCol).as<const Char_t*>());
      ird->SetIMEI(row->at(kIMEICol).as<const Char_t*>());
   
   } else {
      throw std::runtime_error(
         Form("<TSnPQCompIridiumKey::BuildObjFromResult>: "
	      "Unexpected number of rows in result [%u]",
	      static_cast<UInt_t>(res.size())));
   }
   return ird;
}

NSnDatabase::EWriteType TSnPQCompIridiumKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompIridium::Class())) {
      const TSnCompIridium& ird = dynamic_cast<const TSnCompIridium&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);
      if (wc==NSnDatabase::kUpdate) {
         // update the table
         TString sql(Form("UPDATE %s SET %s=",
		          kIrdTbl, kModelCol));
         AddVarToSQL(trans, sql, ird.GetModel());
         sql += Form(", %s=",kEmailCol);
         AddVarToSQL(trans, sql, ird.GetEmail());
         sql += Form(", %s=",kIMEICol);
         AddVarToSQL(trans, sql, ird.GetIMEI());
         sql += Form(" WHERE %s=", kIdCol);
         sql += GetCompID();

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the table

         TString sql(Form("INSERT INTO %s (%s, %s, %s, %s",
		          kIrdTbl, kIdCol, kModelCol, kEmailCol, kIMEICol));
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, ird.GetModel());
         sql += ", ";
         AddVarToSQL(trans, sql, ird.GetEmail());
         sql += ", ";
         AddVarToSQL(trans, sql, ird.GetIMEI());
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
            "<TSnPQCompIridiumKey::WriteToPQDb>: "
            "Unknown write type [%d] from component key.",
            static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompIridiumKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }

   return wt;
}






