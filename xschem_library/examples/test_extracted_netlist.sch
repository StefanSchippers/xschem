v {xschem version=3.4.5 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
}
G {}
K {}
V {}
S {}
E {}
L 7 40 -800 80 -800 {}
B 2 850 -470 1650 -70 {flags=graph
y1=-58.7947
y2=61.2053
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=2e-05
x2=0.00042
divx=5
subdivx=1


dataset=0
unitx=u
color="7 8 12 4 9 7"
node="OUT
\\"in*12; in 12 *\\"
MINUS
PLUS
VPP
VNN"
hilight_wave=-1}
P 7 7 60 -700 60 -760 50 -760 60 -790 70 -760 60 -760 60 -700 {fill=true}
T {actual value
50u} 270 -580 0 0 0.4 0.4 {}
T {Example using a symbol with an extracted netlist} 30 -1100 0 0 1 1 {}
T {Ideal gain should be 
Gain =
(R2 + R1 // R13) / (R1 // R13)} 10 -910 0 0 0.4 0.4 {}
T {tcleval(= [expr \{ ( [g R2] + [rpar R1 R13] ) / [rpar R1 R13] \}])} 10 -830 0 0 0.4 0.4 {name=R2}
T {Title defines some helper tcl procedures} 260 -80 0 0 0.4 0.4 { layer=7}
T {This text is a floater on R2,
floater attribute only used
to force a tcl /attribute
substitution } 10 -680 0 0 0.4 0.4 { layer=7}
N 360 -850 360 -830 {
lab=MINUS}
N 290 -280 290 -260 {lab=#net1}
N 290 -140 290 -120 {lab=#net2}
N 290 -120 400 -120 {lab=#net2}
N 290 -280 400 -280 {lab=#net1}
N 460 -280 530 -280 {lab=VPP}
N 460 -120 530 -120 {lab=VNN}
N 460 -200 530 -200 {lab=VSS}
N 290 -200 400 -200 {lab=#net3}
N 660 -210 660 -190 {
lab=VSS}
N 920 -1000 920 -850 {
lab=OUT}
N 360 -1000 920 -1000 {
lab=OUT}
N 360 -1000 360 -930 {
lab=OUT}
N 490 -850 620 -850 {
lab=MINUS}
N 550 -830 620 -830 {
lab=PLUS}
N 550 -830 550 -510 {
lab=PLUS}
N 1030 -850 1060 -850 {
lab=OUT}
N 30 -390 30 -330 {
lab=VSS}
N 460 -690 460 -670 {lab=VPP}
N 480 -510 480 -480 {lab=PLUS}
N 480 -420 480 -400 {lab=VSS}
N 460 -510 480 -510 {lab=PLUS}
N 400 -510 460 -510 {lab=PLUS}
N 460 -610 460 -510 { lab=PLUS}
N 360 -870 360 -850 {
lab=MINUS}
N 360 -710 360 -680 {
lab=VSS}
N 480 -510 550 -510 {
lab=PLUS}
N 30 -510 340 -510 {
lab=IN}
N 490 -960 490 -940 {lab=VPP}
N 490 -880 490 -850 { lab=MINUS}
N 1030 -850 1030 -780 {
lab=OUT}
N 1030 -720 1030 -690 {
lab=VSS}
N 30 -510 30 -450 {
lab=IN}
N 360 -850 490 -850 {
lab=MINUS}
N 920 -850 1030 -850 {
lab=OUT}
C {code.sym} 20 -190 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value=".include \\"models_poweramp.txt\\"
.options method=gear savecurrents ITL4=500 ITL5=50000  CHGTOL=1e-15 TRTOL=1
.control
save all
op
write test_extracted_netlist.raw
set appendwrite
tran  100n 400u 
* .FOUR 20k v(outm,outp)
* .probe i(*) 
write test_extracted_netlist.raw
.endc
"}
C {title.sym} 160 -30 0 0 {name=l2 author="tcleval(Stefan Schippers[
proc g \{n\} \{
  return [from_eng [xschem getprop instance $n value]]
\}
proc rpar \{n1 n2\} \{
  return [expr \{ [g $n1] * [g $n2] / ([g $n1] + [g $n2]) \}]
\}
])"
}
C {lab_wire.sym} 620 -850 0 0 {name=l1 lab=MINUS}
C {lab_pin.sym} 620 -810 0 0 {name=p3 lab=VSS}
C {lab_pin.sym} 1060 -850 0 1 {name=p4 lab=OUT}
C {lab_pin.sym} 620 -790 0 0 {name=p5 lab=VPP}
C {lab_pin.sym} 620 -770 0 0 {name=p6 lab=VNN}
C {res.sym} 360 -800 0 0 {name=R1
value=10k
footprint=1206
device=resistor
m=1}
C {res.sym} 360 -900 0 0 {name=R2
value=100k
footprint=1206
device=resistor
m=1}
C {vsource.sym} 290 -230 0 0 {name=VPP value="dc 50"}
C {vsource.sym} 290 -170 0 0 {name=VNN value="dc 50"}
C {lab_pin.sym} 530 -280 0 1 {name=p7 lab=VPP}
C {lab_pin.sym} 530 -120 0 1 {name=p8 lab=VNN}
C {lab_pin.sym} 530 -200 0 1 {name=p9 lab=VSS}
C {ammeter.sym} 430 -280 3 0 {name=vcurrvpp}
C {ammeter.sym} 430 -120 3 0 {name=vcurrvnn}
C {ammeter.sym} 430 -200 3 0 {name=vcurrvss}
C {ngspice_probe.sym} 370 -200 0 1 {name=p34}
C {ngspice_probe.sym} 290 -280 0 1 {name=p35}
C {ngspice_probe.sym} 290 -120 0 1 {name=p36}
C {ngspice_probe.sym} 530 -200 0 0 {name=p37}
C {spice_probe.sym} 520 -280 0 0 {name=p45 analysis=tran voltage=49.93}
C {spice_probe.sym} 520 -120 0 0 {name=p46 analysis=tran voltage=-49.93}
C {vsource.sym} 660 -160 0 0 {name=VVSS value=0}
C {lab_pin.sym} 660 -210 0 1 {name=p10 lab=VSS}
C {lab_pin.sym} 660 -130 0 1 {name=p11 lab=0}
C {lab_pin.sym} 360 -680 0 1 {name=p12 lab=VSS}
C {vsource.sym} 30 -420 0 0 {name=VIN value="pwl 
+   0     0 100u  0
+ 110u   2 200u 2
+ 210u    0 300u  0
+ 310u  -2"
}
C {lab_pin.sym} 30 -330 0 1 {name=p1 lab=VSS}
C {spice_probe.sym} 970 -850 0 0 {name=p2 analysis=tran voltage=-0.1176}
C {spice_probe.sym} 380 -850 0 0 {name=p13 analysis=tran voltage=21.07}
C {spice_probe.sym} 120 -510 0 0 {name=p14 analysis=tran voltage=0.0000e+00}
C {lab_wire.sym} 620 -830 0 0 {name=l3 lab=PLUS}
C {launcher.sym} 905 -535 0 0 {name=h5 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran
"
}
C {res.sym} 460 -640 0 1 {name=R4
value=100k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 460 -690 0 0 {name=p18 lab=VPP}
C {res.sym} 480 -450 0 1 {name=R5
value=100k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 480 -400 0 0 {name=p15 lab=VSS}
C {capa.sym} 370 -510 1 0 {name=C1 m=1 value="50u"}
C {capa.sym} 360 -740 0 0 {name=C4 m=1 value="50u"}
C {lab_wire.sym} 210 -510 0 0 {name=l4 lab=IN}
C {res.sym} 490 -910 0 1 {name=R13 m=1 value=100k}
C {lab_pin.sym} 490 -960 0 0 {name=p16 lab=VPP}
C {res.sym} 1030 -750 0 0 {name=RLOAD
value=4
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1030 -690 0 0 {name=p17 lab=VSS}
C {launcher.sym} 660 -350 0 0 {name=h1
descr=Backannotate
tclcommand="xschem annotate_op"}
C {spice_probe.sym} 550 -620 0 0 {name=p19 analysis=tran voltage=21.07}
C {lab_pin.sym} 360 -770 0 1 {name=p20 lab=C1}
C {mos_power_ampli_extracted.sym} 770 -810 0 0 {name=x1}
