v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 3 910 -560 960 -540 {}
L 3 880 -680 1200 -680 {}
L 8 900 -540 950 -560 {}
L 11 900 -550 950 -530 {dash=4}
L 16 910 -530 960 -550 {}
B 4 940 -510 960 -450 {}
B 7 900 -490 970 -470 {dash=4}
B 8 910 -510 930 -450 {}
A 4 925 -95 35.35533905932738 8.13010235415598 360 {}
A 6 990 -150 70.71067811865476 188.130102354156 89.99999999999997 {}
A 7 934.1666666666666 -90 39.58991173406564 210.3432488842396 198.9246444160511 {}
P 1 10 930 -370 920 -430 1000 -410 960 -390 990 -360 970 -360 960 -340 940 -380 930 -360 930 -370 {dash=3}
P 4 6 880 -350 880 -440 870 -390 870 -430 860 -400 860 -370 {}
P 15 11 920 -400 930 -440 950 -410 960 -430 970 -400 990 -410 970 -370 940 -380 910 -360 890 -410 920 -400 {fill=true}
T {Welcome to XSCHEM!} 110 -990 0 0 1 1 {layer=5}
T {This is a test schematic window 

On the left you see some sample circuits. You may descend into any
of these by selecting one with a left mouse button click and
pressing the 'e' key, or by menu 'Edit -> Push Schematic'.
You can return here after descending into a schematic by hitting 
'<Ctrl>-e' or by menu 'Edit -> Pop'.
} 60 -920 0 0 0.4 0.4 {}
T {Lines} 1010 -560 0 0 0.6 0.6 {layer=4}
T {Rectangles} 1010 -500 0 0 0.6 0.6 {layer=4}
T {Polygons} 1010 -410 0 0 0.6 0.6 {layer=4}
T {Electrical
wires} 1010 -330 0 0 0.6 0.6 {layer=4}
T {XSCHEM OBJECTS} 910 -720 0 0 0.6 0.6 {layer=7}
T {Components} 1010 -210 0 0 0.6 0.6 {layer=4}
T {TEXT} 890 -620 0 0 0.5 0.5 {layer=8}
T {TEXT} 890 -670 1 0 0.7 0.7 {layer=6 font=Monospace}
T {Text} 900 -660 0 0 0.5 0.5 {layer=7 font="serif"}
T {TEXT} 990 -660 1 0 0.5 0.5 {weight=bold}
T {Text} 1010 -650 0 0 0.6 0.6 {layer=4}
T {Arcs/Circles} 1010 -120 0 0 0.6 0.6 {layer=4}
T {Select the components here under
and press'<Ctrl>-h' or <Ctrl>-click
them to visit xschem websites
or local docs} 400 -360 0 0 0.3 0.3 {slant=oblique }
N 870 -300 990 -300 {lab=#net1}
N 910 -340 910 -250 {lab=#net2}
N 910 -270 970 -270 {lab=#net2}
N 930 -310 960 -300 {lab=#net1}
N 380 -530 700 -530 {lab=BUS[4:0]}
N 510 -640 510 -540 {lab=BUS[1]}
N 410 -600 410 -540 {lab=BUS[2]}
C {poweramp.sym} 160 -460 0 0 {name=x1
tclcommand="xschem descend"}
C {tesla.sym} 160 -380 0 0 {name=x2}
C {test_ne555.sym} 160 -340 0 0 {name=x3}
C {test_lm324.sym} 160 -300 0 0 {name=x4}
C {osc.sym} 160 -420 0 0 {name=x5}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {cmos_example.sym} 160 -500 0 0 {name=x6}
C {greycnt.sym} 160 -540 0 0 {name=x8}
C {loading.sym} 160 -580 0 0 {name=x9}
C {inv_bsource.sym} 930 -200 0 0 {name=B1 TABLE="1.4 3.0 1.6 0.0"}
C {launcher.sym} 460 -210 0 0 {name=h1 
descr="XSCHEM ON REPO.HU" 
url="http://repo.hu/projects/xschem"
program=x-www-browser}
C {launcher.sym} 460 -160 0 0 {name=h3 
descr="Toggle light/dark 
colorscheme" 
tclcommand="xschem toggle_colorscheme"
}
C {launcher.sym} 460 -260 0 0 {name=h2 
descr="LOCAL DOCUMENTATION" 
url="$\{XSCHEM_SHAREDIR\}/../doc/xschem/index.html"
program=x-www-browser

}
C {rlc.sym} 160 -620 0 0 {name=x0}
C {lab_pin.sym} 700 -530 0 1 {name=l2 sig_type=std_logic lab=BUS[4:0]}
C {bus_connect.sym} 500 -530 0 0 {name=l3 lab=BUS[1]}
C {bus_connect_nolab.sym} 400 -530 0 0 {name=r1}
C {lab_pin.sym} 410 -600 3 1 {name=l4 sig_type=std_logic lab=BUS[2]}
C {LCC_instances.sym} 160 -260 0 0 {name=x7}
C {test_backannotated_subckt.sym} 160 -220 0 0 {name=x10}
C {plot_manipulation.sym} 160 -180 0 0 {name=x11}
C {logic_test.sym} 160 -140 0 0 {name=x12}
C {simulate_ff.sym} 160 -100 0 0 {name=x13}
