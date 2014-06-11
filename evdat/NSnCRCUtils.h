#ifndef SNS_NSnCRCUtils
#define SNS_NSnCRCUtils

#include <Rtypes.h>

/*******************************************************************\
*   CRC code "borrowed" from:                                       *
*   http://mbed.org/users/jpelletier/programs/CRC/lje513            *
*   Library         : lib_crc                                       *
*   File            : lib_crc.c                                     *
*   Author          : Lammert Bies  1999-2008                       *
*   E-mail          : info@lammertbies.nl                           *
*   Language        : ANSI C                                        *
\*******************************************************************/

namespace NSnCRCUtils {
   
   extern
   const UInt_t crc_tabxfer_normal[256];

   //
   // CRC calculator routines
   //
   inline
   UInt_t update_crc32_normal(const UInt_t* table, 
                              UInt_t crc, const UChar_t c ) {
      UInt_t long_c;
      long_c = 0x000000ffL & (UInt_t) c;
      return (crc << 8) ^ table[((crc >> 24) ^ long_c) & 0xff];
   }
   inline
   UInt_t update_crc32_xfer( UInt_t crc, const UChar_t c ) {
      return update_crc32_normal(crc_tabxfer_normal,crc,c);
   }

   //
   // calculate CRC for data set
   //
   template<typename T>
   inline
   UInt_t GetUpdatedCRC32for(UInt_t crc, const T* data, const UInt_t len) {
      // get the updated 32bit CRC for an array of data
      // len = number of array elements; not number of bytes
      
      union {
         const T* x;
         const UChar_t* c;
      } d;
      d.x = data;
      const UInt_t bytes = len * sizeof(T);
      for (register UInt_t i=0; i<bytes; ++i, ++d.c) {
         crc = update_crc32_xfer(crc, *(d.c));
      }
      return crc;
   }
   template<typename T>
   inline
   UInt_t GetCRC32for(const T* data, const UInt_t len) {
      // calculate the 32bit CRC for an array of data
      // len = number of array elements; not number of bytes
      UInt_t crc(0);
      return GetUpdatedCRC32for(crc, data, len);
   }

   extern
   UInt_t GetCRC32for(FILE* inf);

   extern
   UInt_t GetCRC32for(const Char_t* infn);

};

#endif // SNS_NSnCRCUtils
