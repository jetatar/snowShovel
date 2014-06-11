#ifndef SNS_NSnConstants
#define SNS_NSnConstants

/*******************************************************************************
*
* TODO: These should be moved to some detector description classes.
* (And probably obtained from the database.)
*
*
* These MUST match the values on the station(s).
*
* Changing these values is a BIG DEAL! The streamers of many classes
* must be correspondingly increased. The i/o versions of the online
* interfaces must also change. Work will have to be done to ensure
* backward compatibility -- i.e. the old values will have to be
* available and also used when reading old versions.
*
*******************************************************************************/

#include <Rtypes.h>

namespace NSnConstants {

   static const UChar_t    kNchans     = 4;
   static const UChar_t    kNdacs      = 4;
   static const UChar_t    kNsamps     = 128;
   static const UShort_t   kTotSamps   = NSnConstants::kNchans 
                                       * NSnConstants::kNsamps;
   static const UShort_t   kTotDacs    = NSnConstants::kNchans
                                       * NSnConstants::kNdacs;
   static const UChar_t    kMaxPlasV1  = 5;
   static const UChar_t    kMaxPlas    = kMaxPlasV1;

   static const UShort_t   kNadcs      = 4096;

   static const Float_t    kSampRate   = 1.92; // GHz

   template<typename Num_t>
   inline
   Int_t WrapSampleNum(Num_t& s) {
      // set 's' to the sample number modulo kNsamps
      // return value is the offset applied (can be used to shift other
      // variables by the same amount)
      Int_t d(0);
      while (s<0) {
         s += kNsamps;
         d += kNsamps;
      }
      while (s>=kNsamps) {
         s -= kNsamps;
         d -= kNsamps;
      }
      return d;
   }
   
};

#endif // SNS_NSnConstants
