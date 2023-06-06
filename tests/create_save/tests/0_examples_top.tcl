xschem line 910 -560 960 -540 
xschem line 880 -680 1200 -680 
xschem line 900 -540 950 -560 
xschem line 900 -550 950 -530 
xschem line 910 -530 960 -550 
xschem rect 940 -510 960 -450 
xschem rect 900 -490 970 -470 
xschem rect 910 -510 930 -450 
xschem wire 870 -300 990 -300 {lab=#net1}
xschem wire 910 -340 910 -250 {lab=#net2}
xschem wire 910 -270 970 -270 {lab=#net2}
xschem wire 930 -310 960 -300 {lab=#net1}
xschem wire 380 -530 700 -530 {lab=BUS[4:0]}
xschem wire 510 -640 510 -540 {lab=BUS[1]}
xschem wire 410 -600 410 -540 {lab=BUS[2]}
xschem instance ../../../xschem_library/examples/poweramp.sym 160 -310 0 0 {name=x1 tclcommand="xschem descend"}
xschem instance ../../../xschem_library/examples/tesla.sym 160 -230 0 0 {name=x2}
xschem instance ../../../xschem_library/examples/test_ne555.sym 160 -190 0 0 {name=x3}
xschem instance ../../../xschem_library/examples/test_lm324.sym 160 -150 0 0 {name=x4}
xschem instance ../../../xschem_library/examples/osc.sym 160 -270 0 0 {name=x5}
xschem instance ../../../xschem_library/devices/title.sym 160 -30 0 0 {name=l1 author="Stefan Schippers"}
xschem instance ../../../xschem_library/examples/cmos_example.sym 160 -350 0 0 {name=x6}
xschem instance ../../../xschem_library/examples/greycnt.sym 160 -390 0 0 {name=x8}
xschem instance ../../../xschem_library/examples/loading.sym 160 -430 0 0 {name=x9}
xschem instance ../../../xschem_library/examples/and.sym 930 -200 0 0 {name=E1 TABLE="1.4 3.0 1.6 0.0"}
xschem instance ../../../xschem_library/devices/launcher.sym 460 -210 0 0 {name=h1 descr="XSCHEM ON REPO.HU" url="http://repo.hu/projects/xschem"program=x-www-browser}
xschem instance ../../../xschem_library/devices/launcher.sym 460 -160 0 0 {name=h3 descr="Toggle light/dark colorscheme" tclcommand="xschem toggle_colorscheme"}
xschem instance ../../../xschem_library/devices/launcher.sym 460 -260 0 0 {name=h2 descr="LOCAL DOCUMENTATION" url="$\{XSCHEM_SHAREDIR\}/../doc/xschem/index.html"program=x-www-browser}
xschem instance ../../../xschem_library/examples/rlc.sym 160 -470 0 0 {name=x0}
xschem instance ../../../xschem_library/devices/lab_pin.sym 700 -530 0 1 {name=l2 sig_type=std_logic lab=BUS[4:0]}
xschem instance ../../../xschem_library/devices/bus_connect.sym 500 -530 0 0 {name=l3 lab=BUS[1]}
xschem instance ../../../xschem_library/devices/bus_connect_nolab.sym 400 -530 0 0 {name=r1}
xschem instance ../../../xschem_library/devices/lab_pin.sym 410 -600 3 1 {name=l4 sig_type=std_logic lab=BUS[2]}
xschem save
xschem exit closewindow