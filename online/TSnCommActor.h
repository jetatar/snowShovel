#ifndef SNS_TSnCommActor
#define SNS_TSnCommActor

#include <TNamed.h>
#include <TList.h>
#include <TMap.h>
class TTimeStamp;
class TClass;

class TSnStatusUpdate;
class TSnPowerReading;
class TSnCommAction;
class TSnCommObject;
class TSnCommConfPack;
class TSnCommIncoming;

class TSnCommActor : public TNamed {
 private:
   TList            fActs;     // the (ordered) list of actions
   TList            fComQue;   // FIFO queue of comm objects to send
   TMap             fLastSent; // copies of the last sent comm objects, indexed by class name
   Bool_t           fUseDB;    // useful for actions to determine whether they
                               // should use the DB or not

   mutable       TSnCommAction*   fLastAct;   //! the last action that had a resp
   
   static
   TClass* GetClassSafe(const Char_t* cn);
   TSnCommAction* NewAction(const Char_t* acn);
   
   TObjLink* FindLinkSlow(const Char_t* acn);
   TObjLink* FindLinkSafe(const Char_t* acn);

 protected:
   void       SetLastAction(TSnCommAction* a) { fLastAct = a; }
   TIterator* NewActionIter() const { return fActs.MakeIterator(); }
   
 public:
   TSnCommActor() : fUseDB(kFALSE), fLastAct(0) {
      fActs.SetOwner(kTRUE);
      fComQue.SetOwner(kTRUE);
      fLastSent.SetOwnerKeyValue(kTRUE,kTRUE);
   }
   TSnCommActor(const Char_t* name, const Char_t* mac) : 
      TNamed(name,mac),
      fUseDB(kFALSE), fLastAct(0) {
      fActs.SetOwner(kTRUE);
      fComQue.SetOwner(kTRUE);
      fLastSent.SetOwnerKeyValue(kTRUE,kTRUE);
   }
   virtual ~TSnCommActor();

   // for use by TSnCommActions to make their decision
   Bool_t                 IsUsingDB() const { return fUseDB; }
   
   // to build an ordered list of possible actions
   void AddFirst(const Char_t* actionClassName);
   void AddLast(const Char_t* actionClassName);
   void AddBefore(const Char_t* actionClassNameBefore, 
                  const Char_t* actionClassName);
   void AddAfter(const Char_t* actionClassNameAfter, 
                 const Char_t* actionClassName);
   void Remove(const Char_t* actionClassName);
   
   // intentionally not getter for the action list (even const)
   // because Root's MakeIterator is not const correct: the function
   // is const, but yields non-const pointers to items in the list
   //
   // instead, loop through the list via:
   const TSnCommAction* FirstAction() const;
   const TSnCommAction* LastAction() const;
   const TSnCommAction* NextAction(const TSnCommAction* a) const;
   const TSnCommAction* PreviousAction(const TSnCommAction* a) const;
   const TSnCommAction* FindAction(const Char_t* actionClassName) const;
   
   // manage the FIFO communications queue
   Bool_t               HasComms() const { return fComQue.GetSize()>0; }
   void                 AddComm(const TSnCommObject& co) { AppendComm(co); }
   void                 AppendComm(const TSnCommObject& co);
   void                 PrependComm(const TSnCommObject& co);
   Bool_t               RemoveNextComm();
   Bool_t               ClearAllComms();
   const TSnCommObject* PeekNextComm() const;
         TSnCommObject* PullNextComm();
   const TList&         PeekCommQueue() const { return fComQue; }
   // intentionally no non-const PeekCommQueue
   
   // called from outside
         TSnCommAction* GetAction(const Char_t* actionClassName);
   const TSnCommAction* GetAction(const Char_t* actionClassName) const;

   TSnCommObject*       GetResponseFor(const TSnCommIncoming& in);

   const TSnCommAction* PullLastResponder() const 
      { return fLastAct; fLastAct=0; }

   const TMap&            GetLastSentMap() const { return fLastSent; }
   const TSnCommObject*   GetLastSent(const Char_t* classname) const;
   const TSnCommConfPack* GetLastConf() const;
   // intentionally no non-const getters for last sent objects
   
   void                 ReplaceLastSent(const TSnCommObject& o);

   void                 SetUseDB(const Bool_t b) { fUseDB = b; }

   virtual void         Print(Option_t* option = "") const;
   
   ClassDef(TSnCommActor, 4); // manages a collection of TSnCommActions
};

#endif // SNS_TSnCommActor
