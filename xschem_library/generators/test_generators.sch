v {xschem version=3.4.4 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
L 4 290 -510 290 -210 {}
L 4 290 -560 290 -510 {}
P 3 5 400 -440 620 -440 620 -350 400 -350 400 -440 {dash=4}
P 3 5 140 -250 220 -250 220 -170 140 -170 140 -250 {dash=4}
P 3 5 140 -540 220 -540 220 -460 140 -460 140 -540 {dash=4}
P 4 7 710 -390 670 -390 670 -400 640 -390 670 -380 670 -390 710 -390 {}
P 4 7 290 -510 250 -510 250 -520 220 -510 250 -500 250 -510 290 -510 {}
P 4 7 290 -210 250 -210 250 -220 220 -210 250 -200 250 -210 290 -210 {}
T {Resistor generator (res.tcl) 
if a value <= 0.1 is given
it becomes a short} 720 -410 0 0 0.4 0.4 { layer=4}
T {Tier generator (tier.tcl) 
if 'lab' matches VDD | VCC | VPP shows 
as supply node, else show as ground node} 240 -640 0 0 0.4 0.4 { layer=4}
T {Title symbol has embedded TCL command
to enable show_pin_net_names.} 160 -120 0 0 0.4 0.4 { layer=6}
N 180 -300 180 -240 {
lab=VSS}
N 180 -480 180 -360 {
lab=VDD}
N 460 -480 460 -420 {
lab=VDD}
N 180 -480 460 -480 {
lab=VDD}
N 570 -480 570 -420 {
lab=VDD}
N 460 -480 570 -480 {
lab=VDD}
N 460 -360 460 -280 {
lab=#net1}
N 570 -360 570 -280 {
lab=VDD}
C {tier.tcl(@lab\\)} 180 -480 0 0 {name=p1 sig_type=std_logic lab=VDD
tclcommand="edit_file [abs_sym_path tier.tcl]"}
C {tier.tcl(@lab\\)} 180 -240 0 0 {name=p2 sig_type=std_logic lab=VSS
tclcommand="edit_file [abs_sym_path tier.tcl]"
}
C {res.sym} 180 -330 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_show.sym} 180 -380 0 0 {name=l1}
C {lab_show.sym} 180 -260 0 0 {name=l2}
C {capa.sym} 460 -250 0 0 {name=C1
m=1
value=1p
footprint=1206
device="ceramic capacitor"
net_name=true}
C {capa.sym} 570 -250 0 0 {name=C2
m=1
value=1p
footprint=1206
device="ceramic capacitor"
net_name=true}
C {tier.tcl(@lab\\)} 460 -220 0 0 {name=p3 sig_type=std_logic lab=VSS}
C {tier.tcl(@lab\\)} 570 -220 0 0 {name=p4 sig_type=std_logic lab=VSS}
C {res.tcl(@value\\)} 460 -390 0 0 {name=R2 value=100
net_name=true
tclcommand="edit_file [abs_sym_path res.tcl]"
}
C {res.tcl(@value\\)} 570 -390 0 0 {name=R3 value=0.1
net_name=true
tclcommand="edit_file [abs_sym_path res.tcl]"
}
C {lab_show.sym} 460 -290 0 0 {name=l3}
C {lab_show.sym} 570 -290 0 0 {name=l4}
C {title.sym} 160 -30 0 0 {name=l5
author="tcleval([
  if \{$show_pin_net_names == 0\} \{
    set show_pin_net_names 1
    xschem update_all_sym_bboxes
  \}
  return \{Stefan Schippers\}
])"
}
