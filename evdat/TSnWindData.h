#ifndef SNS_TSnWindData
#define SNS_TSnWindData

#include <TObject.h>
#include <TTimeStamp.h>


class TSnWindData : public TObject 
{
    private:
        Double_t fWind;
        TTimeStamp fTime;
        Bool_t bSB;
        
    public:
        TSnWindData( ) : fWind( 0 ){ }

        TSnWindData( const Double_t w, const TTimeStamp& t ) : 
            fWind(w), fTime(t){ }
    
        virtual ~TSnWindData( ){ }

        // constant function specifies the function as 'read only'
        Double_t GetWind( ) const           { return fWind; }
        const TTimeStamp& GetTime( ) const  { return fTime; }
        TTimeStamp& GetTime( )              { return fTime; }
        UInt_t GetUnixTime( ) const         { return fTime.GetSec( ); }

        void SetWind( const Double_t w )    { fWind = w; }
        void SetTime( const TTimeStamp& t ) { fTime = t; }
        void SetUnixTime( const UInt_t t )  { fTime.Set( t, kTRUE, 0, kFALSE ); }
        // Was the wind entry from Scott Base (bSB = 1) or Station 2 (bSB = 0)?
        void SetSB( const Bool_t sb )       { bSB = sb; } 

    ClassDef( TSnWindData, 1 );
};


#endif // SNS_TSnWindData
