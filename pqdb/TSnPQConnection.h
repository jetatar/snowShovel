#ifndef SNS_TSnPQConnection
#define SNS_TSnPQConnection

#include <TString.h>

#include "TSnConnection.h"
class TSnPQReadTransactor;
class TSnPQWriteTransactor;
class TSnDatabase;

namespace pqxx {
   class connect_direct;

   template <class T>
   class basic_connection;
};

#include <stdexcept>

//////////////////////////////////////////////////////////////////////////
//
// TSnPQConnection
//
// Interface to a PostgreSQL database using libpqxx
//
//////////////////////////////////////////////////////////////////////////

class TSnPQConnection : public TSnConnection {
 public:
   enum EPQConnType {
      kRead,
      kWrite
   };
   
   typedef pqxx::basic_connection<pqxx::connect_direct> PQxxConn_t;
   
 private:
   PQxxConn_t* fConn;
   
   static const Char_t* kDefConnNm;
   static const Char_t* kPQDBprops;
   static const Char_t* kReadUser;
   static const Char_t* kReadPwd;
   static const Char_t* kWriteUser;
   static const Char_t* kWritePwd;
   
   static const Char_t* GetConnStr(const TSnPQConnection::EPQConnType type);
   
 public:
   TSnPQConnection() : fConn(0) {}
   TSnPQConnection(const TSnPQConnection::EPQConnType type,
                   const Char_t* name=TSnPQConnection::kDefConnNm,
                   const Char_t* title=0);
   virtual ~TSnPQConnection();
   
         TSnPQConnection::PQxxConn_t* GetPQConn()       { return fConn; }
   const TSnPQConnection::PQxxConn_t* GetPQConn() const { return fConn; }
   
   Bool_t   IsOpen() const;
   void     Disconnect();
   
   Bool_t   Open() { Activate(); return kTRUE; }
   
   void     Activate();
   void     Deactivate();
   
   // would be nicer to do Perform(const TSnPQTransactor&), but
   // this is not possible due to the way perform is templated
   void     Perform(const TSnPQReadTransactor& tr);
   void     Perform(const TSnPQWriteTransactor& tr);
   
   static TSnPQConnection* GetOrOpenPQConn(
                                         const TSnPQConnection::EPQConnType ct,
                                         TSnDatabase& db);
   
   static const Char_t* GetDefConnNm() { return kDefConnNm; }
   static const Char_t* GetPQConnTypeStr(const TSnPQConnection::EPQConnType ct);
   
   ClassDef(TSnPQConnection, 1);
};

inline
const Char_t* TSnPQConnection::GetPQConnTypeStr(
                                       const TSnPQConnection::EPQConnType ct) {
   switch (ct) {
      case kRead:
         return "Read";
      case kWrite:
         return "Write";
      default:
         throw std::invalid_argument(
            Form("<TSnPQConnection::GetPQConnTypeStr>: "
            "Unknown connection type [%d]",
            static_cast<Int_t>(ct)));
   
   };
}

inline
const Char_t* TSnPQConnection::GetConnStr(
                                    const TSnPQConnection::EPQConnType type) {
   switch (type) {
      case kRead:
         return Form("%s user=%s password=%s",
            kPQDBprops, kReadUser, kReadPwd);
      case kWrite:
         return Form("%s user=%s password=%s",
            kPQDBprops, kWriteUser, kWritePwd);
      default:
         throw std::invalid_argument(
            Form("<TSnPQConnection::GetConnStr>: Unknown connection type [%d]",
            static_cast<Int_t>(type)));
   };
   return 0;
}

#endif // SNS_TSnPQConnection
