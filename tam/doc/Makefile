# $Id: Makefile 1689 2005-09-21 21:17:40Z maartenb $
#############################


TEXSDIR   = ./
PICSDIR   = ./pics 
FILENAME  = tamdoc

###

TEXSRCS   = $(shell find $(TEXSDIR) -iname "*.tex")
PICS      = $(shell find $(PICSDIR) -iname "*.eps")

ADDFILES  = Makefile

BACKFILES = $(PICS) $(TEXSRCS)

PDFPICS   = $(patsubst %.eps,%.pdf,$(PICS))

LATEX	  = latex
PDFTEX    = pdflatex
BIBTEX    = bibtex
DVIPS	  = dvips
PS2PDF    = ps2pdf
EPS2EPS   = eps2eps
EPSTOPDF  = epstopdf
THUMBPDF  = thumbpdf
L2HTML    = latex2html
#EPSOPTION = --hires 

WWWSRVDIR = pgate.mit.edu:/var/www/html/tam
WWWDOCDIR = html_docu 

default:	$(FILENAME).pdf

$(FILENAME):		$(FILENAME).dvi

$(FILENAME).pdf:	$(FILENAME).tex $(TEXSRCS) $(ADDFILES) $(PDFPICS) 
	$(PDFTEX) 	$(FILENAME)
#	$(BIBTEX) 	$(FILENAME)
	$(PDFTEX) 	$(FILENAME)
	$(PDFTEX) 	$(FILENAME)

$(FILENAME).dvi:	$(FILENAME).tex $(TEXSRCS) $(ADDFILES)
	$(LATEX) 	$(FILENAME)
#	$(BIBTEX) 	$(FILENAME)
	$(LATEX) 	$(FILENAME)
	$(LATEX) 	$(FILENAME)

$(FILENAME).ps:		$(FILENAME).dvi $(PICS)
	$(DVIPS) $(FILENAME).dvi -o $(FILENAME).ps


$(FILENAME).tar.gz:	backup

$(FILENAME).html:	$(FILENAME).dvi
	rm -rf $(WWWDOCDIR)
	mkdir $(WWWDOCDIR)
	$(L2HTML) -t "TAM documentation" -dir $(WWWDOCDIR) $(FILENAME).tex

dvi:	$(FILENAME).dvi

ps:	$(FILENAME).ps

gzip:	ps
	gzip $(FILENAME).ps

pdf:	$(FILENAME).pdf

fpdf:	$(FILENAME).tex $(TEXSRCS) $(ADDFILES) $(PDFPICS) 
	$(PDFTEX) 	$(FILENAME)

ps2pdf:	$(FILENAME).dvi
	$(DVIPS) -Ppdf $(FILENAME).dvi -o $(FILENAME).ps
	$(PS2PDF) $(FILENAME).ps

print:  $(FILENAME).ps
	lpr $(FILENAME).ps

vdvi:	$(FILENAME).dvi
	xdvi $(FILENAME).dvi &

vps:	$(FILENAME).ps
	gv $(FILENAME).ps &

vpdf:	$(FILENAME).pdf
	xpdf $(FILENAME).pdf &

html:	$(FILENAME).ps $(FILENAME).pdf $(FILENAME).html
	scp -p $(FILENAME).ps $(FILENAME).pdf $(WWWSRVDIR)
	scp -p -r $(WWWDOCDIR) $(WWWSRVDIR)

clean:
	rm -f $(FILENAME).dvi $(FILENAME).toc .tmp.eps	\
	$(FILENAME).aux $(FILENAME).out $(FILENAME).tpt \
	$(FILENAME).bbl $(FILENAME).blg $(FILENAME).log \
	$(FILENAME).lof $(FILENAME).lot $(FILENAME).idx \
	$(FILENAME).ind $(FILENAME).ilg $(FILENAME).glo \
	$(FILENAME).blg $(FILENAME).brf missfont.log \
        texput.log core *.aux *.tmp
	find $(TEXSDIR) $(PICSDIR) -name "*~" -type f -exec rm -f "{}" \;
	find $(TEXSDIR) $(PICSDIR) -type d -exec chmod 755 "{}" \;
	find $(TEXSDIR) $(PICSDIR) -type f -exec chmod 644 "{}" \;
	find $(TEXSDIR) -name "*.sh" -type f -exec chmod 755 "{}" \;

realclean: clean
	rm -f $(FILENAME).ps.gz $(FILENAME).pdf $(PDFPICS) $(FILENAME).ps $(FILENAME).tar.gz archiv.tar.gz
	rm -rf $(WWWDOCDIR)

archiv:
	tar -cvzf archiv.tar.gz $(FILENAME).tex $(BACKFILES)

backup:
	tar -cvzf $(FILENAME).tar.gz $(FILENAME).tex $(BACKFILES) $(ADDFILES)

export: pdf ps html
	cp -a $(FILENAME).pdf $(WWWDOCDIR) $(WEBDIR)

.phony: print clean backup export

$(PDFPICS): %.pdf: %.eps
	$(EPS2EPS) $< .tmp.eps && $(EPSTOPDF) $(EPSOPTION) .tmp.eps --outfile=$@ && rm -f .tmp.eps
