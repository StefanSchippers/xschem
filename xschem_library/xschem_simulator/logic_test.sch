v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
B 1 160 -3040 6800 -2320 {}
P 4 7 920 -960 110 -960 110 -1040 100 -1040 110 -1080 120 -1040 110 -1040 {}
P 4 9 1320 -640 920 -640 920 -460 900 -460 920 -420 940 -460 920 -460 920 -640 1320 -640 {}
T {This example shows Xschem (very simple) simulation abilities
Ctrl-Click here to start a simulation.
Xschem will be only partially responsive during simulation.
Simulation will terminate automatically after 30 iterations
Otherwise go to Simulation ->Forced stop tcl scripts} 1340 -740 0 0 2 2 {}
T {Clicking an input net and pressing '0', '1', '2' or '3' will set the net to logic state 0,1,X,Z respectively} 960 -1030 0 0 2 2 {}
T {9 Flip Flop Linear Feedback Shift Register} 1980 -3440 0 0 3 3 {}
T {Cyan: Uninitialized or unknown (X)} 4930 -540 0 0 1.5 1.5 {layer=1}
T {Signal colors:} 4850 -640 0 0 1.5 1.5 {}
T {Blue: Logic level low (0)} 4930 -440 0 0 1.5 1.5 {layer=12}
T {Red: Logic level high (1)} 4930 -340 0 0 1.5 1.5 {layer=5}
N 480 -1900 480 -1870 { lab=#net1}
N 360 -1970 480 -1900 { lab=#net1}
N 360 -2000 360 -1970 { lab=#net1}
N 480 -2020 480 -1990 { lab=#net2}
N 360 -1920 480 -1990 { lab=#net2}
N 360 -1920 360 -1910 { lab=#net2}
N 530 -1870 560 -1870 { lab=#net1}
N 300 -2040 360 -2040 { lab=#net3}
N 480 -1380 480 -1350 { lab=#net3}
N 360 -1450 480 -1380 { lab=#net3}
N 360 -1460 360 -1450 { lab=#net3}
N 480 -1500 480 -1470 { lab=#net4}
N 360 -1400 480 -1470 { lab=#net4}
N 480 -1500 560 -1500 { lab=#net4}
N 250 -1500 360 -1500 { lab=CLK}
N 710 -1640 710 -1610 { lab=#net5}
N 590 -1710 710 -1640 { lab=#net5}
N 590 -1740 590 -1710 { lab=#net5}
N 710 -1760 710 -1730 { lab=q1}
N 590 -1660 710 -1730 { lab=q1}
N 590 -1660 590 -1650 { lab=q1}
N 710 -1760 770 -1760 { lab=q1}
N 250 -1870 360 -1870 { lab=CLK}
N 250 -1870 250 -1500 { lab=CLK}
N 300 -2040 300 -1460 { lab=#net3}
N 300 -1460 360 -1460 { lab=#net3}
N 340 -1540 360 -1540 { lab=#net1}
N 340 -1600 340 -1540 { lab=#net1}
N 340 -1600 530 -1730 { lab=#net1}
N 530 -1870 530 -1730 { lab=#net1}
N 560 -1780 590 -1780 { lab=#net1}
N 560 -1870 560 -1780 { lab=#net1}
N 560 -1610 590 -1610 { lab=#net4}
N 560 -1610 560 -1500 { lab=#net4}
N 590 -1570 590 -1230 { lab=CLEAR_}
N 180 -1310 360 -1310 { lab=CLEAR_}
N 180 -1310 180 -1230 { lab=CLEAR_}
N 180 -1830 180 -1310 { lab=CLEAR_}
N 180 -1830 360 -1830 { lab=CLEAR_}
N 480 -1870 530 -1870 { lab=#net1}
N 120 -1350 360 -1350 { lab=#net6}
N 110 -1230 180 -1230 { lab=CLEAR_}
N 6630 -1230 6840 -1230 { lab=CLEAR_}
N 180 -1230 590 -1230 { lab=CLEAR_}
N 6290 -1110 6840 -1110 { lab=CLK}
N 250 -1500 250 -1110 { lab=CLK}
N 110 -1110 250 -1110 { lab=CLK}
N 1140 -1900 1140 -1870 { lab=#net7}
N 1020 -1970 1140 -1900 { lab=#net7}
N 1020 -2000 1020 -1970 { lab=#net7}
N 1140 -2020 1140 -1990 { lab=#net8}
N 1020 -1920 1140 -1990 { lab=#net8}
N 1020 -1920 1020 -1910 { lab=#net8}
N 1190 -1870 1220 -1870 { lab=#net7}
N 960 -2040 1020 -2040 { lab=#net9}
N 1140 -1380 1140 -1350 { lab=#net9}
N 1020 -1450 1140 -1380 { lab=#net9}
N 1020 -1460 1020 -1450 { lab=#net9}
N 1140 -1500 1140 -1470 { lab=#net10}
N 1020 -1400 1140 -1470 { lab=#net10}
N 1140 -1500 1220 -1500 { lab=#net10}
N 910 -1500 1020 -1500 { lab=CLK}
N 1370 -1640 1370 -1610 { lab=#net11}
N 1250 -1710 1370 -1640 { lab=#net11}
N 1250 -1740 1250 -1710 { lab=#net11}
N 1370 -1760 1370 -1730 { lab=q2}
N 1250 -1660 1370 -1730 { lab=q2}
N 1250 -1660 1250 -1650 { lab=q2}
N 1370 -1760 1430 -1760 { lab=q2}
N 910 -1870 1020 -1870 { lab=CLK}
N 910 -1870 910 -1500 { lab=CLK}
N 960 -2040 960 -1460 { lab=#net9}
N 960 -1460 1020 -1460 { lab=#net9}
N 1000 -1540 1020 -1540 { lab=#net7}
N 1000 -1600 1000 -1540 { lab=#net7}
N 1000 -1600 1190 -1730 { lab=#net7}
N 1190 -1870 1190 -1730 { lab=#net7}
N 1220 -1780 1250 -1780 { lab=#net7}
N 1220 -1870 1220 -1780 { lab=#net7}
N 1220 -1610 1250 -1610 { lab=#net10}
N 1220 -1610 1220 -1500 { lab=#net10}
N 1250 -1570 1250 -1230 { lab=CLEAR_}
N 840 -1310 1020 -1310 { lab=CLEAR_}
N 840 -1310 840 -1230 { lab=CLEAR_}
N 840 -1830 840 -1310 { lab=CLEAR_}
N 840 -1830 1020 -1830 { lab=CLEAR_}
N 1140 -1870 1190 -1870 { lab=#net7}
N 840 -1230 1250 -1230 { lab=CLEAR_}
N 910 -1500 910 -1110 { lab=CLK}
N 770 -1760 770 -1350 { lab=q1}
N 770 -1350 1020 -1350 { lab=q1}
N 590 -1230 840 -1230 { lab=CLEAR_}
N 250 -1110 910 -1110 { lab=CLK}
N 1800 -1900 1800 -1870 { lab=#net12}
N 1680 -1970 1800 -1900 { lab=#net12}
N 1680 -2000 1680 -1970 { lab=#net12}
N 1800 -2020 1800 -1990 { lab=#net13}
N 1680 -1920 1800 -1990 { lab=#net13}
N 1680 -1920 1680 -1910 { lab=#net13}
N 1850 -1870 1880 -1870 { lab=#net12}
N 1620 -2040 1680 -2040 { lab=#net14}
N 1800 -1380 1800 -1350 { lab=#net14}
N 1680 -1450 1800 -1380 { lab=#net14}
N 1680 -1460 1680 -1450 { lab=#net14}
N 1800 -1500 1800 -1470 { lab=#net15}
N 1680 -1400 1800 -1470 { lab=#net15}
N 1800 -1500 1880 -1500 { lab=#net15}
N 1570 -1500 1680 -1500 { lab=CLK}
N 2030 -1640 2030 -1610 { lab=#net16}
N 1910 -1710 2030 -1640 { lab=#net16}
N 1910 -1740 1910 -1710 { lab=#net16}
N 2030 -1760 2030 -1730 { lab=q3}
N 1910 -1660 2030 -1730 { lab=q3}
N 1910 -1660 1910 -1650 { lab=q3}
N 2030 -1760 2090 -1760 { lab=q3}
N 1570 -1870 1680 -1870 { lab=CLK}
N 1570 -1870 1570 -1500 { lab=CLK}
N 1620 -2040 1620 -1460 { lab=#net14}
N 1620 -1460 1680 -1460 { lab=#net14}
N 1660 -1540 1680 -1540 { lab=#net12}
N 1660 -1600 1660 -1540 { lab=#net12}
N 1660 -1600 1850 -1730 { lab=#net12}
N 1850 -1870 1850 -1730 { lab=#net12}
N 1880 -1780 1910 -1780 { lab=#net12}
N 1880 -1870 1880 -1780 { lab=#net12}
N 1880 -1610 1910 -1610 { lab=#net15}
N 1880 -1610 1880 -1500 { lab=#net15}
N 1910 -1570 1910 -1230 { lab=CLEAR_}
N 1500 -1310 1680 -1310 { lab=CLEAR_}
N 1500 -1310 1500 -1230 { lab=CLEAR_}
N 1500 -1830 1500 -1310 { lab=CLEAR_}
N 1500 -1830 1680 -1830 { lab=CLEAR_}
N 1800 -1870 1850 -1870 { lab=#net12}
N 1500 -1230 1910 -1230 { lab=CLEAR_}
N 1570 -1500 1570 -1110 { lab=CLK}
N 2460 -1900 2460 -1870 { lab=#net17}
N 2340 -1970 2460 -1900 { lab=#net17}
N 2340 -2000 2340 -1970 { lab=#net17}
N 2460 -2020 2460 -1990 { lab=#net18}
N 2340 -1920 2460 -1990 { lab=#net18}
N 2340 -1920 2340 -1910 { lab=#net18}
N 2510 -1870 2540 -1870 { lab=#net17}
N 2280 -2040 2340 -2040 { lab=#net19}
N 2460 -1380 2460 -1350 { lab=#net19}
N 2340 -1450 2460 -1380 { lab=#net19}
N 2340 -1460 2340 -1450 { lab=#net19}
N 2460 -1500 2460 -1470 { lab=#net20}
N 2340 -1400 2460 -1470 { lab=#net20}
N 2460 -1500 2540 -1500 { lab=#net20}
N 2230 -1500 2340 -1500 { lab=CLK}
N 2690 -1640 2690 -1610 { lab=#net21}
N 2570 -1710 2690 -1640 { lab=#net21}
N 2570 -1740 2570 -1710 { lab=#net21}
N 2690 -1760 2690 -1730 { lab=q4}
N 2570 -1660 2690 -1730 { lab=q4}
N 2570 -1660 2570 -1650 { lab=q4}
N 2230 -1870 2340 -1870 { lab=CLK}
N 2230 -1870 2230 -1500 { lab=CLK}
N 2280 -2040 2280 -1460 { lab=#net19}
N 2280 -1460 2340 -1460 { lab=#net19}
N 2320 -1540 2340 -1540 { lab=#net17}
N 2320 -1600 2320 -1540 { lab=#net17}
N 2320 -1600 2510 -1730 { lab=#net17}
N 2510 -1870 2510 -1730 { lab=#net17}
N 2540 -1780 2570 -1780 { lab=#net17}
N 2540 -1870 2540 -1780 { lab=#net17}
N 2540 -1610 2570 -1610 { lab=#net20}
N 2540 -1610 2540 -1500 { lab=#net20}
N 2570 -1570 2570 -1230 { lab=CLEAR_}
N 2160 -1310 2340 -1310 { lab=CLEAR_}
N 2160 -1310 2160 -1230 { lab=CLEAR_}
N 2160 -1830 2160 -1310 { lab=CLEAR_}
N 2160 -1830 2340 -1830 { lab=CLEAR_}
N 2460 -1870 2510 -1870 { lab=#net17}
N 2160 -1230 2570 -1230 { lab=CLEAR_}
N 2230 -1500 2230 -1110 { lab=CLK}
N 3120 -1900 3120 -1870 { lab=#net22}
N 3000 -1970 3120 -1900 { lab=#net22}
N 3000 -2000 3000 -1970 { lab=#net22}
N 3120 -2020 3120 -1990 { lab=#net23}
N 3000 -1920 3120 -1990 { lab=#net23}
N 3000 -1920 3000 -1910 { lab=#net23}
N 3170 -1870 3200 -1870 { lab=#net22}
N 2940 -2040 3000 -2040 { lab=#net24}
N 3120 -1380 3120 -1350 { lab=#net24}
N 3000 -1450 3120 -1380 { lab=#net24}
N 3000 -1460 3000 -1450 { lab=#net24}
N 3120 -1500 3120 -1470 { lab=#net25}
N 3000 -1400 3120 -1470 { lab=#net25}
N 3120 -1500 3200 -1500 { lab=#net25}
N 2890 -1500 3000 -1500 { lab=CLK}
N 3350 -1640 3350 -1610 { lab=#net26}
N 3230 -1710 3350 -1640 { lab=#net26}
N 3230 -1740 3230 -1710 { lab=#net26}
N 3350 -1760 3350 -1730 { lab=q5}
N 3230 -1660 3350 -1730 { lab=q5}
N 3230 -1660 3230 -1650 { lab=q5}
N 3350 -1760 4130 -1760 { lab=q5}
N 2890 -1870 3000 -1870 { lab=CLK}
N 2890 -1870 2890 -1500 { lab=CLK}
N 2940 -2040 2940 -1460 { lab=#net24}
N 2940 -1460 3000 -1460 { lab=#net24}
N 2980 -1540 3000 -1540 { lab=#net22}
N 2980 -1600 2980 -1540 { lab=#net22}
N 2980 -1600 3170 -1730 { lab=#net22}
N 3170 -1870 3170 -1730 { lab=#net22}
N 3200 -1780 3230 -1780 { lab=#net22}
N 3200 -1870 3200 -1780 { lab=#net22}
N 3200 -1610 3230 -1610 { lab=#net25}
N 3200 -1610 3200 -1500 { lab=#net25}
N 3230 -1570 3230 -1230 { lab=CLEAR_}
N 2820 -1310 3000 -1310 { lab=CLEAR_}
N 2820 -1310 2820 -1230 { lab=CLEAR_}
N 2820 -1830 2820 -1310 { lab=CLEAR_}
N 2820 -1830 3000 -1830 { lab=CLEAR_}
N 3120 -1870 3170 -1870 { lab=#net22}
N 2820 -1230 3230 -1230 { lab=CLEAR_}
N 2890 -1500 2890 -1110 { lab=CLK}
N 4500 -1900 4500 -1870 { lab=#net27}
N 4380 -1970 4500 -1900 { lab=#net27}
N 4380 -2000 4380 -1970 { lab=#net27}
N 4500 -2020 4500 -1990 { lab=#net28}
N 4380 -1920 4500 -1990 { lab=#net28}
N 4380 -1920 4380 -1910 { lab=#net28}
N 4550 -1870 4580 -1870 { lab=#net27}
N 4320 -2040 4380 -2040 { lab=#net29}
N 4500 -1380 4500 -1350 { lab=#net29}
N 4380 -1450 4500 -1380 { lab=#net29}
N 4380 -1460 4380 -1450 { lab=#net29}
N 4500 -1500 4500 -1470 { lab=#net30}
N 4380 -1400 4500 -1470 { lab=#net30}
N 4500 -1500 4580 -1500 { lab=#net30}
N 4270 -1500 4380 -1500 { lab=CLK}
N 4730 -1640 4730 -1610 { lab=#net31}
N 4610 -1710 4730 -1640 { lab=#net31}
N 4610 -1740 4610 -1710 { lab=#net31}
N 4730 -1760 4730 -1730 { lab=q6}
N 4610 -1660 4730 -1730 { lab=q6}
N 4610 -1660 4610 -1650 { lab=q6}
N 4730 -1760 4800 -1760 { lab=q6}
N 4270 -1870 4380 -1870 { lab=CLK}
N 4270 -1870 4270 -1500 { lab=CLK}
N 4320 -2040 4320 -1460 { lab=#net29}
N 4320 -1460 4380 -1460 { lab=#net29}
N 4360 -1540 4380 -1540 { lab=#net27}
N 4360 -1600 4360 -1540 { lab=#net27}
N 4360 -1600 4550 -1730 { lab=#net27}
N 4550 -1870 4550 -1730 { lab=#net27}
N 4580 -1780 4610 -1780 { lab=#net27}
N 4580 -1870 4580 -1780 { lab=#net27}
N 4580 -1610 4610 -1610 { lab=#net30}
N 4580 -1610 4580 -1500 { lab=#net30}
N 4610 -1570 4610 -1230 { lab=CLEAR_}
N 4200 -1310 4380 -1310 { lab=CLEAR_}
N 4200 -1310 4200 -1230 { lab=CLEAR_}
N 4200 -1830 4200 -1310 { lab=CLEAR_}
N 4200 -1830 4380 -1830 { lab=CLEAR_}
N 4500 -1870 4550 -1870 { lab=#net27}
N 4200 -1230 4610 -1230 { lab=CLEAR_}
N 4270 -1500 4270 -1110 { lab=CLK}
N 4130 -1760 4130 -1350 { lab=q5}
N 4130 -1350 4380 -1350 { lab=q5}
N 5170 -1900 5170 -1870 { lab=#net32}
N 5050 -1970 5170 -1900 { lab=#net32}
N 5050 -2000 5050 -1970 { lab=#net32}
N 5170 -2020 5170 -1990 { lab=#net33}
N 5050 -1920 5170 -1990 { lab=#net33}
N 5050 -1920 5050 -1910 { lab=#net33}
N 5220 -1870 5250 -1870 { lab=#net32}
N 4990 -2040 5050 -2040 { lab=#net34}
N 5170 -1380 5170 -1350 { lab=#net34}
N 5050 -1450 5170 -1380 { lab=#net34}
N 5050 -1460 5050 -1450 { lab=#net34}
N 5170 -1500 5170 -1470 { lab=#net35}
N 5050 -1400 5170 -1470 { lab=#net35}
N 5170 -1500 5250 -1500 { lab=#net35}
N 4940 -1500 5050 -1500 { lab=CLK}
N 5400 -1640 5400 -1610 { lab=#net36}
N 5280 -1710 5400 -1640 { lab=#net36}
N 5280 -1740 5280 -1710 { lab=#net36}
N 5400 -1760 5400 -1730 { lab=q7}
N 5280 -1660 5400 -1730 { lab=q7}
N 5280 -1660 5280 -1650 { lab=q7}
N 5400 -1760 5470 -1760 { lab=q7}
N 4940 -1870 5050 -1870 { lab=CLK}
N 4940 -1870 4940 -1500 { lab=CLK}
N 4990 -2040 4990 -1460 { lab=#net34}
N 4990 -1460 5050 -1460 { lab=#net34}
N 5030 -1540 5050 -1540 { lab=#net32}
N 5030 -1600 5030 -1540 { lab=#net32}
N 5030 -1600 5220 -1730 { lab=#net32}
N 5220 -1870 5220 -1730 { lab=#net32}
N 5250 -1780 5280 -1780 { lab=#net32}
N 5250 -1870 5250 -1780 { lab=#net32}
N 5250 -1610 5280 -1610 { lab=#net35}
N 5250 -1610 5250 -1500 { lab=#net35}
N 5280 -1570 5280 -1230 { lab=CLEAR_}
N 4870 -1310 5050 -1310 { lab=CLEAR_}
N 4870 -1310 4870 -1230 { lab=CLEAR_}
N 4870 -1830 4870 -1310 { lab=CLEAR_}
N 4870 -1830 5050 -1830 { lab=CLEAR_}
N 5170 -1870 5220 -1870 { lab=#net32}
N 4870 -1230 5280 -1230 { lab=CLEAR_}
N 4940 -1500 4940 -1110 { lab=CLK}
N 5840 -1900 5840 -1870 { lab=#net37}
N 5720 -1970 5840 -1900 { lab=#net37}
N 5720 -2000 5720 -1970 { lab=#net37}
N 5840 -2020 5840 -1990 { lab=#net38}
N 5720 -1920 5840 -1990 { lab=#net38}
N 5720 -1920 5720 -1910 { lab=#net38}
N 5890 -1870 5920 -1870 { lab=#net37}
N 5660 -2040 5720 -2040 { lab=#net39}
N 5840 -1380 5840 -1350 { lab=#net39}
N 5720 -1450 5840 -1380 { lab=#net39}
N 5720 -1460 5720 -1450 { lab=#net39}
N 5840 -1500 5840 -1470 { lab=#net40}
N 5720 -1400 5840 -1470 { lab=#net40}
N 5840 -1500 5920 -1500 { lab=#net40}
N 5610 -1500 5720 -1500 { lab=CLK}
N 6070 -1640 6070 -1610 { lab=#net41}
N 5950 -1710 6070 -1640 { lab=#net41}
N 5950 -1740 5950 -1710 { lab=#net41}
N 6070 -1760 6070 -1730 { lab=q8}
N 5950 -1660 6070 -1730 { lab=q8}
N 5950 -1660 5950 -1650 { lab=q8}
N 5610 -1870 5720 -1870 { lab=CLK}
N 5610 -1870 5610 -1500 { lab=CLK}
N 5660 -2040 5660 -1460 { lab=#net39}
N 5660 -1460 5720 -1460 { lab=#net39}
N 5700 -1540 5720 -1540 { lab=#net37}
N 5700 -1600 5700 -1540 { lab=#net37}
N 5700 -1600 5890 -1730 { lab=#net37}
N 5890 -1870 5890 -1730 { lab=#net37}
N 5920 -1780 5950 -1780 { lab=#net37}
N 5920 -1870 5920 -1780 { lab=#net37}
N 5920 -1610 5950 -1610 { lab=#net40}
N 5920 -1610 5920 -1500 { lab=#net40}
N 5950 -1570 5950 -1230 { lab=CLEAR_}
N 5540 -1310 5720 -1310 { lab=CLEAR_}
N 5540 -1310 5540 -1230 { lab=CLEAR_}
N 5540 -1830 5540 -1310 { lab=CLEAR_}
N 5540 -1830 5720 -1830 { lab=CLEAR_}
N 5840 -1870 5890 -1870 { lab=#net37}
N 5540 -1230 5950 -1230 { lab=CLEAR_}
N 5610 -1500 5610 -1110 { lab=CLK}
N 6520 -1900 6520 -1870 { lab=#net42}
N 6400 -1970 6520 -1900 { lab=#net42}
N 6400 -2000 6400 -1970 { lab=#net42}
N 6520 -2020 6520 -1990 { lab=#net43}
N 6400 -1920 6520 -1990 { lab=#net43}
N 6400 -1920 6400 -1910 { lab=#net43}
N 6570 -1870 6600 -1870 { lab=#net42}
N 6340 -2040 6400 -2040 { lab=#net44}
N 6520 -1380 6520 -1350 { lab=#net44}
N 6400 -1450 6520 -1380 { lab=#net44}
N 6400 -1460 6400 -1450 { lab=#net44}
N 6520 -1500 6520 -1470 { lab=#net45}
N 6400 -1400 6520 -1470 { lab=#net45}
N 6520 -1500 6600 -1500 { lab=#net45}
N 6290 -1500 6400 -1500 { lab=CLK}
N 6750 -1640 6750 -1610 { lab=#net46}
N 6630 -1710 6750 -1640 { lab=#net46}
N 6630 -1740 6630 -1710 { lab=#net46}
N 6750 -1760 6750 -1730 { lab=q9}
N 6630 -1660 6750 -1730 { lab=q9}
N 6630 -1660 6630 -1650 { lab=q9}
N 6760 -1760 6770 -1760 { lab=q9}
N 6290 -1870 6400 -1870 { lab=CLK}
N 6290 -1870 6290 -1500 { lab=CLK}
N 6340 -2040 6340 -1460 { lab=#net44}
N 6340 -1460 6400 -1460 { lab=#net44}
N 6380 -1540 6400 -1540 { lab=#net42}
N 6380 -1600 6380 -1540 { lab=#net42}
N 6380 -1600 6570 -1730 { lab=#net42}
N 6570 -1870 6570 -1730 { lab=#net42}
N 6600 -1780 6630 -1780 { lab=#net42}
N 6600 -1870 6600 -1780 { lab=#net42}
N 6600 -1610 6630 -1610 { lab=#net45}
N 6600 -1610 6600 -1500 { lab=#net45}
N 6630 -1570 6630 -1230 { lab=CLEAR_}
N 6220 -1310 6400 -1310 { lab=CLEAR_}
N 6220 -1310 6220 -1230 { lab=CLEAR_}
N 6220 -1830 6220 -1310 { lab=CLEAR_}
N 6220 -1830 6400 -1830 { lab=CLEAR_}
N 6520 -1870 6570 -1870 { lab=#net42}
N 6730 -1760 6750 -1760 { lab=q9}
N 6220 -1230 6630 -1230 { lab=CLEAR_}
N 6290 -1500 6290 -1110 { lab=CLK}
N 1430 -1760 1430 -1350 { lab=q2}
N 1430 -1350 1680 -1350 { lab=q2}
N 2090 -1760 2090 -1350 { lab=q3}
N 2090 -1350 2340 -1350 { lab=q3}
N 2760 -1760 2760 -1350 { lab=q4}
N 2760 -1350 3000 -1350 { lab=q4}
N 1250 -1230 1500 -1230 { lab=CLEAR_}
N 910 -1110 1570 -1110 { lab=CLK}
N 1910 -1230 2160 -1230 { lab=CLEAR_}
N 1570 -1110 2230 -1110 { lab=CLK}
N 2570 -1230 2820 -1230 { lab=CLEAR_}
N 2230 -1110 2890 -1110 { lab=CLK}
N 3230 -1230 4200 -1230 { lab=CLEAR_}
N 2890 -1110 4270 -1110 { lab=CLK}
N 4610 -1230 4870 -1230 { lab=CLEAR_}
N 4270 -1110 4940 -1110 { lab=CLK}
N 5280 -1230 5540 -1230 { lab=CLEAR_}
N 4940 -1110 5610 -1110 { lab=CLK}
N 5950 -1230 6220 -1230 { lab=CLEAR_}
N 5610 -1110 6290 -1110 { lab=CLK}
N 2690 -1760 2760 -1760 { lab=q4}
N 4800 -1760 4800 -1350 { lab=q6}
N 4800 -1350 5050 -1350 { lab=q6}
N 5470 -1760 5470 -1350 { lab=q7}
N 5470 -1350 5720 -1350 { lab=q7}
N 6150 -1760 6150 -1350 { lab=q8}
N 6150 -1350 6400 -1350 { lab=q8}
N 6130 -1760 6150 -1760 { lab=q8}
N 360 -1400 360 -1390 { lab=#net4}
N 1020 -1400 1020 -1390 { lab=#net10}
N 1680 -1400 1680 -1390 { lab=#net15}
N 2340 -1400 2340 -1390 { lab=#net20}
N 3000 -1400 3000 -1390 { lab=#net25}
N 4380 -1400 4380 -1390 { lab=#net30}
N 5050 -1400 5050 -1390 { lab=#net35}
N 5720 -1400 5720 -1390 { lab=#net40}
N 6400 -1400 6400 -1390 { lab=#net45}
N 5990 -2250 6760 -2250 { lab=q9}
N 6760 -2250 6760 -1760 { lab=q9}
N 6130 -2210 6130 -1760 { lab=q8}
N 5990 -2210 6130 -2210 { lab=q8}
N 120 -2230 5220 -2230 { lab=#net6}
N 120 -2230 120 -1350 { lab=#net6}
N 6750 -1760 6760 -1760 { lab=q9}
N 6070 -1760 6130 -1760 { lab=q8}
N 5300 -2230 5870 -2230 { lab=#net47}
C {nand2_1.sym} 420 -2020 0 0 {name=x45 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 650 -1760 0 0 {name=x49 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 420 -1870 0 0 {name=x51 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {ipin.sym} 110 -1110 0 0 {name=p8 lab=CLK}
C {nand3_1.sym} 420 -1500 0 0 {name=x46 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 650 -1610 0 0 {name=x50 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {ipin.sym} 110 -1230 0 0 {name=p11 lab=CLEAR_}
C {nand3_1.sym} 420 -1350 0 0 {name=x48 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 1080 -2020 0 0 {name=x47 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 1310 -1760 0 0 {name=x52 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1080 -1870 0 0 {name=x53 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1080 -1500 0 0 {name=x54 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1310 -1610 0 0 {name=x55 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1080 -1350 0 0 {name=x56 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 1740 -2020 0 0 {name=x57 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 1970 -1760 0 0 {name=x58 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1740 -1870 0 0 {name=x59 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1740 -1500 0 0 {name=x60 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1970 -1610 0 0 {name=x61 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 1740 -1350 0 0 {name=x62 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 2400 -2020 0 0 {name=x63 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 2630 -1760 0 0 {name=x64 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2400 -1870 0 0 {name=x65 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2400 -1500 0 0 {name=x66 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2630 -1610 0 0 {name=x67 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2400 -1350 0 0 {name=x68 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 3060 -2020 0 0 {name=x69 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 3290 -1760 0 0 {name=x70 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 3060 -1870 0 0 {name=x71 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 3060 -1500 0 0 {name=x72 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 3290 -1610 0 0 {name=x73 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 3060 -1350 0 0 {name=x74 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 4440 -2020 0 0 {name=x75 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 4670 -1760 0 0 {name=x76 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 4440 -1870 0 0 {name=x77 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 4440 -1500 0 0 {name=x78 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 4670 -1610 0 0 {name=x79 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 4440 -1350 0 0 {name=x80 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 5110 -2020 0 0 {name=x81 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 5340 -1760 0 0 {name=x82 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5110 -1870 0 0 {name=x83 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5110 -1500 0 0 {name=x84 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5340 -1610 0 0 {name=x85 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5110 -1350 0 0 {name=x86 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 5780 -2020 0 0 {name=x87 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 6010 -1760 0 0 {name=x88 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5780 -1870 0 0 {name=x89 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5780 -1500 0 0 {name=x90 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 6010 -1610 0 0 {name=x91 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 5780 -1350 0 0 {name=x92 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 6460 -2020 0 0 {name=x93 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 6690 -1760 0 0 {name=x94 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 6460 -1870 0 0 {name=x95 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 6460 -1500 0 0 {name=x96 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 6690 -1610 0 0 {name=x97 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 6460 -1350 0 0 {name=x98 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 770 -1760 0 1 {name=l29 sig_type=std_logic lab=q1}
C {lab_pin.sym} 1430 -1760 0 1 {name=l30 sig_type=std_logic lab=q2}
C {lab_pin.sym} 2090 -1760 0 1 {name=l31 sig_type=std_logic lab=q3}
C {lab_pin.sym} 2760 -1760 0 1 {name=l32 sig_type=std_logic lab=q4}
C {lab_pin.sym} 4130 -1760 0 1 {name=l33 sig_type=std_logic lab=q5}
C {lab_pin.sym} 4800 -1760 0 1 {name=l34 sig_type=std_logic lab=q6}
C {lab_pin.sym} 5470 -1760 0 1 {name=l35 sig_type=std_logic lab=q7}
C {lab_pin.sym} 6150 -1760 0 1 {name=l36 sig_type=std_logic lab=q8}
C {lab_pin.sym} 6770 -1760 0 1 {name=l37 sig_type=std_logic lab=q9}
C {xor2_1.sym} 5930 -2230 0 1 {name=x99 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {inv_2.sym} 5260 -2230 2 0 {name=x100 VGND=GND VNB=GND VPB=VCC VPWR=VCC prefix=sky130_fd_sc_hd__ }
C {title.sym} 240 -50 0 0 {name=l38 author="Stefan Schippers"}
C {launcher.sym} 960 -370 0 0 {name=h3
descr="START SIMULATION" 
comment="
  This launcher Starts a simple interactive simulation of the LFSR
"
tclcommand="  set tclstop 0  ;# clear stop flag
  set count 0
  xschem select instance l23 ;# VSS
  xschem select instance p11 ;# CLEAR
  xschem select instance p8  ;# CLOCK
  xschem logic_set 0         ;# reset pulse (active low) 
  update ;# allow event loop to come in (update screen, user input etc)
  after 1000
  xschem select instance p8 clear ;# release CLOCK
  xschem select instance l23 clear ;# release VSS
  xschem logic_set 1
  xschem select instance p11 clear ;# release CLEAR
  xschem select instance p8
  update ;# allow event loop to come in (update screen, user input etc) 
  after 1000
  set logic_value 0
  while \{1\} \{
    update ;# allow event loop to come in (update screen, user input etc) 
    incr count
    after 500
    if \{$count==30 || $tclstop == 1\} break
    xschem logic_set $logic_value
    set logic_value [expr !$logic_value]
  \}
"
}
C {giant_label2.sym} 240 -3040 0 0 {name=l1 sig_type=std_logic lab=q1 text=M}
C {giant_label2.sym} 880 -3040 0 0 {name=l2 sig_type=std_logic lab=q2 text=E}
C {giant_label2.sym} 1520 -3040 0 0 {name=l3 sig_type=std_logic lab=q3 text=R}
C {giant_label2.sym} 2160 -3040 0 0 {name=l4 sig_type=std_logic lab=q4 text=R}
C {giant_label2.sym} 2800 -3040 0 0 {name=l5 sig_type=std_logic lab=q5 text=Y}
C {giant_label2.sym} 4240 -3040 0 0 {name=l6 sig_type=std_logic lab=q6 text=X}
C {giant_label2.sym} 4880 -3040 0 0 {name=l7 sig_type=std_logic lab=q7 text=M}
C {giant_label2.sym} 5520 -3040 0 0 {name=l8 sig_type=std_logic lab=q8 text=A}
C {giant_label2.sym} 6160 -3040 0 0 {name=l9 sig_type=std_logic lab=q9 text=S}
