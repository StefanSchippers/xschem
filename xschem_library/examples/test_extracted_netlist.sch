v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
B 2 840 -490 1640 -90 {flags=graph
y1=-29
y2=41
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=0.0004
divx=5
subdivx=1


dataset=0
unitx=u
color="7 8"
node="out
in"}
T {actual value
50u} 270 -580 0 0 0.4 0.4 {}
T {Example using a symbol with an extracted netlist} 220 -1160 0 0 1 1 {}
N 350 -920 350 -900 {
lab=MINUS}
N 290 -280 290 -260 {lab=#net1}
N 290 -140 290 -120 {lab=#net2}
N 480 -130 480 -120 {lab=VNN}
N 480 -200 480 -190 {lab=VSS}
N 290 -120 320 -120 {lab=#net2}
N 290 -280 320 -280 {lab=#net1}
N 480 -200 490 -200 {lab=VSS}
N 460 -280 490 -280 {lab=VPP}
N 460 -120 480 -120 {lab=VNN}
N 460 -200 480 -200 {lab=VSS}
N 490 -280 530 -280 {lab=VPP}
N 480 -120 530 -120 {lab=VNN}
N 490 -200 530 -200 {lab=VSS}
N 490 -210 490 -200 { lab=VSS}
N 490 -280 490 -270 { lab=VPP}
N 380 -280 400 -280 {lab=#net3}
N 380 -120 400 -120 {lab=#net4}
N 290 -200 400 -200 {lab=#net5}
N 660 -210 660 -190 {
lab=VSS}
N 920 -1070 920 -920 {
lab=OUT}
N 350 -1070 920 -1070 {
lab=OUT}
N 350 -1070 350 -1000 {
lab=OUT}
N 350 -920 620 -920 {
lab=MINUS}
N 550 -900 620 -900 {
lab=PLUS}
N 550 -900 550 -510 {
lab=PLUS}
N 920 -920 1060 -920 {
lab=OUT}
N 30 -390 30 -330 {
lab=VSS}
N 460 -690 460 -670 {lab=VPP}
N 480 -510 480 -480 {lab=PLUS}
N 480 -420 480 -400 {lab=VSS}
N 460 -510 480 -510 {lab=PLUS}
N 400 -510 460 -510 {lab=PLUS}
N 460 -610 460 -510 { lab=PLUS}
N 350 -940 350 -920 {
lab=MINUS}
N 350 -780 350 -750 {
lab=VSS}
N 480 -510 550 -510 {
lab=PLUS}
N 30 -510 340 -510 {
lab=IN}
N 490 -1030 490 -1010 {lab=VPP}
N 490 -950 490 -940 { lab=MINUS}
N 490 -940 490 -920 { lab=MINUS}
N 1030 -920 1030 -850 {
lab=OUT}
N 1030 -790 1030 -760 {
lab=VSS}
N 30 -510 30 -450 {
lab=IN}
C {code.sym} 20 -190 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value=".include \\"models_poweramp.txt\\"
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
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
C {lab_wire.sym} 620 -920 0 0 {name=l1 lab=MINUS}
C {lab_pin.sym} 620 -880 0 0 {name=p3 lab=VSS}
C {lab_pin.sym} 1060 -920 0 1 {name=p4 lab=OUT}
C {lab_pin.sym} 620 -860 0 0 {name=p5 lab=VPP}
C {lab_pin.sym} 620 -840 0 0 {name=p6 lab=VNN}
C {mos_power_ampli_extracted.sym} 770 -880 0 0 {name=x2}
C {res.sym} 350 -870 0 0 {name=R1
value=100k
footprint=1206
device=resistor
m=1}
C {res.sym} 350 -970 0 0 {name=R2
value=100k
footprint=1206
device=resistor
m=1}
C {vsource.sym} 290 -230 0 0 {name=VPP value="dc 50"}
C {vsource.sym} 290 -170 0 0 {name=VNN value="dc 50"}
C {lab_pin.sym} 530 -280 0 1 {name=p7 lab=VPP}
C {lab_pin.sym} 530 -120 0 1 {name=p8 lab=VNN}
C {lab_pin.sym} 530 -200 0 1 {name=p9 lab=VSS}
C {capa.sym} 480 -160 0 0 {name=C3 m=1 value="100u"}
C {res.sym} 350 -280 1 1 {name=R11 m=1 value=0.3}
C {res.sym} 350 -120 1 1 {name=R9 m=1 value=0.3}
C {ammeter.sym} 430 -280 3 0 {name=vcurrvpp  net_name=true       current=0.54}
C {ammeter.sym} 430 -120 3 0 {name=vcurrvnn  net_name=true       current=-0.4526}
C {ammeter.sym} 430 -200 3 0 {name=vcurrvss  net_name=true       current=-0.08742}
C {ngspice_probe.sym} 370 -200 0 1 {name=p34}
C {capa.sym} 490 -240 0 0 {name=C2 m=1 value="100u"}
C {ngspice_probe.sym} 290 -280 0 1 {name=p35}
C {ngspice_probe.sym} 290 -120 0 1 {name=p36}
C {ngspice_probe.sym} 530 -200 0 0 {name=p37}
C {spice_probe.sym} 520 -280 0 0 {name=p45 analysis=tran voltage=49.93}
C {spice_probe.sym} 520 -120 0 0 {name=p46 analysis=tran voltage=-49.93}
C {vsource.sym} 660 -160 0 0 {name=VVSS value=0}
C {lab_pin.sym} 660 -210 0 1 {name=p10 lab=VSS}
C {lab_pin.sym} 660 -130 0 1 {name=p11 lab=0}
C {lab_pin.sym} 350 -750 0 1 {name=p12 lab=VSS}
C {vsource.sym} 30 -420 0 0 {name=VIN value="pwl 
+   0     0 100u  0
+ 110u   10 200u 10
+ 210u    0 300u  0
+ 310u  -10"
}
C {lab_pin.sym} 30 -330 0 1 {name=p1 lab=VSS}
C {spice_probe.sym} 970 -920 0 0 {name=p2 analysis=tran voltage=-0.1176}
C {spice_probe.sym} 370 -920 0 0 {name=p13 analysis=tran voltage=21.07}
C {spice_probe.sym} 120 -510 0 0 {name=p14 analysis=tran voltage=0.0000e+00}
C {lab_wire.sym} 620 -900 0 0 {name=l3 lab=PLUS}
C {launcher.sym} 905 -535 0 0 {name=h5 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
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
C {capa.sym} 350 -810 0 0 {name=C4 m=1 value="50u"}
C {lab_wire.sym} 210 -510 0 0 {name=l4 lab=IN}
C {res.sym} 490 -980 0 1 {name=R13 m=1 value=100k}
C {lab_pin.sym} 490 -1030 0 0 {name=p16 lab=VPP}
C {res.sym} 1030 -820 0 0 {name=RLOAD
value=4
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1030 -760 0 0 {name=p17 lab=VSS}
C {launcher.sym} 660 -350 0 0 {name=h1
descr=Backannotate
tclcommand="ngspice::annotate"}
C {spice_probe.sym} 550 -620 0 0 {name=p19 analysis=tran voltage=21.07}
