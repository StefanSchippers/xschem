ROOT = ../..

all:

include $(ROOT)/Makefile.conf

install: FORCE
	$(SCCBOX) mkdir -p "$(XSHAREDIR)/utile"
	$(SCCBOX) install -f -d * "$(XSHAREDIR)/utile"
	$(SCCBOX) rm "$(XSHAREDIR)/utile/Makefile"

uninstall: FORCE
	$(SCCBOX) rm -f "$(XSHAREDIR)"/utile/*

clean:

distclean:

FORCE:


