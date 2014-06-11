#include "TSnPQCompMotherboardKey.h"

#include <TClass.h>
#include <TTimeStamp.h>

#include "TSnDatabase.h"
#include "TSnCompMotherboard.h"
#include "TSnCompFpga.h"
#include "TSnPQCompFpgaKey.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>


ClassImp(TSnPQCompMotherboardKey);

const Char_t* TSnPQCompMotherboardKey::kIdCol         = "component_id";
const Char_t* TSnPQCompMotherboardKey::kNumCol        = "num";
const Char_t* TSnPQCompMotherboardKey::kFpgaCol       = "fpga_id";
const Char_t* TSnPQCompMotherboardKey::kDateCnstrCol  = "date_constructed";
const Char_t* TSnPQCompMotherboardKey::kModelCol      = "model";
const Char_t* TSnPQCompMotherboardKey::kMbdTbl        = "component_motherboard";
const Char_t* TSnPQCompMotherboardKey::kCompType      = "motherboard";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompMotherboardKey::AddPKToPrepSQL(
                              pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCompMotherboardKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(fNum);
}

Bool_t TSnPQCompMotherboardKey::IsEqual(const TObject* obj) const { 
   Bool_t eq = TSnPQComponentKey::IsEqual(obj);
   if (eq) {
      const TSnPQCompMotherboardKey* ak =
         dynamic_cast<const TSnPQCompMotherboardKey*>(obj);
      eq = fNum == ak->fNum;
   }
   return eq;
}

TString TSnPQCompMotherboardKey::AsString() const {
   TString s(TSnPQComponentKey::AsString());
   s += ", num=";
   s +=  fNum;
   return s;
}

Bool_t TSnPQCompMotherboardKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   if (IsCompIDcached()) {
      return kTRUE;
   } else {
      // fetch from db
      TString sql(Form("SELECT %s FROM %s WHERE %s=",
		       kIdCol, kMbdTbl, kNumCol));
      AddVarToSQL(trans, sql, fNum);
      pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
      if (pkr.size()==1) {
         SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
         return kTRUE;
      } else if (pkr.size()>1) {
         throw std::runtime_error(Form(
            "<TSnPQCompMotherboardKey::FetchPrimKey>: "
	    "Invalid number of primary key rows "
	    "selected [%u]",
	    static_cast<UInt_t>(pkr.size())));
      }
   }
   return kFALSE;
}

void TSnPQCompMotherboardKey::GetFromPQDb(pqxx::dbtransaction& trans,
				       pqxx::result& res) {
   // get component ID
   if (FetchPrimKey(trans)) {
      // use it to get the general component values
      TSnPQComponentKey::GetFromPQDb(trans);

      // now get the rest
      TString sql(Form("SELECT %s, %s, %s, %s FROM %s WHERE %s=",
		       kNumCol, kFpgaCol, kDateCnstrCol, kModelCol,
		       kMbdTbl, kIdCol));
      AddVarToSQL(trans, sql, GetCompID());
      res = trans.exec(sql.Data(), sql.Data());
   } else {
      // not in DB yet
      throw std::runtime_error(Form("<TSnPQCompMotherboardKey::GetFromPQDb>: "
				    "No component for this key: %s",
				    AsString().Data()));
   }
}

TObject* TSnPQCompMotherboardKey::BuildObjFromResult(const pqxx::result& res) {
   TSnCompMotherboard* mb=0;
   if (res.size()==1) {
      pqxx::result::const_iterator row = res.begin();
      // the component
      mb = new TSnCompMotherboard;

      // first build the genearl component parameters
      BuildCompFromResult(*mb);

      // get the connected fpga component (possibly from cache)
      TSnPQCompFpgaKey fpgaKey;
      TSnPQCompFpgaKeyWhisperer::SetCompID(fpgaKey,
                                           row->at(kFpgaCol).as<Int_t>());
      TSnCompFpga* fpga = dynamic_cast<TSnCompFpga*>(
         TSnPQCompFpgaKeyWhisperer::GetFromDb(fpgaKey, TSnDatabase::GetDB()));
      if (fpga==0) {
         throw std::runtime_error(
            "<TSnPQCompMotherboardKey::BuildObjFromResult>: "
            "Could not get FPGA from DB.");
      }
      
      // build the rest
      mb->SetNum(row->at(kNumCol).as<Int_t>());
      mb->SetFpga(*fpga);
      mb->SetModel(row->at(kModelCol).as<const Char_t*>());    
      pqxx::result::field dcnstr = row->at(kDateCnstrCol);
      if (dcnstr.is_null()==false) {
         TTimeStamp datc;
         GetTimestampFromStr(datc, dcnstr.as<const Char_t*>());
         mb->SetDateConstructed(datc);
      }

   } else {
      throw std::runtime_error(
         Form("<TSnPQCompMotherboardKey::BuildObjFromResult>: "
	       "Unexpected number of rows in result [%u]",
	       static_cast<UInt_t>(res.size())));
   }
   return mb;
}

NSnDatabase::EWriteType TSnPQCompMotherboardKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompMotherboard::Class())) {
      const TSnCompMotherboard& mb = 
         dynamic_cast<const TSnCompMotherboard&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);

      // write the fpga via its key
      const TSnCompFpga& fpga = mb.GetFpga();
      TSnPQCompFpgaKey fpgaKey(fpga.GetNum());
      TSnPQCompFpgaKeyWhisperer::WriteToDb(fpgaKey, TSnDatabase::GetDB(), fpga);
      const Int_t fpgaId = TSnPQCompFpgaKeyWhisperer::GetCompID(fpgaKey);
      if (fpgaId<0) {
         throw std::runtime_error("<TSnPQCompMotherboardKey::WriteToPQDb>: "
            "Could not get FPGA component_id from DB");
      }

      if (wc==NSnDatabase::kUpdate) {
         // update the table
         TString sql(Form("UPDATE %s SET %s=",
		          kMbdTbl, kNumCol));
         AddVarToSQL(trans, sql, mb.GetNum());
         sql += Form(", %s=",kFpgaCol);
         AddVarToSQL(trans, sql, fpgaId);
         sql += Form(", %s=",kModelCol);
         AddVarToSQL(trans, sql, mb.GetModel());
         if (mb.HasDateConstructed()) {
            sql += Form(", %s=",kDateCnstrCol);
            AddVarToSQL(trans, sql, 
                        GetSQLTimestampUTC(*(mb.GetDateConstructed())));
         }
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the table

         TString sql(Form("INSERT INTO %s (%s, %s, %s, %s",
		          kMbdTbl, kIdCol, kNumCol, kFpgaCol, kModelCol));
         if (mb.HasDateConstructed()) {
            sql += Form(", %s",kDateCnstrCol);
         }
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, mb.GetNum());
         sql += ", ";
         AddVarToSQL(trans, sql, fpgaId);
         sql += ", ";
         AddVarToSQL(trans, sql, mb.GetModel());
         if (mb.HasDateConstructed()) {
            sql += ", ";
            AddVarToSQL(trans, sql, 
                        GetSQLTimestampUTC(*(mb.GetDateConstructed())));
         }
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
            "<TSnPQCompMotherboardKey::WriteToPQDb>: "
            "Unknown write type [%d] from component key.",
            static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompMotherboardKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }

   return wt;
}





