ROOT = ..

all:

include ../Makefile.conf

install: FORCE
	$(SCCBOX) mkdir -p "$(system_library_dir)" "$(XDOCDIR)"/examples "$(XDOCDIR)"/pcb
	$(SCCBOX) mkdir -p "$(XDOCDIR)"/logic "$(XDOCDIR)"/xTAG "$(XDOCDIR)"/binto7seg 
	$(SCCBOX) mkdir -p "$(XDOCDIR)"/symgen "$(XDOCDIR)"/ngspice "$(XDOCDIR)"/rulz-r8c33
	$(SCCBOX) mkdir -p "$(XDOCDIR)"/generators
	$(SCCBOX) mkdir -p "$(XDOCDIR)"/inst_sch_select
	$(SCCBOX) mkdir -p "$(XDOCDIR)"/rom8k "$(XDOCDIR)"/gschem_import/sym
	$(SCCBOX) install -f -d devices/*.sym "$(system_library_dir)"
	$(SCCBOX) install -f -d devices/*.sch "$(system_library_dir)"
	$(SCCBOX) install -f -d examples/*.sym "$(XDOCDIR)"/examples
	$(SCCBOX) install -f -d examples/*.sch "$(XDOCDIR)"/examples
	$(SCCBOX) install -f -d examples/stimuli.* "$(XDOCDIR)"/examples
	$(SCCBOX) install -f -d binto7seg/*.sym "$(XDOCDIR)"/binto7seg
	$(SCCBOX) install -f -d binto7seg/*.sch "$(XDOCDIR)"/binto7seg
	$(SCCBOX) install -f -d generators/* "$(XDOCDIR)"/generators
	$(SCCBOX) install -f -d inst_sch_select/*.* "$(XDOCDIR)"/inst_sch_select
	$(SCCBOX) install -f -d xTAG/*.sym* "$(XDOCDIR)"/xTAG
	$(SCCBOX) install -f -d xTAG/*.sch "$(XDOCDIR)"/xTAG
	$(SCCBOX) install -f -d logic/*.sym "$(XDOCDIR)"/logic
	$(SCCBOX) install -f -d logic/*.sch "$(XDOCDIR)"/logic
	$(SCCBOX) install -f -d logic/stimuli.* "$(XDOCDIR)"/logic
	$(SCCBOX) install -f -d rulz-r8c33/*.sch "$(XDOCDIR)"/rulz-r8c33
	$(SCCBOX) install -f -d rulz-r8c33/*.sym* "$(XDOCDIR)"/rulz-r8c33
	$(SCCBOX) install -f -d pcb/*.sch "$(XDOCDIR)"/pcb
	$(SCCBOX) install -f -d pcb/*.sym "$(XDOCDIR)"/pcb
	$(SCCBOX) install -f -d ngspice/*.sym "$(XDOCDIR)"/ngspice
	$(SCCBOX) install -f -d ngspice/*.sch "$(XDOCDIR)"/ngspice
	$(SCCBOX) install -f -d ngspice/stimuli.* "$(XDOCDIR)"/ngspice
	$(SCCBOX) install -f -d rom8k/*.sym "$(XDOCDIR)"/rom8k
	$(SCCBOX) install -f -d rom8k/*.sch "$(XDOCDIR)"/rom8k
	$(SCCBOX) install -f -d xschem_simulator/*.sym "$(XDOCDIR)"/xschem_simulator
	$(SCCBOX) install -f -d xschem_simulator/*.sch "$(XDOCDIR)"/xschem_simulator
	$(SCCBOX) install -f -d rom8k/stimuli.* "$(XDOCDIR)"/rom8k
	$(SCCBOX) install -f -d symgen/*.sym* "$(XDOCDIR)"/symgen
	$(SCCBOX) install -f -d gschem_import/*.sym "$(XDOCDIR)"/gschem_import
	$(SCCBOX) install -f -d gschem_import/*.sch "$(XDOCDIR)"/gschem_import
	$(SCCBOX) install -f -d gschem_import/sym/*.sym "$(XDOCDIR)"/gschem_import/sym

uninstall: FORCE
	$(SCCBOX) rm -f "$(system_library_dir)"/* "$(XDOCDIR)"/examples/* "$(XDOCDIR)"/pcb/* \
                        "$(XDOCDIR)"/ngspice/* "$(XDOCDIR)"/symgen/* "$(XDOCDIR)"/logic/* "$(XDOCDIR)"/xTAG/* \
                        "$(XDOCDIR)"/rom8k/* "$(XDOCDIR)"/xschem_simulator/* "$(XDOCDIR)"/binto7seg/* \
                        "$(XDOCDIR)"/rulz-r8c33/* "$(XDOCDIR)"/generators/* "$(XDOCDIR)"/inst_sch_select/* \
                        "$(XDOCDIR)"/gschem_import/*.sym \
                        "$(XDOCDIR)"/gschem_import/*.sch \
                        "$(XDOCDIR)"/gschem_import/sym/*.sym \
                        "$(XDOCDIR)"/gschem_import/sym
clean:

distclean:

FORCE:

