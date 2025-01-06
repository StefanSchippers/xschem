all: FORCE
	cd src && $(MAKE)
	cd xschem_library && $(MAKE)
	cd doc && $(MAKE)
	cd src/utile && $(MAKE)

clean: FORCE
	cd src && $(MAKE) clean
	cd xschem_library && $(MAKE) clean
	cd doc && $(MAKE) clean
	cd src/utile && $(MAKE) clean

distclean: FORCE
	$(MAKE) clean ; true
	cd src && $(MAKE) distclean
	cd xschem_library && $(MAKE) distclean
	cd doc && $(MAKE) distclean
	cd src/utile && $(MAKE) distclean
	rm -f Makefile.conf config.h src/Makefile
	cd scconfig && make distclean

install: FORCE
	cd src && $(MAKE) install
	cd xschem_library && $(MAKE) install
	cd doc && $(MAKE) install
	cd src/utile && $(MAKE) install

install_pdf: FORCE
	cd doc && $(MAKE) install_pdf

uninstall: FORCE
	cd xschem_library && $(MAKE) uninstall
	cd doc && $(MAKE) uninstall
	cd src/utile && $(MAKE) uninstall
	cd src && $(MAKE) uninstall

FORCE:
