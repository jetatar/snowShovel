#ifndef SNS_TSnCommAlertPack
#define SNS_TSnCommAlertPack

#include <TString.h>

#include "TSnCommObject.h"

class TSnCommAlertPack : public TSnCommObject {
 public:
   enum ESeverity {
      // worse goes later
      kLogInfo,
      kLogWarn,
      kEmailAlert
   };

 private:
   ESeverity    fLevel;      // the severity level of the alert (def: kEmailAlert)
   TSnCommObject* fComm;     // the comm object to be sent to the station
   TString      fSource;     // the action from which this alert originated
   TString      fSubject;    // the subject / brief summary of the alert
   TString      fMessage;    // the complete description of what happened
   
 protected:
   TSnCommAlertPack(const TSnCommAlertPack& a) :
      TSnCommObject(a),
      fLevel(a.fLevel),
      fComm( (a.fComm!=0)?(a.fComm->NewClone()):0 ),
      fSource(a.fSource), fSubject(a.fSubject), fMessage(a.fMessage) {}
   TSnCommAlertPack(const TSnCommObject& comm,
                    const Char_t* source, const Char_t* subj,
                    const Char_t* msg,
                    const TSnCommAlertPack::ESeverity level=
                       TSnCommAlertPack::kEmailAlert) :
      fLevel(level), fComm(comm.NewClone()),
      fSource(source), fSubject(subj), fMessage(msg) {
   }
   TSnCommAlertPack(TSnCommObject* comm,
                    const Char_t* source, const Char_t* subj,
                    const Char_t* msg,
                    const TSnCommAlertPack::ESeverity level=
                       TSnCommAlertPack::kEmailAlert) :
      fLevel(level), fComm(comm),
      fSource(source), fSubject(subj), fMessage(msg) {
   }
   
 public:
   // this must be public for Root's streaming to work
   TSnCommAlertPack() : fLevel(kEmailAlert), fComm(0) {}

   // pseudo constructors, to avoid confusion about who owns the
   // associated communication object
   static
   TSnCommAlertPack* NewAlertPackCopyComm(const TSnCommObject& comm,
                                          const Char_t* source, 
                                          const Char_t* subj,
                                          const Char_t* msg,
                                          const TSnCommAlertPack::ESeverity 
                                          level=TSnCommAlertPack::kEmailAlert) {
      // returns a new TSnCommAlertPack that will store a copy of 'comm'
      return new TSnCommAlertPack(comm, source, subj, msg, level);
   }
   static
   TSnCommAlertPack* NewAlertPackPassComm(TSnCommObject* comm,
                                          const Char_t* source, 
                                          const Char_t* subj,
                                          const Char_t* msg,
                                          const TSnCommAlertPack::ESeverity
                                          level=TSnCommAlertPack::kEmailAlert) {
      // returns a new TSnCommAlertPack that will take ownership of 'comm'
      return new TSnCommAlertPack(comm, source, subj, msg, level);
   }
   virtual ~TSnCommAlertPack() { delete fComm; }

   static
   void          Swap(TSnCommAlertPack& a, TSnCommAlertPack& b);
   
   TSnCommAlertPack& operator=(TSnCommAlertPack o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }
   
   TSnCommAlertPack::ESeverity GetSeverity() const { return fLevel; }
   Bool_t        ShouldSendEmail() const { return (fLevel >= kEmailAlert); }
   TString       GetSource() const { return fSource; }
   TString       GetSubject() const { return fSubject; }
   TString       GetMessage() const { return fMessage; }
   const Char_t* GetSourceStr() const { return fSource.Data(); }
   const Char_t* GetSubjectStr() const { return fSubject.Data(); }
   const Char_t* GetMessageStr() const { return fMessage.Data(); }
   const TSnCommObject* GetComm() const { return fComm; }
   
   void          SetSeverity(const TSnCommAlertPack::ESeverity l) { fLevel=l; }
   void          SetSource(const Char_t* s) { fSource = s; }
   void          SetSubject(const Char_t* s) { fSubject = s; }
   void          SetMessage(const Char_t* m) { fMessage = m; }

   void          CopyComm(const TSnCommObject& c) {
      // store a copy of the communication with this alert
      delete fComm; fComm = c.NewClone();
   }
   void          PassComm(TSnCommObject* c) {
      // transfer ownership of the communication to this alert
      delete fComm; fComm = c;
   }

   virtual Bool_t  IsOkForStatusVersion(const Int_t sv) const;

   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const;
   virtual TSnIOBuffer GetIOByteArray() const;

   virtual TSnCommObject* NewClone() const { return new TSnCommAlertPack(*this); }
   
   virtual Bool_t  IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const { return fSubject.Hash(); }
   
   virtual TString GetLabel() const { return fSubject; }
   
   virtual void   Print(Option_t *option="") const;

   
   ClassDef(TSnCommAlertPack, 3); // an alert to be handled by the communication system
};

#endif // SNS_TSnCommAlertPack
