from twisted.internet import stdio, defer
from twisted.protocols import basic
import curses

# setup term color
curses.setupterm()
setaf = curses.tigetstr('setaf')
if not setaf:
    setaf=""
setab = curses.tigetstr('setab')
if not setab:
    setab=""
bold = curses.tigetstr('bold')
if not bold:
    bold=""
norm = curses.tigetstr('sgr0')
if not norm:
    norm=""

from AriUtils import *

hkeys=enum(kInput='ii',                                     # input stage
           kStopTransfer='st', kDelRun='dr', kDelAll='da',  # option stage
           kConfirm='yy', kCancel='cc')                     # confirm stage
stage=enum(kInput=0, kOption=10, kConfirm=20, kLaunch=30)

termcols=enum(kBlack=0, kRed=1, kGreen=2, kYellow=3, kBlue=4,
              kMagenta=5, kCyan=6, kWhite=7, kDefault=9)

FGCOL=termcols.kYellow
BGCOL=termcols.kBlack

class AriFileTransferOpts(basic.LineReceiver):
    from os import linesep as delimiter
    
    stage = None
    opt   = None
    drun  = None

    def printfancy(self, msg):
        print curses.tparm(setaf,FGCOL) +\
              curses.tparm(setab,BGCOL) +\
              curses.tparm(bold) +\
              msg +\
              curses.tparm(setaf,termcols.kDefault) +\
              curses.tparm(setab,termcols.kDefault) +\
              curses.tparm(norm)
    
    def connectionMade(self):
        printout(vtype.kDebug,"########### made conn")
        self.setStage(stage.kInput,False)

    def connectionLost(self, reason):
        printout(vtype.kDebug,"########### lost conn")
        self.stage = None
        self.opt   = None
        self.drun  = None

    def setStage(self, s, withPrompt=True):
        if (s==stage.kInput):
            if (withPrompt):
                self.printfancy('  {0}<enter> for transfer interrupt options.'.
                                format(hkeys.kInput))
            self.opt  = None
            self.drun = None
        self.stage = s

    def hasInput(self):
        printout(vtype.kDebug,'STAGE={0}'.format(self.stage))
        return (self.stage==stage.kLaunch)

    def popInput(self):
        if self.hasInput():
            opt, run = self.opt, self.drun
            self.setStage(stage.kInput)
            return opt, run
        else:
            return None, None
    
    def lineReceived(self, line):
        if (self.stage==stage.kInput):
            # at the input stage
            if (line==hkeys.kInput):
                self.printfancy('Interrupt transfer. Type: ')
                self.printfancy('  {0}<enter> to stop transferring'.
                                format(hkeys.kStopTransfer))
                self.printfancy('  {0}:#<enter> to stop transferring and '
                                'delete an entire run, where # = run nunber'.
                                format(hkeys.kDelRun))
                self.printfancy('  {0}<enter> to stop transferring and '
                                'delete all runs'.format(hkeys.kDelAll))
                self.setStage(stage.kOption)
        elif (self.stage==stage.kOption):
            # at the option stage
            if (line==hkeys.kStopTransfer):
                self.opt = hkeys.kStopTransfer
                self.printfancy('Stop transferring. Confirm?')
                self.printfancy('  {0}<enter> to confirm'.
                                format(hkeys.kConfirm))
                self.setStage(stage.kConfirm)
            elif (line.startswith(hkeys.kDelRun)):
                try:
                    run = int(line.strip().split(':')[1])
                    self.opt = hkeys.kDelRun
                    self.printfancy('Stop transfer and delete run {0}. '
                                    'Confirm?'.
                                    format(run))
                    self.printfancy('  {0}<enter> to confirm'.
                                    format(hkeys.kConfirm))
                    self.drun = run
                    self.setStage(stage.kConfirm)
                except:
                    self.printfancy('Error parsing run number. '
                                    'Operation canceled.')
                    self.setStage(stage.kInput)
            elif (line==hkeys.kDelAll):
                self.opt = hkeys.kDelAll
                self.printfancy('Delete ALL runs. Confirm?')
                self.printfancy('  {0}<enter> to confirm'.
                                format(hkeys.kConfirm))
                self.setStage(stage.kConfirm)
            else:
                self.printfancy('Unknown option. Canceling request.')
                self.setStage(stage.kInput)
        elif (self.stage==stage.kConfirm):
            # at the confirmation stage
            if (line==hkeys.kConfirm):
                self.printfancy('Confirmed.')
                self.printfancy('  {0}<enter> to cancel'.format(hkeys.kCancel))
                self.setStage(stage.kLaunch)
            else:
                self.printfancy('Not confirmed. Canceling request.')
                self.setStage(stage.kInput)
        elif (self.stage==stage.kLaunch):
            # at the confirmed, ready to launch stage
            if (line==hkeys.kCancel):
                self.printfancy('Canceling request.')
                self.setStage(stage.kInput)
        else:
            # at an unkown stage??
            self.printfancy('Not expecting input.')
    

fto = AriFileTransferOpts()
stdio.StandardIO(fto)
