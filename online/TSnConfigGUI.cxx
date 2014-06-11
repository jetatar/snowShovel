// Mainframe macro generated from application: /Applications/root/bin/root.exe
// By ROOT version 5.32/00 on 2012-04-30 14:30:57

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGFontDialog
#include "TGFontDialog.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"

void TSnConfigGUI()
{

   // main frame
   TGMainFrame *fMainFrame2119 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame2119->SetName("fMainFrame2119");
   fMainFrame2119->SetLayoutBroken(kTRUE);

   // composite frame
   TGCompositeFrame *fMainFrame5221 = new TGCompositeFrame(fMainFrame2119,690,628,kVerticalFrame);
   fMainFrame5221->SetName("fMainFrame5221");
   fMainFrame5221->SetLayoutBroken(kTRUE);

   // "Amplifiers" group frame
	// Amp #1 Button
   TGGroupFrame *fGroupFrame825 = new TGGroupFrame(fMainFrame5221,"Amplifiers");
   fGroupFrame825->SetTitlePos(TGGroupFrame::kCenter);
   fGroupFrame825->SetLayoutBroken(kTRUE);
   TGCheckButton *fCheckButton826 = new TGCheckButton(fGroupFrame825,"Amplifier #1");
   fCheckButton826->SetState(kButtonDown);
   fCheckButton826->SetToolTipText("Turn On/Off Amp and Channel");
   fCheckButton826->SetTextJustify(36);
   fCheckButton826->SetMargins(0,0,0,0);
   fCheckButton826->SetWrapLength(-1);

   fGroupFrame825->AddFrame(fCheckButton826, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButton826->MoveResize(3,32,104,19);

	// Amp #2 Button
   TGCheckButton *fCheckButton829 = new TGCheckButton(fGroupFrame825,"Amplifier #2");
   fCheckButton829->SetState(kButtonDown);
   fCheckButton829->SetToolTipText("Turn On/Off Amp and Channel");
   fCheckButton829->SetTextJustify(36);
   fCheckButton829->SetMargins(0,0,0,0);
   fCheckButton829->SetWrapLength(-1);
   fGroupFrame825->AddFrame(fCheckButton829, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButton829->MoveResize(3,72,104,19);

	// Amp #3 Button
   TGCheckButton *fCheckButton832 = new TGCheckButton(fGroupFrame825,"Amplifier #3");
   fCheckButton832->SetState(kButtonDown);
   fCheckButton832->SetToolTipText("Turn On/Off Amp and Channel");
   fCheckButton832->SetTextJustify(36);
   fCheckButton832->SetMargins(0,0,0,0);
   fCheckButton832->SetWrapLength(-1);
   fGroupFrame825->AddFrame(fCheckButton832, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButton832->MoveResize(3,112,104,19);

	// Amp #4 Button
   TGCheckButton *fCheckButton835 = new TGCheckButton(fGroupFrame825,"Amplifier #4");
   fCheckButton835->SetState(kButtonDown);
   fCheckButton835->SetToolTipText("Turn On/Off Amp and Channel");
   fCheckButton835->SetTextJustify(36);
   fCheckButton835->SetMargins(0,0,0,0);
   fCheckButton835->SetWrapLength(-1);
   fGroupFrame825->AddFrame(fCheckButton835, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButton835->MoveResize(3,152,104,19);

   fGroupFrame825->SetLayoutManager(new TGVerticalLayout(fGroupFrame825));
   fGroupFrame825->Resize(119,520);
   fMainFrame5221->AddFrame(fGroupFrame825, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame825->MoveResize(160,32,119,520);
   TGTextButton *fTextButton838 = new TGTextButton(fMainFrame5221,"Submit");
   fTextButton838->SetTextJustify(36);
   fTextButton838->SetMargins(0,0,0,0);
   fTextButton838->SetWrapLength(-1);
   fTextButton838->Resize(92,24);
   fMainFrame5221->AddFrame(fTextButton838, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton838->MoveResize(576,592,92,24);

   // "Trigger Settings" group frame
   TGGroupFrame *fGroupFrame840 = new TGGroupFrame(fMainFrame5221,"Trigger Settings");
   fGroupFrame840->SetTitlePos(TGGroupFrame::kCenter);
   fGroupFrame840->SetLayoutBroken(kTRUE);
   TGLabel *fLabel853 = new TGLabel(fGroupFrame840,"Data Throttle Period");
   fLabel853->SetTextJustify(36);
   fLabel853->SetMargins(0,0,0,0);
   fLabel853->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel853, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel853->MoveResize(8,32,120,18);
   TGLabel *fLabel854 = new TGLabel(fGroupFrame840,"DAC");
   fLabel854->SetTextJustify(36);
   fLabel854->SetMargins(0,0,0,0);
   fLabel854->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel854, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel854->MoveResize(8,232,40,18);
   TGNumberEntry *fNumberEntry855 = new TGNumberEntry(fGroupFrame840, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1,(TGNumberFormat::ELimit) 2,0,4095);
   fGroupFrame840->AddFrame(fNumberEntry855, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry855->MoveResize(8,296,59,22);

   ULong_t ucolor;        // will reflect user color changes
   gClient->GetColorByName("#ffffff",ucolor);

   // combo box
   TGComboBox *fComboBox859 = new TGComboBox(fGroupFrame840,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox859->AddEntry("DAC 1 ",0);
   fComboBox859->AddEntry("DAC 2 ",1);
   fComboBox859->AddEntry("DAC 3 ",2);
   fComboBox859->AddEntry("DAC 4 ",3);
   fComboBox859->Resize(102,22);
   fComboBox859->Select(0);
   fGroupFrame840->AddFrame(fComboBox859, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox859->MoveResize(8,256,102,22);
   TGNumberEntry *fNumberEntry877 = new TGNumberEntry(fGroupFrame840, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1,(TGNumberFormat::ELimit) 2,0,4095);
   fGroupFrame840->AddFrame(fNumberEntry877, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry877->MoveResize(8,328,59,22);
   TGNumberEntry *fNumberEntry881 = new TGNumberEntry(fGroupFrame840, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5,(TGNumberFormat::EAttribute) 1,(TGNumberFormat::ELimit) 2,0,4095);
   fGroupFrame840->AddFrame(fNumberEntry881, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry881->MoveResize(8,360,59,22);
   TGNumberEntry *fNumberEntry885 = new TGNumberEntry(fGroupFrame840, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1,(TGNumberFormat::ELimit) 2,0,4095);
   fGroupFrame840->AddFrame(fNumberEntry885, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry885->MoveResize(8,392,59,22);
   TGLabel *fLabel889 = new TGLabel(fGroupFrame840,"#1");
   fLabel889->SetTextJustify(36);
   fLabel889->SetMargins(0,0,0,0);
   fLabel889->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel889, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel889->MoveResize(80,296,24,18);
   TGLabel *fLabel890 = new TGLabel(fGroupFrame840,"#2");
   fLabel890->SetTextJustify(36);
   fLabel890->SetMargins(0,0,0,0);
   fLabel890->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel890, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel890->MoveResize(80,328,24,18);
   TGLabel *fLabel891 = new TGLabel(fGroupFrame840,"#3");
   fLabel891->SetTextJustify(36);
   fLabel891->SetMargins(0,0,0,0);
   fLabel891->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel891, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel891->MoveResize(80,360,24,18);
   TGLabel *fLabel892 = new TGLabel(fGroupFrame840,"#4");
   fLabel892->SetTextJustify(36);
   fLabel892->SetMargins(0,0,0,0);
   fLabel892->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel892, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel892->MoveResize(80,392,24,18);
   TGLabel *fLabel896 = new TGLabel(fGroupFrame840,"(ms)");
   fLabel896->SetTextJustify(36);
   fLabel896->SetMargins(0,0,0,0);
   fLabel896->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel896, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel896->MoveResize(72,56,24,18);
   TGLabel *fLabel897 = new TGLabel(fGroupFrame840,"(sec)");
   fLabel897->SetTextJustify(36);
   fLabel897->SetMargins(0,0,0,0);
   fLabel897->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel897, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel897->MoveResize(72,168,32,18);
   TGNumberEntry *fNumberEntry844 = new TGNumberEntry(fGroupFrame840, (Double_t) 1,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 2,(TGNumberFormat::ELimit) 2,1,2.14748e+09);
   fGroupFrame840->AddFrame(fNumberEntry844, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry844->MoveResize(8,168,59,22);
   TGNumberEntry *fNumberEntry1696 = new TGNumberEntry(fGroupFrame840, (Double_t) 1,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1,(TGNumberFormat::ELimit) 2,50,600000);
   fGroupFrame840->AddFrame(fNumberEntry1696, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry1696->MoveResize(8,56,59,22);

	// Thermal Trigger Check Button
   TGCheckButton *fTextButton1868 = new TGCheckButton(fGroupFrame840,"Thermal Trigger");
   fTextButton1868->SetState(kButtonDown);	
   fTextButton1868->SetTextJustify(36);
   fTextButton1868->SetMargins(0,0,0,0);
   fTextButton1868->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fTextButton1868, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton1868->MoveResize(8,103,116,19);
   TGLabel *fLabel852 = new TGLabel(fGroupFrame840,"Forced Trigger Period");
   fLabel852->SetTextJustify(36);
   fLabel852->SetMargins(0,0,0,0);
   fLabel852->SetWrapLength(-1);
   fGroupFrame840->AddFrame(fLabel852, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel852->MoveResize(8,144,128,18);

   fGroupFrame840->SetLayoutManager(new TGVerticalLayout(fGroupFrame840));
   fGroupFrame840->Resize(144,520);
   fMainFrame5221->AddFrame(fGroupFrame840, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame840->MoveResize(288,32,144,520);

   // "Patterns" group frame
   TGGroupFrame *fGroupFrame898 = new TGGroupFrame(fMainFrame5221,"Patterns");
   fGroupFrame898->SetTitlePos(TGGroupFrame::kCenter);
   fGroupFrame898->SetLayoutBroken(kTRUE);
   TGLabel *fLabel899 = new TGLabel(fGroupFrame898,"High PLA");
   fLabel899->SetTextJustify(36);
   fLabel899->SetMargins(0,0,0,0);
   fLabel899->SetWrapLength(-1);
   fGroupFrame898->AddFrame(fLabel899, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel899->MoveResize(8,96,64,18);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valEntry900;
   valEntry900.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry900.fForeground);
   gClient->GetColorByName("#e0e0e0",valEntry900.fBackground);
   valEntry900.fFillStyle = kFillSolid;
   valEntry900.fFont = ufont->GetFontHandle();
   valEntry900.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry900, kTRUE);
   TGTextEntry *fTextEntry900 = new TGTextEntry(fGroupFrame898, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry900->SetMaxLength(4096);
   fTextEntry900->SetAlignment(kTextLeft);
   fTextEntry900->SetText("LAAAAAAA");
   fTextEntry900->Resize(93,fTextEntry900->GetDefaultHeight());
   fGroupFrame898->AddFrame(fTextEntry900, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry900->MoveResize(8,181,93,22);
   TGLabel *fLabel901 = new TGLabel(fGroupFrame898,"Low PLA");
   fLabel901->SetTextJustify(36);
   fLabel901->SetMargins(0,0,0,0);
   fLabel901->SetWrapLength(-1);
   fGroupFrame898->AddFrame(fLabel901, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel901->MoveResize(8,153,64,18);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valEntry902;
   valEntry902.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry902.fForeground);
   gClient->GetColorByName("#e0e0e0",valEntry902.fBackground);
   valEntry902.fFillStyle = kFillSolid;
   valEntry902.fFont = ufont->GetFontHandle();
   valEntry902.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry902, kTRUE);
   TGTextEntry *fTextEntry902 = new TGTextEntry(fGroupFrame898, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry902->SetMaxLength(4096);
   fTextEntry902->SetAlignment(kTextLeft);
   fTextEntry902->SetText("HAAAAAAA");
   fTextEntry902->Resize(93,fTextEntry902->GetDefaultHeight());
   fGroupFrame898->AddFrame(fTextEntry902, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry902->MoveResize(8,120,93,22);

   gClient->GetColorByName("#ffffff",ucolor);

	// Patterns 
   TGComboBox *fComboBox903 = new TGComboBox(fGroupFrame898,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox903->AddEntry("Pattern #1 ",0);
   fComboBox903->AddEntry("Pattern #2 ",1);
   fComboBox903->AddEntry("Pattern #3 ",2);
   fComboBox903->AddEntry("Pattern #4 ",3);
   fComboBox903->AddEntry("Pattern #5 ",4);
   fComboBox903->AddEntry("Pattern #6 ",5);
   fComboBox903->AddEntry("Pattern #7 ",6);
   fComboBox903->AddEntry("Pattern #8 ",7);
   fComboBox903->AddEntry("Pattern #9 ",8);
   fComboBox903->AddEntry("Pattern #10 ",9);
   fComboBox903->AddEntry("Pattern #11 ",10);
   fComboBox903->AddEntry("Pattern #12 ",11);
   fComboBox903->AddEntry("Pattern #13 ",12);
   fComboBox903->AddEntry("Pattern #14 ",13);
   fComboBox903->AddEntry("Pattern #15 ",14);
   fComboBox903->AddEntry("Pattern #16 ",15);
   fComboBox903->AddEntry("Pattern #17 ",16);
   fComboBox903->AddEntry("Pattern #18 ",17);
   fComboBox903->AddEntry("Pattern #19 ",18);
   fComboBox903->AddEntry("Pattern #20 ",19);
   fComboBox903->AddEntry("Pattern #21 ",20);
   fComboBox903->AddEntry("Pattern #22 ",21);
   fComboBox903->AddEntry("Pattern #23 ",22);
   fComboBox903->AddEntry("Pattern #24 ",23);
   fComboBox903->AddEntry("Pattern #25 ",24);
   fComboBox903->AddEntry("Pattern #26 ",25);
   fComboBox903->AddEntry("Pattern #27 ",26);
   fComboBox903->AddEntry("Pattern #28 ",27);
   fComboBox903->AddEntry("Pattern #29 ",28);
   fComboBox903->AddEntry("Pattern #30 ",29);
   fComboBox903->AddEntry("Pattern #31 ",30);
   fComboBox903->AddEntry("Pattern #32 ",31);
   fComboBox903->Resize(102,22);
   fComboBox903->Select(0);
   fGroupFrame898->AddFrame(fComboBox903, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox903->MoveResize(8,56,102,22);
   TGLabel *fLabel921 = new TGLabel(fGroupFrame898,"Pattern #");
   fLabel921->SetTextJustify(36);
   fLabel921->SetMargins(0,0,0,0);
   fLabel921->SetWrapLength(-1);
   fGroupFrame898->AddFrame(fLabel921, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel921->MoveResize(7,33,56,18);

   fGroupFrame898->SetLayoutManager(new TGVerticalLayout(fGroupFrame898));
   fGroupFrame898->Resize(126,520);
   fMainFrame5221->AddFrame(fGroupFrame898, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame898->MoveResize(441,32,126,520);

   // "General Settings" group frame
   TGGroupFrame *fGroupFrame922 = new TGGroupFrame(fMainFrame5221,"General Settings");
   fGroupFrame922->SetTitlePos(TGGroupFrame::kCenter);
   fGroupFrame922->SetLayoutBroken(kTRUE);

   gClient->GetColorByName("#ffffff",ucolor);

	// User names
   TGComboBox *fComboBox923 = new TGComboBox(fGroupFrame922,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox923->AddEntry("Joulien ",0);
   fComboBox923->AddEntry("Corey ",1);
   fComboBox923->AddEntry("Eric ",2);
   fComboBox923->AddEntry("Jordan ",3);
   fComboBox923->AddEntry("Liang ",4);
   fComboBox923->AddEntry("Mahshid ",6);
   fComboBox923->Resize(102,22);
   fComboBox923->Select(0);
   fGroupFrame922->AddFrame(fComboBox923, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox923->MoveResize(8,160,102,22);
   TGLabel *fLabel940 = new TGLabel(fGroupFrame922,"User Name");
   fLabel940->SetTextJustify(36);
   fLabel940->SetMargins(0,0,0,0);
   fLabel940->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel940, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel940->MoveResize(8,136,72,18);
   TGCheckButton *fCheckButton941 = new TGCheckButton(fGroupFrame922,"Email Alerts");
   fCheckButton941->SetToolTipText("Enable/Disable Email Alerts");
   fCheckButton941->SetTextJustify(36);
   fCheckButton941->SetMargins(0,0,0,0);
   fCheckButton941->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fCheckButton941, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButton941->MoveResize(8,200,88,19);

   gClient->GetColorByName("#ffffff",ucolor);

   // combo box
   TGComboBox *fComboBox944 = new TGComboBox(fGroupFrame922,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox944->AddEntry("Data Taking ",0);
   fComboBox944->AddEntry("Calibration ",1);
   fComboBox944->Resize(102,22);
   fComboBox944->Select(0);
   fGroupFrame922->AddFrame(fComboBox944, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox944->MoveResize(8,408,102,22);
   TGLabel *fLabel962 = new TGLabel(fGroupFrame922,"Config. Type");
   fLabel962->SetTextJustify(36);
   fLabel962->SetMargins(0,0,0,0);
   fLabel962->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel962, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel962->MoveResize(8,376,80,18);
   TGNumberEntry *fNumberEntry963 = new TGNumberEntry(fGroupFrame922, (Double_t) 1,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 2,(TGNumberFormat::ELimit) 2,1,1000);
   fGroupFrame922->AddFrame(fNumberEntry963, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry963->MoveResize(8,96,59,22);
   TGLabel *fLabel967 = new TGLabel(fGroupFrame922,"Station #");
   fLabel967->SetTextJustify(36);
   fLabel967->SetMargins(0,0,0,0);
   fLabel967->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel967, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel967->MoveResize(8,72,57,18);
   TGLabel *fLabel968 = new TGLabel(fGroupFrame922,"Timestamp");
   fLabel968->SetTextJustify(36);
   fLabel968->SetMargins(0,0,0,0);
   fLabel968->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel968, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel968->MoveResize(8,32,57,18);
   TGLabel *fLabel969 = new TGLabel(fGroupFrame922,"Comm. Window");
   fLabel969->SetTextJustify(36);
   fLabel969->SetMargins(0,0,0,0);
   fLabel969->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel969, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel969->MoveResize(8,240,96,18);
   TGNumberEntry *fNumberEntry970 = new TGNumberEntry(fGroupFrame922, (Double_t) 1,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 2);
   fGroupFrame922->AddFrame(fNumberEntry970, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry970->MoveResize(8,264,59,22);
   TGLabel *fLabel974 = new TGLabel(fGroupFrame922,"Comm. Frequency");
   fLabel974->SetTextJustify(36);
   fLabel974->SetMargins(0,0,0,0);
   fLabel974->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel974, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel974->MoveResize(8,304,104,18);
   TGNumberEntry *fNumberEntry975 = new TGNumberEntry(fGroupFrame922, (Double_t) 1,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 2);
   fGroupFrame922->AddFrame(fNumberEntry975, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry975->MoveResize(8,328,59,22);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t vall979;
   vall979.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall979.fForeground);
   gClient->GetColorByName("#e0e0e0",vall979.fBackground);
   vall979.fFillStyle = kFillSolid;
   vall979.fFont = ufont->GetFontHandle();
   vall979.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall979, kTRUE);
   TGLabel *fLabel979 = new TGLabel(fGroupFrame922,"(min)",uGC->GetGC());
   fLabel979->SetTextJustify(36);
   fLabel979->SetMargins(0,0,0,0);
   fLabel979->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel979, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel979->MoveResize(72,264,32,18);
   TGLabel *fLabel980 = new TGLabel(fGroupFrame922,"(min)");
   fLabel980->SetTextJustify(36);
   fLabel980->SetMargins(0,0,0,0);
   fLabel980->SetWrapLength(-1);
   fGroupFrame922->AddFrame(fLabel980, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel980->MoveResize(72,328,32,18);

   fGroupFrame922->SetLayoutManager(new TGVerticalLayout(fGroupFrame922));
   fGroupFrame922->Resize(119,520);
   fMainFrame5221->AddFrame(fGroupFrame922, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fGroupFrame922->MoveResize(32,32,119,520);

   // "Alerts" group frame
   TGGroupFrame *fGroupFrame981 = new TGGroupFrame(fMainFrame5221,"Alerts");
   fGroupFrame981->SetLayoutBroken(kTRUE);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t vall982;
   vall982.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#ff0000",vall982.fForeground);
   gClient->GetColorByName("#e0e0e0",vall982.fBackground);
   vall982.fFillStyle = kFillSolid;
   vall982.fFont = ufont->GetFontHandle();
   vall982.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall982, kTRUE);
   TGLabel *fLabel982 = new TGLabel(fGroupFrame981,"Alerts Box",uGC->GetGC());
   fLabel982->SetTextJustify(36);
   fLabel982->SetMargins(0,0,0,0);
   fLabel982->SetWrapLength(-1);
   fGroupFrame981->AddFrame(fLabel982, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel982->MoveResize(6,15,528,32);

   fGroupFrame981->SetLayoutManager(new TGVerticalLayout(fGroupFrame981));
   fGroupFrame981->Resize(536,56);
   fMainFrame5221->AddFrame(fGroupFrame981, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame981->MoveResize(32,568,536,56);

   fMainFrame2119->AddFrame(fMainFrame5221, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fMainFrame5221->MoveResize(0,0,690,628);

   fMainFrame2119->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame2119->MapSubwindows();

   fMainFrame2119->Resize(fMainFrame2119->GetDefaultSize());
   fMainFrame2119->MapWindow();
   fMainFrame2119->Resize(691,628);
}  
