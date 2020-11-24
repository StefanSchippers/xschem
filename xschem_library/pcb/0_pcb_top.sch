v {xschem version=2.9.8 file_version=1.2}
G {}
K {}
V {}
S {}
E {}
L 3 910 -560 960 -540 {}
L 3 880 -680 1200 -680 {}
L 8 900 -540 950 -560 {}
L 11 900 -550 950 -530 {}
L 16 910 -530 960 -550 {}
B 4 940 -510 960 -450 {}
B 7 900 -490 970 -470 {}
B 8 910 -510 930 -450 {}
A 4 925 -95 35.35533905932738 8.13010235415598 360 {}
A 6 990 -150 70.71067811865476 188.130102354156 89.99999999999997 {}
A 7 934.1666666666666 -90 39.58991173406564 210.3432488842396 198.9246444160511 {}
P 1 10 930 -370 920 -430 1000 -410 960 -390 990 -360 970 -360 960 -340 940 -380 930 -360 930 -370 {}
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
T {Select the components here under
and press'<Ctrl>-h' or <Ctrl>-click
them to visit xschem websites
or local docs} 390 -420 0 0 0.3 0.3 {}
T {Lines} 1010 -560 0 0 0.6 0.6 {layer=4}
T {Rectangles} 1010 -500 0 0 0.6 0.6 {layer=4}
T {Polygons} 1010 -410 0 0 0.6 0.6 {layer=4}
T {Electrical
wires} 1010 -330 0 0 0.6 0.6 {layer=4}
T {XSCHEM OBJECTS} 910 -720 0 0 0.6 0.6 {layer=7}
T {Components} 1010 -210 0 0 0.6 0.6 {layer=4}
T {TEXT} 890 -620 0 0 0.5 0.5 {layer=8}
T {TEXT} 890 -670 1 0 0.7 0.7 {layer=6 font=FreeMono}
T {Text} 900 -660 0 0 0.5 0.5 {layer=7 font="Times"}
T {TEXT} 990 -660 1 0 0.5 0.5 {}
T {Text} 1010 -650 0 0 0.6 0.6 {layer=4}
T {Arcs/Circles} 1010 -120 0 0 0.6 0.6 {layer=4}
N 870 -300 990 -300 {lab=#net1}
N 910 -340 910 -250 {lab=#net2}
N 910 -270 970 -270 {lab=#net2}
N 930 -310 960 -300 {lab=#net1}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {launcher.sym} 450 -270 0 0 {name=h1 
descr="XSCHEM ON REPO.HU" 
url="http://repo.hu/projects/xschem"
program=x-www-browser}
C {launcher.sym} 450 -320 0 0 {name=h2 
descr="LOCAL DOCUMENTATION" 
url="$\{XSCHEM_SHAREDIR\}/../doc/xschem/index.html"
program=x-www-browser

}
C {launcher.sym} 450 -220 0 0 {name=h3 
descr="Toggle light/dark 
colorscheme" 
tclcommand="xschem toggle_colorscheme"
}
C {zener.sym} 920 -190 0 0 {name=x10 model=XXX device=XXX}
C {pcb_current_protection.sym} 170 -460 0 0 {name=x1}
C {pcb_voltage_protection.sym} 170 -420 0 0 {name=x0}
C {pcb_test1.sym} 170 -380 0 0 {name=x2}
C {hierarchical_tedax.sym} 170 -340 0 0 {name=x3}
