v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 4 110 -1580 190 -1580 {}
L 4 110 -1480 190 -1480 {}
L 4 110 -1380 190 -1380 {}
L 4 100 -1470 100 -1390 {}
L 4 200 -1470 200 -1390 {}
L 4 100 -1570 100 -1490 {}
L 4 200 -1570 200 -1490 {}
L 4 1710 -1620 1710 -1360 {}
L 4 1710 -1360 1750 -1360 {}
L 4 1750 -1370 1750 -1360 {}
L 4 1750 -1370 1770 -1360 {}
L 4 1750 -1350 1770 -1360 {}
L 4 1750 -1360 1750 -1350 {}
L 4 1710 -1600 1750 -1600 {}
L 4 1750 -1610 1750 -1600 {}
L 4 1750 -1610 1770 -1600 {}
L 4 1750 -1590 1770 -1600 {}
L 4 1750 -1600 1750 -1590 {}
B 12 3920 -2840 4640 -1540 {}
P 5 9 1800 -300 1800 -220 1790 -230 1810 -180 1830 -230 1820 -220 1820 -300 1810 -290 1800 -300 {fill=true}
T {7 Segment Display driver
and base-10 counter} 1540 -3130 0 0 2.5 2.5 {}
T {Cyan: Uninitialized or unknown (X)} 3240 -1200 0 0 1.5 1.5 {layer=1}
T {Signal colors:} 3160 -1300 0 0 1.5 1.5 {}
T {Blue: Logic level low (0)} 3240 -1100 0 0 1.5 1.5 {layer=12}
T {Red: Logic level high (1)} 3240 -1000 0 0 1.5 1.5 {layer=5}
T {
      0 1 2 3 4 5 6 
======================
0000  X X X   X X X 
0001      X     X
0010  X   X X X   X
0011  X   X X   X X
0100    X X X   X
0101  X X   X   X X
0110  X X   X X X X
0111  X   X     X
1000  X X X X X X X
1001  X X X X   X X
-HEX (not implemented)-
1010  X X X X X X  
1011    X   X X X X
1100  X X     X   X
1101      X X X X X
1110  X X   X X   X
1111  X X   X X    
} 40 -1340 0 0 0.4 0.4 {font=Monospace}
T {0} 140 -1630 0 0 0.8 0.8 {}
T {1} 90 -1550 0 1 0.8 0.8 {}
T {2} 210 -1550 0 0 0.8 0.8 {}
T {3} 140 -1530 0 0 0.8 0.8 {}
T {6} 140 -1430 0 0 0.8 0.8 {}
T {4} 90 -1450 0 1 0.8 0.8 {}
T {5} 210 -1450 0 0 0.8 0.8 {}
T {Segment 0} 1190 -2880 0 0 1 1 { layer=4}
T {Segment 1} 1190 -2400 0 0 1 1 { layer=4}
T {Segment 2} 1190 -1920 0 0 1 1 { layer=4}
T {Segment 3} 1190 -1520 0 0 1 1 { layer=4}
T {Segment 4} 1190 -960 0 0 1 1 { layer=4}
T {Segment 5} 1190 -560 0 0 1 1 { layer=4}
T {Segment 6} 1190 -240 0 0 1 1 { layer=4}
T {Truth Table} 10 -1730 0 0 1.5 1.5 {layer=4}
T {This example shows Xschem (very simple)
logic simulation abilities
Ctrl-Click to start simulation.
Xschem will be only partially 
responsive during simulation.
Will run for 100 iterations.
If you need to stop:
Simulation -> Forced stop tcl scripts} 1620 -960 0 0 1.3 1.3 {}
T {select this net and:
- press '0' to set to logic '0' value
- press '1' to set to logic '1' value
- press '2' to set to logic 'X' value
- press '3' to set to logic 'Z' value
- press '4' to set to toggle 1->0, 0->1, X->X} 1600 -1900 0 0 0.7 0.7 {}
T {Brown: High Impedance (Z)} 3240 -900 0 0 1.5 1.5 {layer=13}
N 2340 -2250 2340 -2170 { lab=#net1}
N 2220 -2250 2340 -2250 { lab=#net1}
N 2220 -2270 2220 -2250 { lab=#net1}
N 2340 -2430 2340 -2350 { lab=#net2}
N 2220 -2430 2340 -2430 { lab=#net2}
N 2220 -2450 2220 -2430 { lab=#net2}
N 2340 -2610 2340 -2530 { lab=#net3}
N 2220 -2610 2340 -2610 { lab=#net3}
N 2220 -2630 2220 -2610 { lab=#net3}
N 3630 -2650 3690 -2650 { lab=B[3]}
N 3630 -2750 3630 -2650 { lab=B[3]}
N 2150 -2750 3630 -2750 { lab=B[3]}
N 2150 -2750 2150 -2710 { lab=B[3]}
N 3630 -2470 3690 -2470 { lab=B[2]}
N 3630 -2570 3630 -2470 { lab=B[2]}
N 2150 -2570 3630 -2570 { lab=B[2]}
N 2150 -2570 2150 -2530 { lab=B[2]}
N 3630 -2290 3690 -2290 { lab=B[1]}
N 3630 -2390 3630 -2290 { lab=B[1]}
N 2150 -2390 3630 -2390 { lab=B[1]}
N 2150 -2390 2150 -2350 { lab=B[1]}
N 3630 -2110 3690 -2110 { lab=B[0]}
N 3630 -2210 3630 -2110 { lab=B[0]}
N 2150 -2210 3630 -2210 { lab=B[0]}
N 2150 -2210 2150 -2170 { lab=B[0]}
N 2010 -2180 2010 -2130 { lab=CLEAR_}
N 2010 -2130 2220 -2130 { lab=CLEAR_}
N 2090 -2310 2090 -2090 { lab=#net4}
N 2090 -2310 2220 -2310 { lab=#net4}
N 2090 -2490 2090 -2310 { lab=#net4}
N 2090 -2490 2220 -2490 { lab=#net4}
N 2090 -2670 2090 -2490 { lab=#net4}
N 2090 -2670 2220 -2670 { lab=#net4}
N 2090 -2090 2220 -2090 { lab=#net4}
N 2340 -2710 2360 -2710 { lab=#net5}
N 2150 -2170 2220 -2170 { lab=B[0]}
N 2150 -2350 2220 -2350 { lab=B[1]}
N 2150 -2530 2220 -2530 { lab=B[2]}
N 2150 -2710 2220 -2710 { lab=B[3]}
N 3610 -2650 3630 -2650 { lab=B[3]}
N 3610 -2470 3630 -2470 { lab=B[2]}
N 3610 -2290 3630 -2290 { lab=B[1]}
N 3610 -2110 3630 -2110 { lab=B[0]}
N 2760 -2630 3430 -2630 { lab=#net6}
N 2490 -2650 2640 -2650 { lab=BNEXT[3]}
N 2490 -2650 2490 -2630 { lab=BNEXT[3]}
N 2340 -2630 2490 -2630 { lab=BNEXT[3]}
N 2760 -2450 3430 -2450 { lab=#net7}
N 2490 -2470 2640 -2470 { lab=BNEXT[2]}
N 2490 -2470 2490 -2450 { lab=BNEXT[2]}
N 2340 -2450 2490 -2450 { lab=BNEXT[2]}
N 2760 -2270 3430 -2270 { lab=#net8}
N 2490 -2290 2640 -2290 { lab=BNEXT[1]}
N 2490 -2290 2490 -2270 { lab=BNEXT[1]}
N 2340 -2270 2490 -2270 { lab=BNEXT[1]}
N 2760 -2090 3430 -2090 { lab=#net9}
N 2490 -2110 2640 -2110 { lab=BNEXT[0]}
N 2490 -2110 2490 -2090 { lab=BNEXT[0]}
N 2340 -2090 2490 -2090 { lab=BNEXT[0]}
N 2480 -1960 2580 -1960 { lab=#net10}
N 2580 -2070 2580 -1960 { lab=#net10}
N 2580 -2610 2640 -2610 { lab=#net10}
N 2580 -2430 2640 -2430 { lab=#net10}
N 2580 -2250 2640 -2250 { lab=#net10}
N 2580 -2070 2640 -2070 { lab=#net10}
N 1910 -1600 3260 -1600 { lab=CLK}
N 2290 -1920 2320 -1920 { lab=BNEXT[3]}
N 2290 -1960 2320 -1960 { lab=BNEXT[1]}
N 2290 -2000 2320 -2000 { lab=BNEXT[2]}
N 3260 -2650 3430 -2650 { lab=CLK}
N 3260 -2650 3260 -1600 { lab=CLK}
N 2580 -2610 2580 -2430 { lab=#net10}
N 2580 -2430 2580 -2250 { lab=#net10}
N 2580 -2250 2580 -2070 { lab=#net10}
N 520 -3140 520 -10 { lab=B[3]}
N 520 -3140 550 -3140 { lab=B[3]}
N 600 -3140 630 -3140 { lab=B[2]}
N 680 -3140 710 -3140 { lab=B[1]}
N 760 -3140 790 -3140 { lab=B[0]}
N 550 -640 880 -640 { lab=#net11}
N 630 -600 880 -600 { lab=#net12}
N 680 -560 880 -560 { lab=B[1]}
N 790 -520 880 -520 { lab=#net13}
N 1000 -580 1230 -580 { lab=S[5]}
N 520 -3160 520 -3140 { lab=B[3]}
N 600 -3160 600 -3140 { lab=B[2]}
N 680 -3160 680 -3140 { lab=B[1]}
N 760 -3160 760 -3140 { lab=B[0]}
N 550 -3040 880 -3040 { lab=#net11}
N 630 -3000 880 -3000 { lab=#net12}
N 710 -2960 880 -2960 { lab=#net14}
N 760 -2920 880 -2920 { lab=B[0]}
N 550 -2880 880 -2880 { lab=#net11}
N 600 -2840 880 -2840 { lab=B[2]}
N 710 -2800 880 -2800 { lab=#net14}
N 790 -2760 880 -2760 { lab=#net13}
N 1020 -2920 1050 -2920 { lab=#net15}
N 1020 -2980 1020 -2920 { lab=#net15}
N 1000 -2980 1020 -2980 { lab=#net15}
N 1020 -2880 1050 -2880 { lab=#net16}
N 1020 -2880 1020 -2820 { lab=#net16}
N 1000 -2820 1020 -2820 { lab=#net16}
N 1170 -2900 1230 -2900 { lab=S[0]}
N 550 -2560 880 -2560 { lab=#net11}
N 630 -2520 880 -2520 { lab=#net12}
N 680 -2480 880 -2480 { lab=B[1]}
N 790 -2440 880 -2440 { lab=#net13}
N 550 -2400 880 -2400 { lab=#net11}
N 630 -2360 880 -2360 { lab=#net12}
N 680 -2320 880 -2320 { lab=B[1]}
N 760 -2280 880 -2280 { lab=B[0]}
N 550 -2240 880 -2240 { lab=#net11}
N 600 -2200 880 -2200 { lab=B[2]}
N 680 -2160 880 -2160 { lab=B[1]}
N 760 -2120 880 -2120 { lab=B[0]}
N 550 -2720 880 -2720 { lab=#net11}
N 630 -2680 880 -2680 { lab=#net12}
N 710 -2640 880 -2640 { lab=#net14}
N 760 -2600 880 -2600 { lab=B[0]}
N 1000 -2660 1050 -2660 { lab=#net17}
N 1000 -2500 1020 -2500 { lab=#net18}
N 1000 -2340 1020 -2340 { lab=#net19}
N 1000 -2180 1050 -2180 { lab=#net20}
N 1050 -2660 1050 -2480 { lab=#net17}
N 1020 -2500 1020 -2440 { lab=#net18}
N 1020 -2440 1050 -2440 { lab=#net18}
N 1020 -2400 1050 -2400 { lab=#net19}
N 1020 -2400 1020 -2340 { lab=#net19}
N 1050 -2360 1050 -2180 { lab=#net20}
N 1170 -2420 1230 -2420 { lab=S[1]}
N 550 -2080 880 -2080 { lab=#net11}
N 600 -2040 880 -2040 { lab=B[2]}
N 710 -2000 880 -2000 { lab=#net14}
N 760 -1960 880 -1960 { lab=B[0]}
N 550 -1920 880 -1920 { lab=#net11}
N 600 -1880 880 -1880 { lab=B[2]}
N 680 -1840 880 -1840 { lab=B[1]}
N 790 -1800 880 -1800 { lab=#net13}
N 1020 -1960 1050 -1960 { lab=#net21}
N 1020 -2020 1020 -1960 { lab=#net21}
N 1000 -2020 1020 -2020 { lab=#net21}
N 1020 -1920 1050 -1920 { lab=#net22}
N 1020 -1920 1020 -1860 { lab=#net22}
N 1000 -1860 1020 -1860 { lab=#net22}
N 1170 -1940 1230 -1940 { lab=S[2]}
N 550 -1760 880 -1760 { lab=#net11}
N 630 -1720 880 -1720 { lab=#net12}
N 710 -1680 880 -1680 { lab=#net14}
N 790 -1640 880 -1640 { lab=#net13}
N 1000 -1700 1050 -1700 { lab=#net23}
N 550 -1600 880 -1600 { lab=#net11}
N 630 -1560 880 -1560 { lab=#net12}
N 710 -1520 880 -1520 { lab=#net14}
N 1000 -1540 1050 -1540 { lab=#net24}
N 550 -1440 880 -1440 { lab=#net11}
N 600 -1400 880 -1400 { lab=B[2]}
N 680 -1360 880 -1360 { lab=B[1]}
N 760 -1320 880 -1320 { lab=B[0]}
N 1000 -1380 1050 -1380 { lab=#net25}
N 1050 -1700 1050 -1580 { lab=#net23}
N 1050 -1500 1050 -1380 { lab=#net25}
N 550 -3060 550 -10 { lab=#net11}
N 630 -3060 630 -10 { lab=#net12}
N 710 -3060 710 -10 { lab=#net14}
N 760 -3140 760 -10 { lab=B[0]}
N 600 -3140 600 -10 { lab=B[2]}
N 790 -3060 790 -10 { lab=#net13}
N 680 -3140 680 -10 { lab=B[1]}
N 760 -1480 880 -1480 { lab=B[0]}
N 1170 -1540 1230 -1540 { lab=S[3]}
N 550 -1280 880 -1280 { lab=#net11}
N 630 -1240 880 -1240 { lab=#net12}
N 710 -1200 880 -1200 { lab=#net14}
N 790 -1160 880 -1160 { lab=#net13}
N 1000 -1220 1050 -1220 { lab=#net26}
N 550 -1120 880 -1120 { lab=#net11}
N 630 -1080 880 -1080 { lab=#net12}
N 680 -1040 880 -1040 { lab=B[1]}
N 790 -1000 880 -1000 { lab=#net13}
N 1000 -1060 1020 -1060 { lab=#net27}
N 550 -960 880 -960 { lab=#net11}
N 600 -920 880 -920 { lab=B[2]}
N 680 -880 880 -880 { lab=B[1]}
N 790 -840 880 -840 { lab=#net13}
N 1000 -900 1020 -900 { lab=#net28}
N 520 -800 880 -800 { lab=B[3]}
N 630 -760 880 -760 { lab=#net12}
N 710 -720 880 -720 { lab=#net14}
N 790 -680 880 -680 { lab=#net13}
N 1000 -740 1050 -740 { lab=#net29}
N 1050 -920 1050 -740 { lab=#net29}
N 1050 -1220 1050 -1040 { lab=#net26}
N 1020 -1000 1050 -1000 { lab=#net27}
N 1020 -1060 1020 -1000 { lab=#net27}
N 1020 -960 1050 -960 { lab=#net28}
N 1020 -960 1020 -900 { lab=#net28}
N 1170 -980 1230 -980 { lab=S[4]}
N 550 -480 880 -480 { lab=#net11}
N 630 -440 880 -440 { lab=#net12}
N 710 -400 880 -400 { lab=#net14}
N 760 -360 880 -360 { lab=B[0]}
N 1000 -420 1050 -420 { lab=#net30}
N 550 -320 880 -320 { lab=#net11}
N 600 -280 880 -280 { lab=B[2]}
N 710 -240 880 -240 { lab=#net14}
N 1000 -260 1050 -260 { lab=#net31}
N 550 -160 880 -160 { lab=#net11}
N 600 -120 880 -120 { lab=B[2]}
N 680 -80 880 -80 { lab=B[1]}
N 760 -40 880 -40 { lab=B[0]}
N 1000 -100 1050 -100 { lab=#net32}
N 1050 -420 1050 -300 { lab=#net30}
N 1050 -220 1050 -100 { lab=#net32}
N 790 -200 880 -200 { lab=#net13}
N 1170 -260 1230 -260 { lab=S[6]}
N 3260 -2470 3430 -2470 { lab=CLK}
N 3260 -2290 3430 -2290 { lab=CLK}
N 3260 -2110 3430 -2110 { lab=CLK}
N 1900 -1360 3320 -1360 { lab=CLEAR_}
N 3320 -2610 3430 -2610 { lab=CLEAR_}
N 3320 -2610 3320 -1360 { lab=CLEAR_}
N 3320 -2430 3430 -2430 { lab=CLEAR_}
N 3320 -2250 3430 -2250 { lab=CLEAR_}
N 3320 -2070 3430 -2070 { lab=CLEAR_}
C {title.sym} 1350 -50 0 0 {name=l38 author="Stefan Schippers"}
C {fa_1.sym} 2280 -2670 0 0 {name=x28 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {fa_1.sym} 2280 -2490 0 0 {name=x33 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {fa_1.sym} 2280 -2310 0 0 {name=x34 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {fa_1.sym} 2280 -2130 0 0 {name=x35 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {dfrtp_1.sym} 3520 -2630 0 0 {name=x36 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 3690 -2650 0 1 {name=l34 sig_type=std_logic lab=B[3]}
C {dfrtp_1.sym} 3520 -2450 0 0 {name=x37 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 3690 -2470 0 1 {name=l37 sig_type=std_logic lab=B[2]}
C {dfrtp_1.sym} 3520 -2270 0 0 {name=x53 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 3690 -2290 0 1 {name=l41 sig_type=std_logic lab=B[1]}
C {dfrtp_1.sym} 3520 -2090 0 0 {name=x54 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 3690 -2110 0 1 {name=l44 sig_type=std_logic lab=B[0]}
C {inv_2.sym} 2050 -2180 0 0 {name=x55 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 2010 -2180 0 0 {name=l45 sig_type=std_logic lab=CLEAR_}
C {noconn.sym} 2360 -2710 0 1 {name=l46}
C {and2_1.sym} 2700 -2630 0 0 {name=x59 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and2_1.sym} 2700 -2450 0 0 {name=x60 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and2_1.sym} 2700 -2270 0 0 {name=x61 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and2_1.sym} 2700 -2090 0 0 {name=x62 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 2490 -2650 0 0 {name=l51 sig_type=std_logic lab=BNEXT[3]}
C {lab_pin.sym} 2490 -2470 0 0 {name=l52 sig_type=std_logic lab=BNEXT[2]}
C {lab_pin.sym} 2490 -2290 0 0 {name=l53 sig_type=std_logic lab=BNEXT[1]}
C {lab_pin.sym} 2490 -2110 0 0 {name=l54 sig_type=std_logic lab=BNEXT[0]}
C {launcher.sym} 1840 -130 0 0 {name=h3
descr="START SIMULATION" 
comment="
  This launcher Starts a simple interactive simulation of the LFSR
"
tclcommand="  set tclstop 0  ;# clear stop flag
  set count 0
  set duration 1000
  xschem select instance p8  ;# CLEAR_
  xschem select instance p9  ;# CLK
  xschem logic_set 0         ;# reset pulse (active low) 
  update ;# allow event loop to come in (update screen, user input etc)
  after $duration
  xschem select instance p9 clear ;# release CLK
  xschem logic_set 1
  xschem select instance p8 clear ;# release CLEAR_
  xschem select instance p9  ;# CLK
  update ;# allow event loop to come in (update screen, user input etc) 
  set logic_value 0
  while \{1\} \{
    update ;# allow event loop to come in (update screen, user input etc) 
    incr count
    if \{$count>100 || $tclstop == 1\} break
    puts \\"simulating: loop $count\\"
    xschem logic_set $logic_value
    after $duration
    set logic_value [expr !$logic_value]
    set duration [expr int($duration/1.05)]
  \}
"
}
C {inv_2.sym} 2620 -1960 0 0 {name=x1 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {giant_label.sym} 1910 -1600 0 1 {name=l1 sig_type=std_logic lab=CLK text=CLK}
C {giant_label.sym} 2660 -1960 0 1 {name=l3 sig_type=std_logic lab=R text="RESET
COUNTER"}
C {giant_label.sym} 1900 -1360 0 1 {name=l4 sig_type=std_logic lab=CLEAR_ text="CLEAR (active low)"}
C {segment.sym} 4560 -2230 0 0 {name=l6 sig_type=std_logic lab=S[3]}
C {segment.sym} 4030 -2200 1 0 {name=l2 sig_type=std_logic lab=S[1]}
C {segment.sym} 4530 -2200 3 1 {name=l5 sig_type=std_logic lab=S[2]}
C {segment.sym} 4560 -2810 0 0 {name=l7 sig_type=std_logic lab=S[0]}
C {segment.sym} 4530 -2180 3 0 {name=l8 sig_type=std_logic lab=S[5]}
C {segment.sym} 4030 -2180 1 1 {name=l9 sig_type=std_logic lab=S[4]}
C {segment.sym} 4000 -1570 2 0 {name=l10 sig_type=std_logic lab=S[6]}
C {ipin.sym} 1900 -1360 0 0 {name=p8 sig_type=std_logic lab=CLEAR_}
C {ipin.sym} 1910 -1600 0 0 {name=p9 sig_type=std_logic lab=CLK}
C {o21ai_1.sym} 2400 -1960 0 0 {name=x2 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 2290 -1920 0 0 {name=l11 sig_type=std_logic lab=BNEXT[3]}
C {lab_pin.sym} 2290 -2000 0 0 {name=l12 sig_type=std_logic lab=BNEXT[2]}
C {lab_pin.sym} 2290 -1960 0 0 {name=l13 sig_type=std_logic lab=BNEXT[1]}
C {inv_2.sym} 550 -3100 3 1 {name=x22 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 520 -3160 0 0 {name=l19 sig_type=std_logic lab=B[3]}
C {inv_2.sym} 630 -3100 3 1 {name=x23 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 600 -3160 0 0 {name=l21 sig_type=std_logic lab=B[2]}
C {inv_2.sym} 710 -3100 3 1 {name=x24 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 680 -3160 0 0 {name=l22 sig_type=std_logic lab=B[1]}
C {inv_2.sym} 790 -3100 3 1 {name=x25 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 760 -3160 0 0 {name=l23 sig_type=std_logic lab=B[0]}
C {nand4_1.sym} 940 -580 0 0 {name=x21 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and4_1.sym} 1110 -2420 0 0 {name=x38 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -2980 0 0 {name=x39 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -2820 0 0 {name=x40 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and2_1.sym} 1110 -2900 0 0 {name=x41 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 1230 -2900 0 1 {name=p4 lab=S[0]}
C {nand4_1.sym} 940 -2500 0 0 {name=x42 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -2340 0 0 {name=x43 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -2180 0 0 {name=x44 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -2660 0 0 {name=x45 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 1230 -2420 0 1 {name=p5 lab=S[1]}
C {nand4_1.sym} 940 -2020 0 0 {name=x46 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -1860 0 0 {name=x47 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and2_1.sym} 1110 -1940 0 0 {name=x48 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 1230 -1940 0 1 {name=p6 lab=S[2]}
C {nand4_1.sym} 940 -1700 0 0 {name=x49 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -1540 0 0 {name=x50 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -1380 0 0 {name=x51 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and3_1.sym} 1110 -1540 0 0 {name=x52 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 1230 -1540 0 1 {name=p1 lab=S[3]}
C {and4_1.sym} 940 -1220 0 0 {name=x18 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and4_1.sym} 940 -1060 0 0 {name=x19 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and4_1.sym} 940 -900 0 0 {name=x20 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and4_1.sym} 940 -740 0 0 {name=x26 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {or4_1.sym} 1110 -980 0 0 {name=x27 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 1230 -980 0 1 {name=p2 lab=S[4]}
C {lab_pin.sym} 1230 -580 0 1 {name=p3 lab=S[5]}
C {nand4_1.sym} 940 -420 0 0 {name=x29 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -260 0 0 {name=x30 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand4_1.sym} 940 -100 0 0 {name=x31 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {and3_1.sym} 1110 -260 0 0 {name=x32 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 1230 -260 0 1 {name=p7 lab=S[6]}
