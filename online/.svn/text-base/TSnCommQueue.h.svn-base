#ifndef SNS_TSnCommQueue
#define SNS_TSnCommQueue

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class TMap;
class TList;

class TSnCommObject;

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommQueue
//
// A map from a station id string (i.e. an MBED mac address) to a
// collection of TSnCommObjects.
//
// The name "queue" refers to its intended application. This collection behaves
// like a map (not a FIFO queue), but the station id keys link to FIFO queues,
// implemented as TLists. In this way, a collection of commands can be
// queued for each station.
// 
// The FIFO queue's are intentionally not directly changeable.
// 
////////////////////////////////////////////////////////////////////////////////

class TSnCommQueue : public TObject {

 private:
   TMap*          fQ;     // the queue, indexed by station ID (i.e. mac address)

   TList*         GetOrMakeStnQueue(const Char_t* station);
   
 public:
   TSnCommQueue() : fQ(0) {}
   TSnCommQueue(const Bool_t); // this one allocates memory
   virtual ~TSnCommQueue();
   
   void           AddCommFor(const Char_t* station, 
                             const TSnCommObject& comdat) {
      // see AppendCommFor
      AppendCommFor(station, comdat);
   }
   void           AppendCommFor(const Char_t* station, 
                                const TSnCommObject& comdat);
   void           PrependCommFor(const Char_t* station, 
                                 const TSnCommObject& comdat);
   Bool_t         HasAnyCommsFor(const Char_t* station) const;
   const TSnCommObject* 
                  PeekNextCommFor(const Char_t* station) const;
   Bool_t         RemoveNextCommFor(const Char_t* station);
   TSnCommObject* PullNextCommFor(const Char_t* station);
   Bool_t         ClearAllCommsFor(const Char_t* station);

   const TList*   PeekCommsFor(const Char_t* station) const;
   
   virtual void   Print(Option_t *option="") const;
   virtual void   ls(Option_t *option="") const;
   
   ClassDef(TSnCommQueue, 2); // a queue for storing info to send to stations
};

#endif // SNS_TSnCommQueue
