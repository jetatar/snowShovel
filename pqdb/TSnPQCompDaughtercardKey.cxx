#include "TSnPQCompDaughtercardKey.h"

#include <TClass.h>
#include <TTimeStamp.h>

#include "TSnDatabase.h"
#include "TSnCompDaughtercard.h"
#include "TSnCompDigitizer.h"
#include "TSnCompFpga.h"
#include "TSnPQCompDigitizerKey.h"
#include "TSnPQCompFpgaKey.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>



ClassImp(TSnPQCompDaughtercardKey);


const Char_t* TSnPQCompDaughtercardKey::kIdCol       = "component_id";
const Char_t* TSnPQCompDaughtercardKey::kNumCol      = "num";
const Char_t* TSnPQCompDaughtercardKey::kFpgaCol     = "fpga_id";
const Char_t* TSnPQCompDaughtercardKey::kDigCol      = "digitizer_id";
const Char_t* TSnPQCompDaughtercardKey::kDateCnstrCol= "date_constructed";
const Char_t* TSnPQCompDaughtercardKey::kModelCol    = "model";
const Char_t* TSnPQCompDaughtercardKey::kDCTbl       = "component_daughtercard";
const Char_t* TSnPQCompDaughtercardKey::kCompType    = "daughtercard";

#if PQXX_VERSION_MAJOR<4
void TSnPQCompDaughtercardKey::AddPKToPrepSQL(
                              pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCompDaughtercardKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(fNum);
}

Bool_t TSnPQCompDaughtercardKey::IsEqual(const TObject* obj) const { 
  Bool_t eq = TSnPQComponentKey::IsEqual(obj);
  if (eq) {
    const TSnPQCompDaughtercardKey* ak =
      dynamic_cast<const TSnPQCompDaughtercardKey*>(obj);
    eq = (fNum == ak->fNum);
  }
  return eq;
}

TString TSnPQCompDaughtercardKey::AsString() const {
   TString s(TSnPQComponentKey::AsString());
   s += ", num=";
   s += fNum;
   return s;
}

Bool_t TSnPQCompDaughtercardKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  if (IsCompIDcached()) {
    return kTRUE;
  } else {
    // fetch from db
    TString sql(Form("SELECT %s FROM %s WHERE %s=",
		     kIdCol, kDCTbl, kNumCol));
    AddVarToSQL(trans, sql, fNum);
    pqxx::result pkr = trans.exec(sql.Data(), sql.Data());
    if (pkr.size()==1) {
      SetCompID(pkr.begin()->at(kIdCol).as<Int_t>());
      return kTRUE;
    } else if (pkr.size()>1) {
      throw std::runtime_error(Form(
         "<TSnPQCompDaughtercardKey::FetchPrimKey>: "
	 "Invalid number of primary key rows "
	 "selected [%u]",
	 static_cast<UInt_t>(pkr.size())));
    }
  }
  return kFALSE;
}

void TSnPQCompDaughtercardKey::GetFromPQDb(pqxx::dbtransaction& trans,
				           pqxx::result& res) {
  // get component ID
  if (FetchPrimKey(trans)) {
    // use it to get the general component values
    TSnPQComponentKey::GetFromPQDb(trans);
    
    // now get the rest
    TString sql(Form("SELECT %s, %s, %s, %s, %s FROM %s WHERE %s=",
		     kNumCol, kFpgaCol, kDigCol, kDateCnstrCol, kModelCol,
		     kDCTbl, kIdCol));
    AddVarToSQL(trans, sql, GetCompID());
    res = trans.exec(sql.Data(), sql.Data());
  } else {
    // not in DB yet
    throw std::runtime_error(Form("<TSnPQCompDaughtercardKey::GetFromPQDb>: "
				  "No component for this key: %s",
				  AsString().Data()));
  }
}

TObject* TSnPQCompDaughtercardKey::BuildObjFromResult(const pqxx::result& res) {
   TSnCompDaughtercard* dc=0;
   if (res.size()==1) {
      pqxx::result::const_iterator row = res.begin();
      // the component
      dc = new TSnCompDaughtercard;

      // first build the genearl component parameters
      BuildCompFromResult(*dc);

      // get the connected fpga component (possibly from cache)
      TSnPQCompFpgaKey fpgaKey;
      TSnPQCompFpgaKeyWhisperer::SetCompID(fpgaKey,
                                           row->at(kFpgaCol).as<Int_t>());
      TSnCompFpga* fpga = dynamic_cast<TSnCompFpga*>(
         TSnPQCompFpgaKeyWhisperer::GetFromDb(fpgaKey, TSnDatabase::GetDB()));
      if (fpga==0) {
         throw std::runtime_error(
            "<TSnPQCompDaughtercardKey::BuildObjFromResult>: "
            "Could not get FPGA from DB.");
      }
      
      // get the connected digitizer compnent (possibly from cache)
      TSnPQCompDigitizerKey digiKey;
      TSnPQCompDigitizerKeyWhisperer::SetCompID(digiKey,
                                                row->at(kDigCol).as<Int_t>());
      TSnCompDigitizer* digi = dynamic_cast<TSnCompDigitizer*>(
         TSnPQCompDigitizerKeyWhisperer::GetFromDb(digiKey,
                                                   TSnDatabase::GetDB()));
      if (digi==0) {
         throw std::runtime_error(
            "<TSnPQCompDaughtercardKey::BuildObjFromResult>: "
            "Could not get digitizer from DB.");
      }
      
      // build the rest
      dc->SetNum(row->at(kNumCol).as<Int_t>());
      dc->SetFpga(*fpga);
      dc->SetDigitizer(*digi);
      dc->SetModel(row->at(kModelCol).as<const Char_t*>());    
      pqxx::result::field dcnstr = row->at(kDateCnstrCol);
      if (dcnstr.is_null()==false) {
         TTimeStamp datc;
         GetTimestampFromStr(datc, dcnstr.as<const Char_t*>());
         dc->SetDateConstructed(datc);
      }

   } else {
      throw std::runtime_error(
         Form("<TSnPQCompDaughtercardKey::BuildObjFromResult>: "
	       "Unexpected number of rows in result [%u]",
	       static_cast<UInt_t>(res.size())));
   }
   return dc;
}


NSnDatabase::EWriteType TSnPQCompDaughtercardKey::WriteToPQDb(
					    pqxx::dbtransaction& trans,
					    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCompDaughtercard::Class())) {
   
      const TSnCompDaughtercard& dc = 
         dynamic_cast<const TSnCompDaughtercard&>(obj);
      // first write the general component
      NSnDatabase::EWriteType wc =
         TSnPQComponentKey::WriteToPQDb(trans, obj);

      // write the fpga via its key
      const TSnCompFpga& fpga = dc.GetFpga();
      TSnPQCompFpgaKey fpgaKey(fpga.GetNum());
      TSnPQCompFpgaKeyWhisperer::WriteToDb(fpgaKey, TSnDatabase::GetDB(), fpga);
      const Int_t fpgaId = TSnPQCompFpgaKeyWhisperer::GetCompID(fpgaKey);
      if (fpgaId<0) {
         throw std::runtime_error("<TSnPQCompDaughtercardKey::WriteToPQDb>: "
            "Could not get FPGA component_id from DB");
      }

      // write the digitizer via its key
      const TSnCompDigitizer& digi = dc.GetDigitizer();
      TSnPQCompDigitizerKey digiKey(digi.GetNum());
      TSnPQCompDigitizerKeyWhisperer::WriteToDb(digiKey, 
                                                TSnDatabase::GetDB(), digi);
      const Int_t digiId = TSnPQCompDigitizerKeyWhisperer::GetCompID(digiKey);
      if (digiId<0) {
         throw std::runtime_error("<TSnPQCompDaughtercardKey::WriteToPQDb>: "
            "Could not get digitizer component_id from DB");
      }

      if (wc==NSnDatabase::kUpdate) {
         // update the table
         TString sql(Form("UPDATE %s SET %s=",
		          kDCTbl, kNumCol));
         AddVarToSQL(trans, sql, dc.GetNum());
         sql += Form(", %s=",kFpgaCol);
         AddVarToSQL(trans, sql, fpgaId);
         sql += Form(", %s=",kDigCol);
         AddVarToSQL(trans, sql, digiId);
         sql += Form(", %s=",kModelCol);
         AddVarToSQL(trans, sql, dc.GetModel());
         if (dc.HasDateConstructed()) {
            sql += Form(", %s=",kDateCnstrCol);
            AddVarToSQL(trans, sql, 
                        GetSQLTimestampUTC(*(dc.GetDateConstructed())));
         }
         sql += Form(" WHERE %s=", kIdCol);
         AddVarToSQL(trans, sql, GetCompID());

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kUpdate;

      } else if (wc==NSnDatabase::kInsert) {
         // insert to the table

         TString sql(Form("INSERT INTO %s (%s, %s, %s, %s, %s",
		          kDCTbl, kIdCol, kNumCol, kFpgaCol, kDigCol, kModelCol));
         if (dc.HasDateConstructed()) {
            sql += Form(", %s",kDateCnstrCol);
         }
         sql += ") VALUES (DEFAULT, ";
         AddVarToSQL(trans, sql, dc.GetNum());
         sql += ", ";
         AddVarToSQL(trans, sql, fpgaId);
         sql += ", ";
         AddVarToSQL(trans, sql, digiId);
         sql += ", ";
         AddVarToSQL(trans, sql, dc.GetModel());
         if (dc.HasDateConstructed()) {
            sql += ", ";
            AddVarToSQL(trans, sql, 
                        GetSQLTimestampUTC(*(dc.GetDateConstructed())));
         }
         sql += ")";

         trans.exec(sql.Data(), sql.Data());
         trans.commit();
         wt = NSnDatabase::kInsert;

      } else {
         throw std::runtime_error(Form(
            "<TSnPQCompDaughtercardKey::WriteToPQDb>: "
            "Unknown write type [%d] from component key.",
            static_cast<Int_t>(wc)));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQCompDaughtercardKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }
  

   return wt;
}













