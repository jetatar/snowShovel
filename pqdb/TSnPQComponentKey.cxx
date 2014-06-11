#include "TSnPQComponentKey.h"

#include <TTimeStamp.h>
#include <TMap.h>
#include <TObjString.h>
#include <TClass.h>

#include "TSnComponent.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <sstream>
#include <stdexcept>

const Char_t* TSnPQComponentKey::kCompTypeCol = "component_type";
const Char_t* TSnPQComponentKey::kDateInstCol = "date_installed";
const Char_t* TSnPQComponentKey::kDateRemvCol = "date_removed";
const Char_t* TSnPQComponentKey::kIdCol       = "id";
const Char_t* TSnPQComponentKey::kCompTbl     = "component";

TSnPQComponentKey::~TSnPQComponentKey() {
   delete fRes;
}

#if PQXX_VERSION_MAJOR<4
void TSnPQComponentKey::AddPKToPrepSQL(
                              pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQComponentKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(GetCompID());
}

Bool_t TSnPQComponentKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQComponentKey* ck = dynamic_cast<const TSnPQComponentKey*>(obj);
      if ( ck->GetCompID() == GetCompID() ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQComponentKey::AsString() const {
   TString s(GetCompType());
   s += " component, id=";
   s += GetCompID();
   return s;
}

void TSnPQComponentKey::GetFromPQDb(pqxx::dbtransaction& trans) {
   TString sql(Form("SELECT %s, %s, %s FROM %s WHERE %s=",
               kCompTypeCol, kDateInstCol, kDateRemvCol,
	       kCompTbl, kIdCol));
   AddVarToSQL(trans, sql, GetCompID());
   
   fRes = new pqxx::result( trans.exec(sql.Data(), sql.Data()) );
}

void TSnPQComponentKey::BuildCompFromResult(TSnComponent& comp) const {
   // simply returns a map from the column name to the value as a string
   //
   // intended to be called by a derived key
   
   if (fRes!=0) {
      if (fRes->size()==1) {
	 // expect single row
	 pqxx::result::const_iterator row = fRes->begin();
	 
	 const TString ctype(row->at(kCompTypeCol).as<const Char_t*>());
	 if (ctype.CompareTo(GetCompType())==0) {
	   pqxx::result::field inst = row->at(kDateInstCol);
	   if (inst.is_null()==false) {
             TTimeStamp datc;
             GetTimestampFromStr(datc, inst.as<const Char_t*>());
	     comp.SetInstallDate(datc);
	   }
	   pqxx::result::field remv = row->at(kDateRemvCol);
	   if (remv.is_null()==false) {
             TTimeStamp datc;
             GetTimestampFromStr(datc, remv.as<const Char_t*>());
	     comp.SetRemovedDate(datc);
	   }

	 } else {
	   throw std::runtime_error(
	      Form("<TSnPQComponentKey::BuildObjFromResult>: "
	      "Key is for component type [%s], but comp ID [%d]"
	      " is of type [%s].",
	      GetCompType(), GetCompID(), ctype.Data()));
	 }
	 
      } else {
         throw std::runtime_error(
            Form("<TSnPQComponentKey::BuildObjFromResult>: "
            "Unexpected number of rows [%u] in result.",
	    static_cast<UInt_t>(fRes->size())));
      }
   } else {
      throw std::runtime_error("<TSnPQComponentKey::BuildObjFromResult>: "
         "Can't build object from null result.");
   }

}

NSnDatabase::EWriteType TSnPQComponentKey::WriteToPQDb(
                                    pqxx::dbtransaction& trans,
                                    const TObject& obj) {
   // if already in the table, update the dates.
   // can only update the dates; component ID and type are fixed.
   // if neither date is defined in the key, the update will
   // silently not occur (since a decendant class might want to
   // udpate something in its component-specific table)
   //
   // if the component is not already in the table, insert it
   
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnComponent::Class())) {
   
      const TSnComponent& comp = dynamic_cast<const TSnComponent&>(obj);

      if (comp.IsValidForDb()) {

         const Bool_t hasInst = comp.HasInstallDate();
         const Bool_t hasRemv = comp.HasRemovedDate();
         if (FetchPrimKey(trans)) {

            // update

            if ( hasInst || hasRemv ) {
               TString sql(Form("UPDATE %s SET ", kCompTbl));
               if (hasInst) {
                  sql += kDateInstCol;
                  sql += "=";
                  AddVarToSQL(trans, sql, 
                              GetSQLTimestampUTC(*(comp.GetInstallDate())));
               }
               if(hasRemv) {
                  if (hasInst) {
                     sql += ", ";
                  }
                  sql += kDateRemvCol;
                  sql += "=";
                  AddVarToSQL(trans, sql, 
                              GetSQLTimestampUTC(*(comp.GetRemovedDate())));
               }
               sql += " WHERE ";
               sql += kIdCol;
               sql += "=";
               AddVarToSQL(trans, sql, GetCompID());

               trans.exec(sql.Data(), sql.Data());
               trans.commit();
	       wt = NSnDatabase::kUpdate;
            }

         } else {

            // insert

            TString sql(Form("INSERT INTO %s (%s, ",
                             kCompTbl, kIdCol));
            if (hasInst) {
               sql += kDateInstCol;
               sql += ", ";
            }
            if (hasRemv) {
               sql += kDateRemvCol;
               sql += ", ";
            }
            sql += kCompTypeCol;
            sql += ") VALUES (DEFAULT, ";
            if (hasInst) {
	       AddVarToSQL(trans, sql, 
                           GetSQLTimestampUTC(*(comp.GetInstallDate())));
               sql += ", ";
            }
            if (hasRemv) {
               AddVarToSQL(trans, sql, 
                           GetSQLTimestampUTC(*(comp.GetRemovedDate())));
               sql += ", ";
            }
            AddVarToSQL(trans, sql, comp.GetCompType());
            sql += ")";

            trans.exec(sql.Data(), sql.Data());
            trans.commit();
            wt = NSnDatabase::kInsert;
         }
      } else {
         throw std::runtime_error(Form("<TSnPQComponentKey::WriteToPQDb>: "
            "%s component [%s] at [%p] is not valid for writing.",
            comp.GetCompType(), comp.GetName(), &obj));
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnPQComponentKey::WriteToPQDb>: "
         "Cannot write object of type [%s]",
         obj.IsA()->GetName()));
   }
   
   return wt;
}

 
