
void makeSBWstripCharts(const Char_t* sbgfn,
                        const Char_t* outtag,
                        const Float_t zoomDays=7.0,
                        const Bool_t  drawMinMax=kFALSE,
                        const Bool_t  finFromData=kFALSE) {
   // dummy script to be able to launch makeSBWcharts from the command line
   
   gROOT->LoadMacro("$SNS/scripts/offline/makeStripCharts.C+");
   makeSBWcharts(sbgfn, outtag, zoomDays, drawMinMax, finFromData);
}
