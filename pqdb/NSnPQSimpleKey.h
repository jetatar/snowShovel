#ifndef SNS_NSnPQSimpleKey
#define SNS_NSnPQSimpleKey

//////////////////////////////////////////////////////////////////////////
// namespace NSnPQSimpleKey
//
// Define some useful constants that can be included in other files.
// 
// Normally, this would be part of the TSnPQSimpleKey, but TSnPQSimpleKey.h
// includes pqxx headers that rootcint can't parse.
//
//////////////////////////////////////////////////////////////////////////


namespace NSnPQSimpleKey {
   
   enum EValType {
      kUndef,
      kString,
      kBool,
      kShort,
      kUShort,
      kInt,
      kUInt,
      kFloat,
      kDouble,
      kLong64,
      kULong64
   };

};

#endif // SNS_NSnPQSimpleKey
