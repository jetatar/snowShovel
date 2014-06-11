# Makefile
# $Id: Makefile,v 1.4 2013-06-20 05:00:48 cjreed Exp $
#


# By default, makes pdf files from tex sources.
# The target to make can be
#	[filename without extension] => make filename.pdf with pdflatex
#	[filename.pdf] => same as above
#	[filename.dvi] => use normal latex to make dvi file
#	[filename.ps] => use normal latex to make dvi, then ps file
#
# Dependencies will be generated automatically for input and include 
# calls in tex files.

TEXDIR    = ./
PICDIR    = ./
TEXSRCS   = $(shell find $(TEXDIR) -maxdepth 1 -iname "*.tex") 
EPSPICS   = $(shell find $(PICDIR) -maxdepth 3 -iname "*.eps")
PICSRCS   = $(shell find $(PICDIR) -maxdepth 3 -iname "*.eps" \
              -or -iname "*.gif" -or -iname "*.jpeg" -or -iname "*.png")
PDFPICS   = $(patsubst %.eps,%.pdf,$(EPSPICS))
BIBFILES  = $(shell find $(TEXDIR) -maxdepth 1 -iname "*.bib")

ARCH      = $(shell uname -s)
PDFTEX    = pdflatex
LATEX	  = latex
BIBTEX	  = ./sh/bibtex.sh
MKINDEX   = makeindex
DVIPS	  = dvips
EPS2EPS   = eps2eps
EPSTOPDF  = epstopdf
THUMBPDF  = thumbpdf
EPSOPTION = 

CMDPDFS   = $(patsubst, %,%.pdf,$(MAKECMDGOALS))

PDFFILES  = $(patsubst %.tex,%.pdf,$(TEXSRCS))
DVIFILES  = $(patsubst %.tex,%.dvi,$(TEXSRCS))
PSFILES  = $(patsubst %.tex,%.ps,$(TEXSRCS))

ADDFILES  = $(BIBFILES) Makefile
BACKFILES = $(PICSRCS) $(TEXSRCS) $(ADDFILES) 

all:			$(PDFFILES)

%.d:	%.tex
	$(get_dependencies) ; $(get_bibs) ; \
	echo "$@ : $< $$deps $$bibs" > $@ 

include $(TEXSRCS:.tex=.d)

%: %.pdf
	@echo

%.pdf:	%.tex %.d $(PDFPICS) $(ADDFILES)
	$(PDFTEX) 	$<
	$(BIBTEX) 	$(TEXDIR)
#	$(MKINDEX)	$<
	$(PDFTEX) 	$<
#	$(THUMBPDF)     $<
	$(PDFTEX) 	$<

%.dvi:	%.tex %.d $(EPSPICS) $(ADDFILES)
	$(LATEX) 	$<
	$(BIBTEX) 	$<
#	$(MKINDEX) 	$<
	$(LATEX) 	$<
	$(LATEX) 	$<

%.ps:	%.dvi
	$(DVIPS) 	$< -o $@

define get_dependencies
	deps=`perl -ne '($$_)=/^[^%]*\\\(?:include|input)\{(.*?)\}/;@_=split /,/;foreach $$t (@_) {print "$$t.tex "}' $<`
endef

define get_bibs
	bibs=`perl -ne '($$_)=/^[^%]*\\\(?:bibliography.*)\{(.*?)\}/;@_=split /,/;foreach $$t (@_) {if (-e "$$t.bib") {print "$$t.bib "}}' $<`
endef

%.gz:	%
	gzip $<

archive: $(PDFFILES)
	tar -cvzf $(BACKFILES) $(PDFFILES)

sarchive:
	tar -cvzf $(BACKFILES)

clean:
	rm -f $(PDFFILES) $(DVIFILES) $(PSFILES) \
	$(TEXSRCS:%.tex=%.aux) $(TEXSRCS:%.tex=%.bbl) $(TEXSRCS:%.tex=%.blg) \
	$(TEXSRCS:%.tex=%.log) $(TEXSRCS:%.tex=%.out) $(TEXSRCS:%.tex=%.idx) \
	$(TEXSRCS:%.tex=%.ilg) $(TEXSRCS:%.tex=%.ind) $(TEXSRCS:%.tex=%.toc) \
	$(TEXSRCS:%.tex=%.d)

realclean: clean
	find . -type f -name "*~" -exec rm -f "{}" \;

.phony: clean realclean archive sarchive

$(PDFPICS): %.pdf: %.eps
ifeq ($(ARCH),Darwin)
	perl -i -pe 's/\015/\012/g' $<
endif
	$(EPSTOPDF) $(EPSOPTION) $< --outfile=$@
#	$(EPS2EPS) $< .tmp.eps && $(EPSTOPDF) $(EPSOPTION) .tmp.eps --outfile=$@ && rm -f .tmp.eps
