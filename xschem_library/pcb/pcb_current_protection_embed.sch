v {xschem version=3.4.4 file_version=1.2
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
B 2 50 -970 850 -570 {flags=graph
y1=2.3e-06
y2=5.1
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-0.00212084
x2=0.0379531
divx=5
subdivx=1
node="vssload
vref"
color="4 8"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 50 -1370 850 -970 {flags=graph
y1=-3.6e-05
y2=0.066
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-0.00212084
x2=0.0379531
divx=5
subdivx=1


dataset=-1
unitx=1
logx=0
logy=0
color=4
node=i(vmeasure)}
N 360 -200 360 -190 {lab=VREF}
N 360 -130 360 -110 {lab=VSS}
N 360 -260 360 -200 {lab=VREF}
N 360 -230 450 -230 {lab=VREF}
N 360 -350 360 -320 {lab=VCC}
N 450 -410 450 -310 {lab=VSSLOAD}
N 450 -410 950 -410 {lab=VSSLOAD}
N 950 -460 950 -390 {lab=VSSLOAD}
N 950 -330 950 -300 {lab=D}
N 680 -150 700 -150 {lab=#net1}
N 590 -150 620 -150 {lab=RST}
N 740 -270 740 -180 {lab=#net2}
N 810 -250 810 -230 {lab=G}
N 720 -270 740 -270 {lab=#net2}
N 740 -200 770 -200 {lab=#net2}
N 810 -240 860 -240 {lab=G}
N 860 -270 860 -240 {lab=G}
N 860 -270 910 -270 {lab=G}
C {code.sym} 1060 -350 0 0 {name=STIMULI
tedax_ignore=true
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="
*.option PARHIER=LOCAL RUNLVL=5 post MODMONTE=1 warn maxwarns=400 ingold=1


vvcc vcc 0 dc 5
vvss vss 0 dc 0
vrst rst 0 pwl 0 5 100u 5 101u 0

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.include \\"models_pcb_current_protection.txt\\"
* .dc RLOAD 10K 1 -0.001
.control
  save all
  tran 10u 200m
  write pcb_current_protection_embed.raw
.endc
"
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
format="
@value
"}
G {}
V {}
S {}
E {}
L 4 20 30 60 30 {}
L 4 20 40 40 40 {}
L 4 20 50 60 50 {}
L 4 20 60 50 60 {}
L 4 20 70 50 70 {}
L 4 20 80 90 80 {}
L 4 20 90 40 90 {}
L 4 20 20 70 20 {}
L 4 20 10 40 10 {}
L 4 100 10 110 10 {}
L 4 110 10 110 110 {}
L 4 20 110 110 110 {}
L 4 20 100 20 110 {}
L 4 100 0 100 100 {}
L 4 10 100 100 100 {}
L 4 10 0 10 100 {}
L 4 10 0 100 0 {}
T {@name} 15 -25 0 0 0.3 0.3 {}
]
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan"
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=logo
template="name=l1 author=\\"Stefan Schippers\\""
verilog_ignore=true
vhdl_ignore=true
spice_ignore=true
tedax_ignore=true}
G {}
V {}
S {}
E {}
L 6 225 0 1020 0 {}
L 6 -160 0 -95 0 {}
T {@schname_ext} 235 5 0 0 0.4 0.4 {}
T {@author} 235 -25 0 0 0.4 0.4 {}
T {@time_last_modified} 1020 -25 0 1 0.4 0.3 {}
T {SCHEM} 5 -25 0 0 1 1 {}
P 5 13 5 -30 -25 0 5 30 -15 30 -35 10 -55 30 -75 30 -45 0 -75 -30 -55 -30 -35 -10 -15 -30 5 -30 {fill=true}
]
C {lab_pin.sym} 90 -280 0 1 {name=p8 lab=VCC
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=label
format="*.alias @lab"
template="name=p1 sig_type=std_logic lab=xxx"}
G {}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
T {@spice_get_voltage} 1.875 3.90625 0 0 0.2 0.2 {layer=15 hide=true}
]
C {conn_8x1.sym} 70 -260 0 0 {name=C2
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=connector
verilog_ignore=true
format="*connector(8,1) @pinlist"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=c1 footprint=connector(8,1)"
}
G {}
V {}
S {}
E {}
B 5 18.75 -21.25 21.25 -18.75 {name=conn_1 dir=inout pinnumber=1}
B 5 18.75 -1.25 21.25 1.25 {name=conn_2 dir=inout pinnumber=2}
B 5 18.75 18.75 21.25 21.25 {name=conn_3 dir=inout pinnumber=3}
B 5 18.75 38.75 21.25 41.25 {name=conn_4 dir=inout pinnumber=4}
B 5 18.75 58.75 21.25 61.25 {name=conn_5 dir=inout pinnumber=5}
B 5 18.75 78.75 21.25 81.25 {name=conn_6 dir=inout pinnumber=6}
B 5 18.75 98.75 21.25 101.25 {name=conn_7 dir=inout pinnumber=7}
B 5 18.75 118.75 21.25 121.25 {name=conn_8 dir=inout pinnumber=8}
A 4 15 -20 5 270 360 {}
A 4 15 0 5 270 360 {}
A 4 15 20 5 270 360 {}
A 4 15 40 5 270 360 {}
A 4 15 60 5 270 360 {}
A 4 15 80 5 270 360 {}
A 4 15 100 5 270 360 {}
A 4 15 120 5 270 360 {}
T {@#0:pinnumber} 6.25 -26.25 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} 6.25 -6.25 0 1 0.2 0.2 {layer=13}
T {@#2:pinnumber} 6.25 13.75 0 1 0.2 0.2 {layer=13}
T {@name} -18.75 -43.75 0 0 0.2 0.2 {}
T {@#3:pinnumber} 6.25 33.75 0 1 0.2 0.2 {layer=13}
T {@#4:pinnumber} 6.25 53.75 0 1 0.2 0.2 {layer=13}
T {@#5:pinnumber} 6.25 73.75 0 1 0.2 0.2 {layer=13}
T {@#6:pinnumber} 6.25 93.75 0 1 0.2 0.2 {layer=13}
T {@#7:pinnumber} 6.25 113.75 0 1 0.2 0.2 {layer=13}
P 4 5 10 130 -10 130 -10 -30 10 -30 10 130 {}
]
C {lab_pin.sym} 90 -260 0 1 {name=p9 lab=VSS
embed=true}
C {lm358.sym} 530 -270 0 0 {name=U3:1
spiceprefix=X
VCC=VCC 
VNN=VSS
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=3&ved=2ahUKEwi8_s-Z5KTfAhUmy4UKHbDOBUIQFjACegQICBAC&url=https%3A%2F%2Fwww.sparkfun.com%2Fdatasheets%2FComponents%2FGeneral%2FLM358.pdf&usg=AOvVaw0pOnlomz_hkzyk7OvZBPFv"
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=opamp
format="@spiceprefix@name @#0 @#1 @VCC @VNN @#2 @model"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=U1:1 
model=LM358 device=LM358 
footprint=so(8) 
numslots=2 
VCC=VCC 
VNN=VNN"

extra="VCC VNN"
extra_pinnumber="8 4"}
G {}
V {}
S {}
E {}
L 4 110 0 130 0 {}
L 4 -80 -40 -60 -40 {}
L 4 -80 40 -60 40 {}
L 4 -60 -70 110 0 {}
L 4 -60 70 110 0 {}
L 4 -60 -70 -60 70 {}
B 5 -82.5 -42.5 -77.5 -37.5 {name=PLUS sig_type=std_logic dir=in pinnumber=3:5}
B 5 -82.5 37.5 -77.5 42.5 {name=MINUS sig_type=std_logic dir=in pinnumber=2:6}
B 5 127.5 -2.5 132.5 2.5 {name=OUT sig_type=std_logic dir=out pinnumber=1:7}
T {@device} -48.5 -11 0 0 0.3 0.3 {}
T {@name} 0 -62 0 0 0.3 0.3 {}
T {@#2:pinnumber} 115 -14 0 0 0.2 0.2 {layer=13}
T {@#0:pinnumber} -65 -54 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} -65 26 0 1 0.2 0.2 {layer=13}
T {OUT} 90 -4 0 1 0.2 0.2 {}
T {PLUS} -55 -44 0 0 0.2 0.2 {}
T {MINUS} -55 36 0 0 0.2 0.2 {}
]
C {res.sym} 360 -160 0 0 {name=R3 m=1 value=4k footprint=1206
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=resistor

function0="1"
function1="0"

format="@name @pinlist @value m=@m"

verilog_format="tran @name (@@P\\\\, @@M\\\\);"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=R1
value=1k
footprint=1206
device=resistor
m=1"
}
G {}
V {}
S {}
E {}
L 4 0 20 0 30 {}
L 4 0 20 7.5 17.5 {}
L 4 -7.5 12.5 7.5 17.5 {}
L 4 -7.5 12.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -12.5 {}
L 4 -7.5 -17.5 7.5 -12.5 {}
L 4 -7.5 -17.5 0 -20 {}
L 4 0 -30 0 -20 {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=P dir=inout propag=1 pinnumber=1 goto=1}
B 5 -2.5 27.5 2.5 32.5 {name=M dir=inout propag=0 pinnumber=2 goto=0}
T {@name} -15 -13.75 0 1 0.2 0.2 {}
T {@value} 15 -6.25 0 0 0.2 0.2 {}
T {@#0:pinnumber} -10 -26.25 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} -10 16.25 0 1 0.2 0.2 {layer=13}
T {@#0:net_name} 10 -28.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 10 20 0 0 0.15 0.15 {layer=15}
T {m=@m} -15 1.25 0 1 0.2 0.2 {}
]
C {res.sym} 360 -290 0 0 {name=R4 m=1 value=100k footprint=1206
embed=true}
C {gnd.sym} 360 -110 0 0 {name=l0 lab=VSS
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=label
function0="L"
global=true
format="*.alias @lab"
template="name=l1 lab=GND"}
G {}
V {}
S {}
E {}
L 4 0 0 0 12.5 {}
L 4 -5 12.5 5 12.5 {}
L 4 0 17.5 5 12.5 {}
L 4 -5 12.5 0 17.5 {}
B 5 -2.5 -2.5 2.5 2.5 {name=p dir=inout goto=0}
T {@lab} 7.5 5 0 0 0.2 0.2 {}
]
C {nmos.sym} 930 -270 0 0 {name=M2
spiceprefix=X
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=nmos
format="@spiceprefix@name @pinlist @model @extra m=@m"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=M1 model=M2N7002 device=2N7002 footprint=SOT23 m=1"
verilog_format="@symname #@del @name ( @@d , @@s , @@g );"}
G {}
V {}
S {}
E {}
L 4 5 -20 20 -20 {}
L 4 20 -30 20 -20 {}
L 4 5 20 20 20 {}
L 4 20 20 20 30 {}
L 4 -5 -15 -5 15 {}
L 4 -5 0 -5 5 {}
L 4 -20 0 -12.5 0 {}
L 4 -20 0 -5 0 {}
L 4 5 -27.5 5 27.5 {}
B 5 17.5 -32.5 22.5 -27.5 {name=d dir=inout pinnumber=3}
B 5 -22.5 -2.5 -17.5 2.5 {name=g dir=in pinnumber=1}
B 5 17.5 27.5 22.5 32.5 {name=s dir=inout pinnumber=2}
T {@#0:pinnumber} 25 -27.5 0 0 0.2 0.2 {layer=13}
T {@#2:pinnumber} 25 17.5 0 0 0.2 0.2 {layer=13}
T {@#1:pinnumber} -8.75 6.25 0 1 0.2 0.2 {layer=13}
T {@device} 21.25 -11.25 0 0 0.2 0.2 {}
T {@name} 21.25 3.75 0 0 0.2 0.2 {}
T {D} 10 -17.5 0 0 0.2 0.2 {}
]
C {res.sym} 950 -360 0 0 {name=R5 m=1 value=1 footprint=1206
embed=true}
C {gnd.sym} 950 -240 0 0 {name=l6 lab=VSS
embed=true}
C {vdd.sym} 360 -350 0 0 {name=l7 lab=VCC
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=label
function0="H"
global=true
format="*.alias @lab"
template="name=l1 lab=VDD"}
G {}
V {}
S {}
E {}
L 4 0 -20 0 0 {}
L 4 -10 -20 10 -20 {}
B 5 -2.5 -2.5 2.5 2.5 {name=p dir=inout verilog_type=wire goto=0}
T {@lab} -12.5 -35 0 0 0.2 0.2 {}
]
C {vdd.sym} 950 -580 0 0 {name=l8 lab=VCC
embed=true}
C {lab_pin.sym} 950 -430 0 0 {name=p1 lab=VSSLOAD
embed=true}
C {lab_pin.sym} 360 -210 0 0 {name=p2 lab=VREF
embed=true}
C {res.sym} 950 -490 0 0 {name=RLOAD m=1 value="R='1/(time+1e-7)'" footprint=1206
embed=true}
C {ammeter.sym} 950 -550 0 0 {name=vmeasure
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=current_probe
format="@name @pinlist 0
.save I( ?1 @name )"
template="name=Vmeas"}
G {}
V {}
S {}
E {}
L 4 0 -30 0 30 {}
L 4 -7.5 0 -0 10 {}
L 4 -0 10 7.5 0 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=plus dir=inout propag=1}
B 5 -2.5 27.5 2.5 32.5 {name=minus dir=inout propag=0}
T {@name} 15 -18.75 0 0 0.2 0.2 {}
T {@#0:net_name} 10 -28.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 10 20 0 0 0.15 0.15 {layer=15}
T {@spice_get_current} 2.5 7.5 0 0 0.2 0.2 {layer=15
hide=true}
]
C {lab_wire.sym} 890 -270 0 0 {name=l12 lab=G
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=label
format="*.alias @lab"
template="name=p1 sig_type=std_logic lab=xxx"}
G {}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -2.5 -1.25 2 0 0.27 0.27 {}
T {@spice_get_voltage} 1.875 10.15625 0 0 0.2 0.2 {vcenter=true layer=15 hide=true}
]
C {lab_pin.sym} 950 -320 0 0 {name=p3 lab=D
embed=true}
C {res.sym} 690 -270 1 0 {name=R2 m=1 value=20k footprint=1206
embed=true}
C {npn.sym} 720 -150 0 0 {name=Q1 model=q2n2222
url="http://html.alldatasheet.com/html-pdf/17919/PHILIPS/MMBT2222/499/2/MMBT2222.html"
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=npn
format="@name @pinlist  @model area=@area m=@m"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=Q1
model=MMBT2222
device=MMBT2222
footprint=SOT23
area=1
m=1"
}
G {}
V {}
S {}
E {}
L 4 0 -30 0 30 {}
L 4 -20 0 -12.5 0 {}
L 4 -20 0 0 0 {}
L 4 -0 10 8.75 18.75 {}
L 4 0 -10 20 -30 {}
B 5 17.5 -32.5 22.5 -27.5 {name=C dir=inout pinnumber=3}
B 5 -22.5 -2.5 -17.5 2.5 {name=B dir=in pinnumber=1}
B 5 17.5 27.5 22.5 32.5 {name=E dir=inout pinnumber=2}
T {@model} 20 -12.5 0 0 0.2 0.2 {}
T {@name} 20 0 0 0 0.2 0.2 {}
T {@#0:pinnumber} 25 -25 0 0 0.2 0.2 {layer=13}
T {@#2:pinnumber} 25 12.5 0 0 0.2 0.2 {layer=13}
T {@#1:pinnumber} -5 6.25 0 1 0.2 0.2 {layer=13}
T {@#0:net_name} 25 -33.75 0 0 0.15 0.15 {layer=15}
T {@#2:net_name} 25 23.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} -6.25 -12.5 0 1 0.15 0.15 {layer=15}
P 4 4 20 30 13.75 13.75 3.75 23.75 20 30 {fill=true}
]
C {gnd.sym} 740 -120 0 0 {name=l16 lab=VSS
embed=true}
C {lab_pin.sym} 590 -150 0 0 {name=p4 lab=RST
embed=true}
C {res.sym} 650 -150 1 0 {name=R6 m=1 value=20k footprint=1206
embed=true}
C {lab_pin.sym} 90 -240 0 1 {name=p5 lab=RST
embed=true}
C {npn.sym} 790 -200 0 0 {name=Q2 model=q2n2222
url="http://html.alldatasheet.com/html-pdf/17919/PHILIPS/MMBT2222/499/2/MMBT2222.html"
embed=true}
C {gnd.sym} 810 -170 0 0 {name=l11 lab=VSS
embed=true}
C {res.sym} 810 -280 2 0 {name=R1 m=1 value=20k footprint=1206
embed=true}
C {vdd.sym} 810 -310 0 0 {name=l13 lab=VCC
embed=true}
C {launcher.sym} 370 -520 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/pcb_current_protection_embed.raw tran"
embed=true}
[
v {xschem version=3.1.0 file_version=1.2}
K {type=launcher
format="** @descr : @url"
verilog_ignore=true
vhdl_ignore=true
spice_ignore=true
template="name=h1
descr=google 
url=www.google.com"}
G {}
V {}
S {}
E {}
T {@descr} 10 0 0 0 0.4 0.4 {vcenter=true}
P 4 9 -0 -0 -30 -12.5 -30 -5 -60 -5 -50 0 -60 5 -30 5 -30 12.5 -0 0 {fill=true}
]
