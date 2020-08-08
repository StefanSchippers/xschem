ROOT=..

include $(ROOT)/Makefile.conf

all:

install: FORCE
	$(SCCBOX) mkdir -p "$(XDOCDIR)" "$(XDOCDIR)"/xschem_man "$(MANDIR)"/man1
	$(SCCBOX) install -f -d *.svg *.html *.css *.png "$(XDOCDIR)"
	$(SCCBOX) install -f -d xschem_man/*.html xschem_man/*.css xschem_man/*.png "$(XDOCDIR)"/xschem_man
	$(SCCBOX) install -f manpages/xschem.1 "$(MANDIR)"/man1/xschem.1

uninstall: FORCE
	$(SCCBOX) rm -f "$(XDOCDIR)"/xschem_man/*
	$(SCCBOX) rm -f "$(XDOCDIR)"/*
	$(SCCBOX) rm -f "$(MANDIR)"/man1/xschem.1

clean:

distclean:
	$(SCCBOX) rm -f manpages/xschem.1

FORCE:


