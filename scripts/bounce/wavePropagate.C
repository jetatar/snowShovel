#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TMath.h>
#include <TVector2.h>
#include <TString.h>
#include <TError.h>
#include <TGraph.h>
#endif

enum EFirnModel {
   kLinear,
   kShelfMC
};

static const EFirnModel gFirnModel = kShelfMC;

static const Double_t kEpsilon   = 1e-10;

static const Double_t kC_m_ns   = TMath::C() / 1.0e9;

static const Double_t kNgTopFern = 1.33;
static const Double_t kNgIce     = 1.78;
static const Double_t kFernDepth = -50;    // m
//static const Double_t kFernDepth = -60;    // m
static const Double_t kIceDepth  = -560;   // m

TGraph* grho, * gt, * gp;

Double_t getNg(const Double_t depth) {
   // depth should be negative -- z=0 at antenna level
   if ((depth>0) || (depth<kIceDepth)) {
      Fatal("getNg","Depth %g out of bounds.",depth);
      return 1;
   } else if (depth<kFernDepth) {
      return kNgIce;
   } else {
      if (gFirnModel==kLinear) {
         static const Double_t m = (kNgIce - kNgTopFern) / kFernDepth;
         return ((depth*m) + kNgTopFern);
      } else if (gFirnModel==kShelfMC) {
         static const Double_t m = 1.0 - (0.638*TMath::Exp(depth/34.7));
         return (1.0 + (0.86*m));
      }
   }
}

Bool_t prop2D(TVector2& pos,
              TVector2& dir,
              Double_t& time,
              const Double_t step) {
   // pos: (rho, z). rho=dist from station, z=height with station at 0
   //      units are meters
   // dir: unit vector in (rho, z) space giving (momentum) direction
   // time: amount of time since T0 in nanoseconds
   // step: distance to propagate in meters
   // 
   // check if index refraction changed:
   //   (a) propagate as if n doesn't change
   //   (b) look up n for new depth
   //   (c) if n changed, assume an interface halfway between z_a and z_b
   // 
   // NOTE: dir must be a unit vector (but this is not checked)
   //
   // return true iff new position has z<0

   static const TVector2 zhat(0,1);
   static const TVector2 phat(1,0);
   
   const Double_t  ngp = getNg(pos.Y());
   const TVector2 xstp( (dir*step) + pos );
   if ( xstp.Y() < kIceDepth ) {
       // reflect off bottom
      dir = dir - 2.0*(dir*zhat)*zhat;   // reflect the direction
      pos.Set(xstp.X(),
              2.0*kIceDepth - xstp.Y()); // new position easy to calc
      time += step * kC_m_ns / ngp;
   } else if (xstp.Y()<0) {
      const Double_t  ngx = getNg(xstp.Y());
      if ( TMath::Abs(ngx-ngp)<kEpsilon ) {
         pos = xstp; // just move up
         // dir unchanged
         time += step * kC_m_ns / ngp;
      } else {
         const Double_t hstep = step/2.0;
         // assume interface in center of step
         const TVector2 mstp( (dir*hstep) + pos );
         // use Snell's law for new angle
         const Double_t sin1 = dir*phat;
         dir = dir.Rotate( (ngp*sin1/ngx) - sin1 );
         // move up the other half of the step
         pos = (dir*hstep) + mstp;
         time += hstep * kC_m_ns / ngp;
         time += hstep * kC_m_ns / ngx;
      }
   } else {
      // hit the detector from below
      // assume final interface at start of this step
      const Double_t ngx = getNg(0);
      const Double_t sin1 = dir*phat;
      dir = dir.Rotate( (ngp*sin1/ngx) - sin1 );
      // just propagate to z=0
      const Double_t nstp = -pos.Y() / (dir*zhat);
      pos  += (dir*nstp);
      time += nstp * kC_m_ns / ngx;
   }

   return (pos.Y()<0);
}


Double_t trajectory(const Double_t theta,
                    const Bool_t draw,
                    const Double_t step=0.5) {
   TVector2 pos(0,0);
   TVector2 dir;
   dir.SetMagPhi(1.0, (360+90)*TMath::DegToRad()-theta);

   Double_t t=0;
   if (draw) {
      gp = new TGraph;
      gp->SetTitle("photon trajectory;dist from station (m);"
                   "height from station (m)");
   }
   Int_t p=0;
   while (prop2D(pos,dir,t,step)) {
      if (draw) {
         gp->SetPoint(p++, pos.X(), pos.Y());
      }
   }
   if (draw) {
      gp->Draw("apl");

      Printf("done at (%g, %g) in %g ns", pos.X(), pos.Y(), t);
      Printf("final direction %g deg",dir.Phi()*TMath::RadToDeg());
   }
   return pos.X();
}

void test() {
   
   TVector2 spos(157.0,  0.0); // 515 ft
   TVector2 sdir(  0.0, -1.0);
   Double_t t(0);
   Double_t step(1); // m
   
   // vary starting direction by one degree and see how far from
   // the station we wind up
   grho = new TGraph(60);
   grho->SetTitle("final dist from station vs starting angle (from vert down)"
                  ";starting angle from vertical down (deg);"
                  "final dist to station (m)");
   gt   = new TGraph(60);
   gt->SetTitle("photon travel time vs starting angle (from vert down)"
                ";starting angle from vertical down (deg);"
                "photon travel time (ns)");
   TVector2 pos, dir;
   Double_t ang(-9*TMath::DegToRad());
   for (Int_t i=0; i<60; ++i) {
      // initial conditions
      pos  = spos;
      dir  = sdir.Rotate(ang);
      t    = 0;
      
      // propagate until we get back to z=0
      while (prop2D(pos,dir,t,step));
      
      grho->SetPoint(i, ang*TMath::RadToDeg(), pos.X());
      gt->SetPoint(i, ang, t);
      
      // angle for next time
      ang -= 0.1*TMath::DegToRad();
   }
   
   // starting angle to get back to station from 515ft
   pos.Set(157.0, 0.0);
   dir.Set(0.0, -1.0);
   dir = dir.Rotate(-10.75 * TMath::DegToRad());
   t=0;
   gp = new TGraph;
   gp->SetTitle("photon trajectory;dist from station (m);"
                "height from station (m)");
   Int_t p=0;
   while (prop2D(pos,dir,t,0.05)) {
      gp->SetPoint(p++, pos.X(), pos.Y());
   }
   
   Printf("done at (%g, %g) in %g ns", pos.X(), pos.Y(), t);
   Printf("final direction %g deg",dir.Phi()*TMath::RadToDeg());
}

void chkang(const Double_t ang) {
   TVector2 pos(157.0, 0.0);
   TVector2 dir(0.0, -1.0);
   dir = dir.Rotate(-10.75 * TMath::DegToRad());
   Double_t t=0;
   gp = new TGraph;
   gp->SetTitle("photon trajectory;dist from station (m);"
                "height from station (m)");
   Int_t p=0;
   while (prop2D(pos,dir,t,0.05)) {
      gp->SetPoint(p++, pos.X(), pos.Y());
   }
   
   Printf("done at (%g, %g) in %g ns", pos.X(), pos.Y(), t);
   Printf("final direction %g deg",dir.Phi()*TMath::RadToDeg());
   
}
