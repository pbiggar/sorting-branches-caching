LATEX = latex
LATEXFLAGS = -interaction=nonstopmode

DVIPS = dvips
DVIPSFLAGS = 

PS2PDF = ps2pdf
PS2PDFFLAGS = 

BIBTEX = bibtex

TITLE = report
DOCUMENT = $(TITLE).ps

FILES = background.tex conclusions.tex heap.tex merge.tex ordernsquared.tex radix.tex result_list.tex software_predictor.tex bugs.tex intro.tex method.tex quick.tex report.tex shell.tex


INTERMEDIATE_FILES = $(TITLE).ps $(TITLE).pdf $(TITLE).dvi $(TITLE).aux $(TITLE).log $(TITLE).lof $(TITLE).bbl $(TITLE).blg $(TITLE).toc

all: $(DOCUMENT)

again:
	$(LATEX) $(LATEXFLAGS) $(TITLE)
	$(BIBTEX) $(TITLE)
	$(LATEX) $(LATEXFLAGS) $(TITLE)
	$(LATEX) $(LATEXFLAGS) $(TITLE)

%.dvi: %.tex $(FILES)
	$(LATEX) $(LATEXFLAGS) $<
	$(BIBTEX) $(TITLE)
	$(LATEX) $(LATEXFLAGS) $<
	$(LATEX) $(LATEXFLAGS) $<

%.ps: %.dvi 
	$(DVIPS) $(DVIPSFLAGS) -o $@ $<

%.pdf: %.dvi
	$(DVIPS) $(DVIPSFLAGS) -Ppdf -o $*.ps $<
	$(PS2PDF) $*.ps

clean:
	rm -f $(INTERMEDIATE_FILES)
