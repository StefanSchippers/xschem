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
N 770 -490 770 -390 {lab=VCC}
N 600 -360 730 -360 {lab=G}
N 770 -300 860 -300 {lab=VOUT}
N 770 -160 770 -130 {lab=VSS}
N 770 -300 770 -220 {lab=VOUT}
N 330 -310 330 -220 {lab=Z}
N 330 -390 560 -390 {lab=B}
N 330 -160 330 -130 {lab=VSS}
N 330 -430 330 -390 {lab=B}
N 600 -490 600 -420 {lab=VCC}
N 600 -260 600 -220 {lab=#net1}
N 600 -160 600 -130 {lab=VSS}
N 330 -390 330 -370 {lab=B}
N 770 -330 770 -300 {lab=VOUT}
N 600 -360 600 -320 {lab=G}
N 250 -160 250 -130 {lab=VSS}
N 250 -220 330 -220 {lab=Z}
N 550 -320 650 -320 {lab=G}
N 550 -260 650 -260 {lab=#net1}
C {conn_3x1.sym} 150 -360 0 0 {name=C1 embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=connector
format="*connector(3,1) @pinlist"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=c1 footprint=connector(3,1)"
}
V {}
S {}
E {}
B 5 18.75 -21.25 21.25 -18.75 {name=conn_1 dir=inout pinnumber=1}
B 5 18.75 -1.25 21.25 1.25 {name=conn_2 dir=inout pinnumber=2}
B 5 18.75 18.75 21.25 21.25 {name=conn_3 dir=inout pinnumber=3}
A 4 15 -20 5 270 360 {}
A 4 15 0 5 270 360 {}
A 4 15 20 5 270 360 {}
T {@#0:pinnumber} 7.5 -23.75 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} 7.5 -3.75 0 1 0.2 0.2 {layer=13}
T {@#2:pinnumber} 7.5 16.25 0 1 0.2 0.2 {layer=13}
T {@name} -18.75 -43.75 0 0 0.2 0.2 {}
P 4 5 10 30 -10 30 -10 -30 10 -30 10 30 {}
]
C {vdd.sym} 770 -490 0 0 {name=l6 lab=VCC embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="H"
global=true
format="*.alias @lab"
template="name=l1 lab=VDD"}
V {}
S {}
E {}
L 4 0 -20 0 0 {}
L 4 -10 -20 10 -20 {}
B 5 -2.5 -2.5 2.5 2.5 {name=p dir=inout verilog_type=wire goto=0}
T {@lab} -12.5 -35 0 0 0.2 0.2 {}
]
C {lab_pin.sym} 860 -300 0 1 {name=p0 lab=VOUT embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
]
C {lab_wire.sym} 660 -360 0 0 {name=l9 lab=G embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -2.5 -1.25 2 0 0.27 0.27 {}
]
C {res.sym} 770 -190 0 0 {name=Rload m=1 value=100 footprint=1206 device=resistor
tedax_ignore=true embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=resistor

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
C {gnd.sym} 770 -130 0 0 {name=l10 lab=VSS embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="L"
global=true
format="*.alias @lab"
template="name=l1 lab=GND"}
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
C {code.sym} 950 -430 0 0 {name=STIMULI
tedax_ignore=true
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.option PARHIER=LOCAL RUNLVL=5 post MODMONTE=1 warn maxwarns=400 ingold=1

vvcc vcc 0 dc 0 pwl 0 0 1m 6 2m 6 3m 5 4m 5 5m 0
vvss vss 0 dc 0
* .tran 5u 7m uic
.dc VVCC 0 8 0.004
.save all
" embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
format="
@value
"}
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
C {pnp.sym} 580 -390 0 0 {name=Q6 model=BC857 device=BC857 area=1 footprint=SOT23
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwijlfagu4zfAhUN0xoKHTPBAb0QFjAAegQIAhAC&url=http%3A%2F%2Fwww.onsemi.com%2Fpub%2FCollateral%2FPN2907-D.PDF&usg=AOvVaw2wgr87fGZgGfBRhXzHGwZM" embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=pnp
format="@spiceprefix@name @pinlist @model area=@area m=@m"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=Q1
model=Q2N2907
device=2N2907
footprint=TO92
area=1
m=1"}
V {}
S {}
E {}
L 4 0 -30 0 30 {}
L 4 -20 0 0 0 {}
L 4 10 -20 20 -30 {}
L 4 0 10 20 30 {}
B 5 17.5 27.5 22.5 32.5 {name=C dir=inout pinnumber=3}
B 5 -22.5 -2.5 -17.5 2.5 {name=B dir=in pinnumber=1}
B 5 17.5 -32.5 22.5 -27.5 {name=E dir=inout pinnumber=2}
T {@model} 20 -12.5 0 0 0.2 0.2 {}
T {@name} 20 0 0 0 0.2 0.2 {}
T {@#2:pinnumber} 25 -25 0 0 0.2 0.2 {layer=13}
T {@#0:pinnumber} 25 12.5 0 0 0.2 0.2 {layer=13}
T {@#1:pinnumber} -5 6.25 0 1 0.2 0.2 {layer=13}
T {@#2:net_name} 25 -33.75 0 0 0.15 0.15 {layer=15}
T {@#0:net_name} 25 23.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} -6.25 -12.5 0 1 0.15 0.15 {layer=15}
P 4 4 0 -10 15 -15 5 -25 0 -10 {fill=true}
]
C {zener.sym} 330 -190 2 0 {name=x3 model=BZX5V1 device=BZX5V1 area=1 footprint=acy(300) embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=diode
format="@spiceprefix@name @pinlist @model"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=X1 model=XXX device=XXX"}
V {}
S {}
E {}
L 4 0 5 0 30 {}
L 4 0 -30 0 -5 {}
L 4 -20 5 20 5 {}
L 4 20 -5 20 5 {}
L 4 -20 5 -20 15 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=p dir=inout pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=m dir=inout pinnumber=2}
T {@#0:pinnumber} -5 -26.25 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} -5 17.5 0 1 0.2 0.2 {layer=13}
T {@name} 15 -18.75 0 0 0.2 0.2 {}
T {@#0:net_name} 10 -28.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 10 20 0 0 0.15 0.15 {layer=15}
T {@model} 15 6.25 0 0 0.2 0.2 {}
P 4 4 -0 5 -10 -5 10 -5 -0 5 {fill=true}
]
C {gnd.sym} 330 -130 0 0 {name=l13 lab=VSS embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="L"
global=true
format="*.alias @lab"
template="name=l1 lab=GND"}
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
C {res.sym} 330 -460 0 0 {name=R4 m=1 value=4.7K footprint=1206 device=resistor embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=resistor

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
C {vdd.sym} 330 -490 0 0 {name=l14 lab=VCC embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="H"
global=true
format="*.alias @lab"
template="name=l1 lab=VDD"}
V {}
S {}
E {}
L 4 0 -20 0 0 {}
L 4 -10 -20 10 -20 {}
B 5 -2.5 -2.5 2.5 2.5 {name=p dir=inout verilog_type=wire goto=0}
T {@lab} -12.5 -35 0 0 0.2 0.2 {}
]
C {vdd.sym} 600 -490 0 0 {name=l15 lab=VCC embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="H"
global=true
format="*.alias @lab"
template="name=l1 lab=VDD"}
V {}
S {}
E {}
L 4 0 -20 0 0 {}
L 4 -10 -20 10 -20 {}
B 5 -2.5 -2.5 2.5 2.5 {name=p dir=inout verilog_type=wire goto=0}
T {@lab} -12.5 -35 0 0 0.2 0.2 {}
]
C {res.sym} 600 -190 0 0 {name=R5 m=1 value=470 footprint=1206 device=resistor embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=resistor

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
C {gnd.sym} 600 -130 0 0 {name=l16 lab=VSS embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="L"
global=true
format="*.alias @lab"
template="name=l1 lab=GND"}
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
C {lab_wire.sym} 360 -390 0 0 {name=l0 lab=B embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -2.5 -1.25 2 0 0.27 0.27 {}
]
C {res.sym} 330 -340 0 0 {name=R2 m=1 value=510 footprint=1206 device=resistor embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=resistor

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
C {pmos.sym} 750 -360 0 0 {name=M2 m=1 model=IRLML6402 device=IRLML6402 footprint=SOT23
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=2ahUKEwjs8pzxuozfAhWpz4UKHR4CDnMQFjAAegQIAhAC&url=https%3A%2F%2Fwww.infineon.com%2Fdgdl%2Firlml6402.pdf%3FfileId%3D5546d462533600a401535668c9822638&usg=AOvVaw21fCRax-ssVpLqDeGK8KiC" embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=pmos
format="@spiceprefix@name @pinlist @model @extra m=@m"
verilog_format="@symname #@del @name ( @@d , @@s , @@g );"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=M1 
model=DMP2035U 
device=DMP2035U 
m=1"
}
V {}
S {}
E {}
L 4 5 -27.5 5 27.5 {}
L 4 5 20 20 20 {}
L 4 20 20 20 30 {}
L 4 5 -20 20 -20 {}
L 4 20 -30 20 -20 {}
L 4 -5 -15 -5 15 {}
L 4 -20 0 -12.5 0 {}
B 5 17.5 27.5 22.5 32.5 {name=d dir=inout pinnumber=3}
B 5 -22.5 -2.5 -17.5 2.5 {name=g dir=in pinnumber=1}
B 5 17.5 -32.5 22.5 -27.5 {name=s dir=inout pinnumber=2}
A 4 -8.75 0 3.75 270 360 {}
T {@device} 18.75 -13.75 0 0 0.2 0.2 {}
T {@name} 18.75 0 0 0 0.2 0.2 {}
T {D} 7.5 8.75 0 0 0.2 0.2 {}
T {@#2:pinnumber} 25 -28.75 0 0 0.2 0.2 {layer=13}
T {@#0:pinnumber} 25 18.75 0 0 0.2 0.2 {layer=13}
T {@#1:pinnumber} -13.75 6.25 0 1 0.2 0.2 {layer=13}
]
C {led.sym} 650 -290 0 0 {name=x1 model=D1N5765 device=D1N5765 area=1 footprint=acy(300) embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=diode
format="@spiceprefix@name @pinlist @model"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=X1 model=XXX device=XXX"}
V {}
S {}
E {}
L 4 0 5 0 30 {}
L 4 0 -30 0 -5 {}
L 4 -20 5 20 5 {}
L 4 -30 -2.5 -20 -12.5 {}
L 4 -30 -12.5 -30 -2.5 {}
L 4 -45 2.5 -30 -12.5 {}
L 4 -35 10 -25 0 {}
L 4 -35 0 -35 10 {}
L 4 -50 15 -35 0 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=p dir=inout pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=m dir=inout pinnumber=2}
T {@name} 7.5 -20 0 0 0.2 0.2 {}
T {@value} 7.5 12.5 0 0 0.2 0.2 {}
T {@#0:pinnumber} -5 -26.25 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} -5 16.25 0 1 0.2 0.2 {layer=13}
T {@#0:net_name} 5 -42.5 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 5 32.5 0 0 0.15 0.15 {layer=15}
P 4 4 -0 5 -10 -5 10 -5 -0 5 {fill=true}
]
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan" embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=logo
template="name=l1 author=\\"Stefan Schippers\\""
verilog_ignore=true
vhdl_ignore=true
spice_ignore=true
tedax_ignore=true}
V {}
S {}
E {}
L 6 225 0 1020 0 {}
L 6 -160 0 -95 0 {}
T {@schname} 235 5 0 0 0.4 0.4 {}
T {@author} 235 -25 0 0 0.4 0.4 {}
T {@time_last_modified} 1020 -25 0 1 0.4 0.3 {}
T {SCHEM} 5 -25 0 0 1 1 {}
P 5 13 5 -30 -25 0 5 30 -15 30 -35 10 -55 30 -75 30 -45 0 -75 -30 -55 -30 -35 -10 -15 -30 5 -30 {fill=true}
]
C {lab_pin.sym} 170 -340 0 1 {name=p6 lab=VOUT embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
]
C {lab_pin.sym} 170 -360 0 1 {name=p7 lab=VSS embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
]
C {lab_pin.sym} 170 -380 0 1 {name=p8 lab=VCC embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
]
C {zener.sym} 250 -190 2 0 {name=x4 model=BZX5V1 device=BZX5V1 area=1 footprint=minimelf spice_ignore=true embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=diode
format="@spiceprefix@name @pinlist @model"

tedax_format="footprint @name @footprint
value @name @value
device @name @device
spicedev @name @spicedev
spiceval @name @spiceval
comptag @name @comptag"

template="name=X1 model=XXX device=XXX"}
V {}
S {}
E {}
L 4 0 5 0 30 {}
L 4 0 -30 0 -5 {}
L 4 -20 5 20 5 {}
L 4 20 -5 20 5 {}
L 4 -20 5 -20 15 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=p dir=inout pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=m dir=inout pinnumber=2}
T {@#0:pinnumber} -5 -26.25 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} -5 17.5 0 1 0.2 0.2 {layer=13}
T {@name} 15 -18.75 0 0 0.2 0.2 {}
T {@#0:net_name} 10 -28.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 10 20 0 0 0.15 0.15 {layer=15}
T {@model} 15 6.25 0 0 0.2 0.2 {}
P 4 4 -0 5 -10 -5 10 -5 -0 5 {fill=true}
]
C {gnd.sym} 250 -130 0 0 {name=l1 lab=VSS embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
function0="L"
global=true
format="*.alias @lab"
template="name=l1 lab=GND"}
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
C {res.sym} 550 -290 0 0 {name=R1 m=1 value=47K footprint=1206 device=resistor embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=resistor

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
C {lab_wire.sym} 330 -260 0 0 {name=l3 lab=Z embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -2.5 -1.25 2 0 0.27 0.27 {}
]
C {code.sym} 950 -260 0 0 {name=MODELS 
tedax_ignore=true
only_toplevel=true
value=".MODEL bd139_2 npn
+IS=1e-09 BF=222.664 NF=0.85 VAF=36.4079
+IKF=0.166126 ISE=5.03418e-09 NE=1.45313 BR=1.35467
+NR=1.33751 VAR=142.931 IKR=1.66126 ISC=5.02557e-09
+NC=3.10227 RB=26.9143 IRB=0.1 RBM=0.1
+RE=0.000472454 RC=1.04109 XTB=0.727762 XTI=1.04311
+EG=1.05 CJE=1e-11 VJE=0.75 MJE=0.33
+TF=1e-09 XTF=1 VTF=10 ITF=0.01
+CJC=1e-11 VJC=0.75 MJC=0.33 XCJC=0.9
+FC=0.5 CJS=0 VJS=0.75 MJS=0.5
+TR=1e-07 PTF=0 KF=0 AF=1

*SRC=BC817-16;DI_BC817-16;BJTs NPN; Si;  45.0V  0.800A  220MHz   Diodes Inc. BJTs
.MODEL BC817  NPN (IS=4.04n NF=1.00 BF=342 VAF=121
+ IKF=0.273 ISE=6.86n NE=2.00 BR=4.00 NR=1.00
+ VAR=20.0 IKR=0.675 RE=0.227 RB=0.907 RC=90.7m
+ XTB=1.5 CJE=107p VJE=1.10 MJE=0.500 CJC=34.7p VJC=0.300 MJC=0.300 
+ TF=651p TR=111n EG=1.12 )

.MODEL BD139 NPN (
+IS=2.3985E-13 Vceo=80 Icrating=1500m
+BF=244.9 NF=1.0 BR=78.11 NR=1.007 ISE=1.0471E-14
+NE=1.2 ISC=1.9314E-11 NC=1.45 VAF=98.5 VAR=7.46
+IKF=1.1863 IKR=0.1445 RB=2.14 RBM=0.001 IRB=0.031
+RE=0.0832 RC=0.01 CJE=2.92702E-10 VJE=0.67412
+MJE=0.3300 FC=0.5 CJC=4.8831E-11 VJC=0.5258
+MJC=0.3928 XCJC=0.5287 XTB=1.1398 EG=1.2105 XTI=3.0) 


.MODEL BD140  PNP(IS=1e-09 BF=650.842 NF=0.85 VAF=10
+IKF=0.0950125 ISE=1e-08 NE=1.54571 BR=56.177
+NR=1.5 VAR=2.11267 IKR=0.950125 ISC=1e-08
+NC=3.58527 RB=41.7566 IRB=0.1 RBM=0.108893
+RE=0.000347052 RC=1.32566 XTB=19.5239 XTI=1
+EG=1.05 CJE=1e-11 VJE=0.75 MJE=0.33
+TF=1e-09 XTF=1 VTF=10 ITF=0.01
+CJC=1e-11 VJC=0.75 MJC=0.33 XCJC=0.9
+FC=0.5 CJS=0 VJS=0.75 MJS=0.5 TR=1e-07 PTF=0 KF=0 AF=10
+ VCEO=80 ICRATING=1A )


.MODEL Q2N2222A NPN IS =3.0611E-14 NF =1.00124 BF =220 IKF=0.52 
+              VAF=104 ISE=7.5E-15 NE =1.41 NR =1.005 BR =4 IKR=0.24 
+              VAR=28 ISC=1.06525E-11 NC =1.3728 RB =0.13 RE =0.22 
+              RC =0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 
+              CJE=27.01E-12 TF =0.325E-9 TR =100E-9
+              vce_max=45 vbe_max=6

.MODEL Q2N2222 NPN (
+ IS = 3.97589E-14 BF = 195.3412 NF = 1.0040078 VAF = 53.081 IKF = 0.976 ISE = 1.60241E-14
+ NE = 1.4791931 BR = 1.1107942 NR = 0.9928261 VAR = 11.3571702 IKR = 2.4993953 ISC = 1.88505E-12
+ NC = 1.1838278 RB = 56.5826472 IRB = 1.50459E-4 RBM = 5.2592283 RE = 0.0402974 RC = 0.4208
+ CJE = 2.56E-11 VJE = 0.682256 MJE = 0.3358856 TF = 3.3E-10 XTF = 6 VTF = 0.574
+ ITF = 0.32 PTF = 25.832 CJC = 1.40625E-11 VJC = 0.5417393 MJC = 0.4547893 XCJC = 1
+ TR = 3.2E-7 CJS = 0 VJS = .75 MJS = 0 XTB = 1.6486 EG = 1.11
+ XTI = 5.8315 KF = 0 AF = 1 FC = 0.83
+              vce_max=45 vbe_max=6 )


* 2n2222 p complementary
.MODEL Q2N2907P PNP(IS=650.6E-18 ISE=54.81E-15 ISC=0 XTI=3
+ BF=231.7 BR=3.563 IKF=1.079 IKR=0 XTB=1.5
+ VAF=115.7 VAR=35 VJE=0.65 VJC=0.65
+ RE=0.15 RC=0.715 RB=10
+ CJE=19.82E-12 CJC=14.76E-12 XCJC=0.75 FC=0.5
+ NF=1 NR=1 NE=1.829 NC=2 MJE=0.3357 MJC=0.5383
+ TF=603.7E-12 TR=111.3E-9 ITF=0.65 VTF=5 XTF=1.7
+ EG=1.11 KF=1E-9 AF=1
+ VCEO=45 ICRATING=100M 
+              vce_max=45 vbe_max=6 )

.MODEL Q2N2907S PNP(IS=2.32E-13 ISE=6.69E-16 ISC=1.65E-13 XTI=3.00
+ BF=3.08E2 BR=2.18E1 IKF=8.42E-1 IKR=1.00 XTB=1.5
+ VAF=1.41E2 VAR=1.35E1 VJE=3.49E-1 VJC=3.00E-1
+ RE=1.00E-2 RC=8.46E-1 RB=4.02E1 RBM=1.00E-2 IRB=1.25E-2
+ CJE=2.66E-11 CJC=1.93E-11 XCJC=1.00 FC=5.00E-1
+ NF=1.04 NR=1.12 NE=1.09 NC=1.13 MJE=4.60E-1 MJC=4.65E-1
+ TF=4.95E-10 TR=0 PTF=0 ITF=3.36E-1 VTF=6.54 XTF=1.87E1
+ EG=1.11 KF=1E-9 AF=1
+ VCEO=40 ICRATING=500M 
+              vce_max=45 vbe_max=6 )





* PINOUT ORDER +IN -IN +V -V OUT
* PINOUT ORDER  1   2   3  4  5
.SUBCKT LM358 1 2 3 4 5
R44 4 6 4E4
I1 4 7 0.5E-6
Q1 4 8 9 QPI
Q2 4 2 10 QPA
Q3 9 9 11 QPI
Q4 10 10 11 QPI
Q5 12 13 4 QNQ
Q6 13 13 4 QNQ
Q7 4 12 14 QPQ
Q8 3 14 6 QNQ
Q9 15 6 4 QNQ
Q10 3 15 16 QNQ
Q11 3 16 17 QNQ
R67 17 16 4E4
R68 5 17 18
Q12 4 15 5 QPQ
Q13 15 17 5 QNQ
I2 18 3 120E-9
I3 19 3 60E-9
I4 20 3 1E-6
Q14 11 18 3 QPQ
Q15 14 19 3 QPQ
Q16 5 7 4 QNQ
Q17 15 20 3 QPQ
C15 21 22 4.8E-12
R69 12 21 3
R70 12 15 3E9
E2 23 8 3 0 -10E-6
V51 23 1 1.56E-3
I6 3 4 5E-6
R71 4 3 4.5E5
Q18 12 9 11 QPQ
Q19 13 10 11 QPQ
C17 12 13 8E-12
C18 6 15 1E-12
C21 3 24 100E-15
R78 11 24 3E5
C22 1 2 0.23E-12
C23 2 0 0.79E-12
C24 1 0 0.79E-12
E3 22 0 15 0 2
C25 5 0 50E-15
Q20 25 25 0 QNQ
G1 3 4 VT 0 3E-4
I7 0 25 1E-3
V53 25 26 0.25
R79 0 26 1E6
E4 VT 0 27 26 1
R80 0 VT 1E6
V54 27 0 0.55
R81 0 27 1E6
.MODEL QPQ PNP IKF=3E-3 RC=300 KF=4.8E-17 BR=1
.MODEL QPA PNP IKF=3E-3 RC=380 IS=1.01E-16 VAF=245 RE=5 RB=1700 BF=300 KF=4.8E-17 BR=1
.MODEL QPI PNP IKF=3E-3 RC=380 IS=1.01E-16 VAF=290 RE=5 RB=1700 BF=306 KF=4.8E-17 BR=1
.MODEL QNQ NPN IKF=5E-3 RC=25  KF=4.8E-17 BR=1
.ENDS
* END SPICE MODEL LM358

.SUBCKT irlml6402 1 2 3
* Node 1 -> Drain
* Node 2 -> Gate
* Node 3 -> Source
M1 9 7 8 8 MM L=100u W=100u
.MODEL MM PMOS LEVEL=1 IS=1e-32
+VTO=-1 LAMBDA=0.0111358 KP=12.788
+CGSO=5.36099e-06 CGDO=5.54234e-08
RS 8 3 0.0246704
D1 1 3 MD
.MODEL MD D IS=2.03395e-08 RS=0.0432758 N=1.5 BV=20
+IBV=0.00025 EG=1 XTI=4 TT=1e-07
+CJO=1.11974e-10 VJ=0.5 M=0.3 FC=0.5
RDS 3 1 5e+07
RD 9 1 0.0001
RG 2 7 29.2227
D2 5 4 MD1
* Default values used in MD1:
*   RS=0 EG=1.11 XTI=3.0 TT=0
*   BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=1.68841e-10 VJ=1.50027 M=0.3 FC=1e-08
D3 5 0 MD2
* Default values used in MD2:
*   EG=1.11 XTI=3.0 TT=0 CJO=0
*   BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=0.4 RS=3e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 9.68769e-10
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 6 0 MD3
* Default values used in MD3:
*   EG=1.11 XTI=3.0 TT=0 CJO=0
*   RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=0.4
.ENDS irlml6402

.SUBCKT irf5305s 1 2 3
* Node 1 -> Drain
* Node 2 -> Gate
* Node 3 -> Source
M1 9 7 8 8 MM L=100u W=100u
* Default values used in MM:
* The voltage-dependent capacitances are
* not included. Other default values are:
*   RS=0 RD=0 LD=0 CBD=0 CBS=0 CGBO=0
.MODEL MM PMOS LEVEL=1 IS=1e-32
+VTO=-3.45761 LAMBDA=0 KP=10.066
+CGSO=1.03141e-05 CGDO=1e-11
RS 8 3 0.0262305
D1 1 3 MD
.MODEL MD D IS=8.90854e-09 RS=0.00921723 N=1.5 BV=55
+IBV=0.00025 EG=1.2 XTI=3.13635 TT=1e-07
+CJO=1.30546e-09 VJ=0.663932 M=0.419045 FC=0.5
RDS 3 1 2.2e+06
RD 9 1 0.0001
RG 2 7 11.0979
D2 5 4 MD1
* Default values used in MD1:
*   RS=0 EG=1.11 XTI=3.0 TT=0
*   BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=1.63729e-09 VJ=0.791199 M=0.552278 FC=1e-08
D3 5 0 MD2
* Default values used in MD2:
*   EG=1.11 XTI=3.0 TT=0 CJO=0
*   BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=0.400245 RS=3e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 1.89722e-09
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 6 0 MD3
* Default values used in MD3:
*   EG=1.11 XTI=3.0 TT=0 CJO=0
*   RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=0.400245
.ENDS






*MM3725 MCE 5/13/95
*Si    1W  40V 500mA 307MHz pkg:TO-39 3,2,1
.MODEL QMM3725 NPN (IS=50.7F NF=1 BF=162 VAF=113 IKF=.45 ISE=38.2P NE=2
+ BR=4 NR=1 VAR=24 IKR=.675 RE=.263 RB=1.05 RC=.105 XTB=1.5
+ CJE=62.2P VJE=1.1 MJE=.5 CJC=14.6P VJC=.3 MJC=.3 TF=518P TR=45.9N)

*MPS651 MCE 5/12/95
*Si 625mW  60V    2A 110MHz pkg:TO-92 1,2,3
.MODEL QMPS651 NPN (IS=72.1F NF=1 BF=260 VAF=139 IKF=2.5 ISE=67.1P NE=2
+ BR=4 NR=1 VAR=20 IKR=3.75 RE=28.2M RB=.113 RC=11.3M XTB=1.5
+ CJE=212P VJE=1.1 MJE=.5 CJC=68.6P VJC=.3 MJC=.3 TF=1.44N TR=1U)

*FCX649 ZETEX Spice model   Last revision  17/7/90   Medium Power
*ZTX Si  1.5W  25V    2A 240MHz pkg:SOT-89 2,1,3
.MODEL QFCX649 NPN(IS=3E-13 BF=225 VAF=80 IKF=2.8 ISE=1.1E-13 NE=1.37 
+ BR=110 NR=0.972 VAR=28 IKR=0.8 ISC=6.5E-13 NC=1.372 RB=0.3 RE=0.063 
+ RC=0.07 CJE=3.25E-10 TF=1E-9 CJC=7E-11 TR=1E-8 )

*MPSW01A MCE 5/12/95
*Si    1W  40V    1A 210MHz pkg:TO-92 1,2,3
.MODEL QMPSW01A NPN (IS=18.1F NF=1 BF=273 VAF=113 IKF=.6 ISE=15.7P NE=2
+ BR=4 NR=1 VAR=20 IKR=.9 RE=96.5M RB=.386 RC=38.6M XTB=1.5
+ CJE=78.7P VJE=1.1 MJE=.5 CJC=17.3P VJC=.3 MJC=.3 TF=757P TR=526N)

*BC546 ZETEX Spice model     Last revision  4/90   General Purpose
*ZTX Si 500mW  65V 200mA 300MHz pkg:TO-92 1,2,3
.MODEL BC546 NPN(IS=1.8E-14 BF=400 NF=0.9955 VAF=80 IKF=0.14 ISE=5E-14 
+ NE=1.46 BR=35.5 NR=1.005 VAR=12.5 IKR=0.03 ISC=1.72E-13 NC=1.27 RB=0.56 
+ RE=0.6 RC=0.25 CJE=1.3E-11 TF=6.4E-10 CJC=4E-12 VJC=0.54 TR=5.072E-8 
+     vce_max=65 vbe_max=6 )

.MODEL BC857 PNP(
+       AF= 1.00E+00    BF= 1.96E+02    BR= 3.38E+00   CJC= 9.80E-12
+      CJE= 1.56E-11   CJS= 0.00E+00    EG= 1.11E+00    FC= 8.28E-01
+      IKF= 8.35E-01   IKR= 1.60E-02   IRB= 1.31E-06    IS= 1.32E-14
+      ISC= 7.71E-14   ISE= 8.44E-13   ITF= 2.14E-01    KF= 0.00E+00
+      MJC= 3.49E-01   MJE= 4.18E-01   MJS= 3.30E-01    NC= 1.19E+00
+       NE= 1.83E+00    NF= 1.00E+00    NR= 1.00E+00   PTF= 0.00E+00
+       RB= 1.00E-02   RBM= 1.00E-02    RC= 1.52E+00    RE= 3.00E-02
+       TF= 6.05E-10    TR= 0.00E+00   VAF= 5.90E+01   VAR= 1.74E+01
+      VJC= 3.00E-01   VJE= 8.00E-01   VJS= 7.50E-01   VTF= 4.39E+00
+     XCJC= 1.00E+00   XTB= 0.00E+00   XTF= 5.81E+00   XTI= 3.00E+00)

.MODEL BC556 PNP( 
+     IS=2.059E-14
+     NF=1.003
+     ISE=2.971E-15
+     NE=1.316
+     BF=227.3
+     IKF=0.08719
+     VAF=37.2
+     NR=1.007
+     ISC=1.339E-14
+     NC=1.15
+     BR=7.69
+     IKR=0.07646
+     VAR=11.42
+     RB=1
+     IRB=1E-06
+     RBM=1
+     RE=0.688
+     RC=0.6437
+     XTB=0
+     EG=1.11
+     XTI=3
+     CJE=1.4E-11
+     VJE=0.5912
+     MJE=0.3572
+     TF=7.046E-10
+     XTF=4.217
+     VTF=5.367
+     ITF=0.1947
+     PTF=0
+     CJC=1.113E-11
+     VJC=0.1
+     MJC=0.3414
+     XCJC=0.6288
+     TR=1E-32
+     CJS=0
+     VJS=0.75
+     MJS=0.333
+     FC=0.7947
+     vce_max=65 vbe_max=6 )
*

* NXP Semiconductors
*
* Medium power NPN transistor
* IC   = 1 A
* VCEO = 20 V 
* hFE  = 85 - 375 @ 2V/500mA
* 
*
*
*
* Package pinning does not match Spice model pinning.
* Package: SOT 223
* 
* Package Pin 1: Base  
* Package Pin 2: Collector
* Package Pin 3: Emitter
* Package Pin 4: Collector
* 
* Extraction date (week/year): 13/2008 
* Simulator: Spice 3 
*
**********************************************************
*#
.SUBCKT BCP68 1 2 3
* 
Q1 1 2 3 BCP68 
D1 2 1 DIODE
*
*The diode does not reflect a 
*physical device but improves 
*only modeling in the reverse 
*mode of operation.
*
.MODEL BCP68 NPN 
+ IS = 2.312E-013 
+ NF = 0.988 
+ ISE = 8.851E-014 
+ NE = 2.191 
+ BF = 273 
+ IKF = 5.5 
+ VAF = 50 
+ NR = 0.9885 
+ ISC = 6.808E-014 
+ NC = 3 
+ BR = 155.6 
+ IKR = 4 
+ VAR = 17 
+ RB = 15 
+ IRB = 2E-006 
+ RBM = 0.65 
+ RE = 0.073 
+ RC = 0.073 
+ XTB = 0 
+ EG = 1.11 
+ XTI = 3 
+ CJE = 2.678E-010 
+ VJE = 0.732 
+ MJE = 0.3484 
+ TF = 5.8E-010 
+ XTF = 1.5 
+ VTF = 2.5 
+ ITF = 1 
+ PTF = 0 
+ CJC = 3.406E-011 
+ VJC = 2 
+ MJC = 0.3142 
+ XCJC = 1 
+ TR = 6.5E-009 
+ CJS = 0 
+ VJS = 0.75 
+ MJS = 0.333 
+ FC = 0.95 
.MODEL DIODE D 
+ IS = 2.702E-015 
+ N = 1.2 
+ BV = 1000 
+ IBV = 0.001 
+ RS = 0.1 
+ CJO = 0 
+ VJ = 1 
+ M = 0.5 
+ FC = 0 
+ TT = 0 
+ EG = 1.11 
+ XTI = 3 
.ENDS

.SUBCKT   D1N5765       2   99
*                      |    |
*                      |    |
*                      |    CATHODE
*                      ANODE
I1  99  4   7.0
D1  2   99   DLOW
D2  2   4   DHIGH
R1  4   99  .1   TC=-6.27E-3,-2.33E-7
*
.MODEL DLOW     D       (
+         IS = 1.0E-15
+         RS = 100
+          N = 2.15
+         TT = 10.0E-09
+        CJO = 8.285237E-11
+         VJ = 1.2076937
+          M = 0.4053107
+         EG = 1.664
+        XTI = 10.78
+         KF = 0
+         AF = 1
+         FC = 0.4340008
+         BV = 5.0
+        IBV = 1E-4
+ )
.MODEL DHIGH    D       (
+         IS = 9.0E-15
+         RS = 0.30
+          N = 1.2
+         TT = 0
+        CJO = 0
+         VJ = 1
+          M = .5
+         EG = 0.1
+        XTI = -3.84
+         KF = 0
+         AF = 1
+         FC = .5
+         BV = 9.9999E+13
+        IBV = .001
+ )
.ENDS

.model D1N4007 D(IS=7.02767n RS=0.0341512 N=1.80803 EG=1.05743 XTI=5 BV=1000 IBV=5e-08 CJO=1e-11 VJ=0.7 M=0.5 FC=0.5 TT=1e-07 )

.MODEL D1N4148 D 
+ IS = 4.352E-9 
+ N = 1.906 
+ BV = 110 
+ IBV = 0.0001 
+ RS = 0.6458 
+ CJO = 7.048E-13 
+ VJ = 0.869 
+ M = 0.03 
+ FC = 0.5 
+ TT = 3.48E-9 

.MODEL D1n5400 d
+IS=2.61339e-12 RS=0.0110501 N=1.20576 EG=0.6
+XTI=3.1271 BV=50 IBV=1e-05 CJO=1e-11
+VJ=0.7 M=0.5 FC=0.5 TT=1e-09
+KF=0 AF=1
* Model generated on October 12, 2003
* Model format: PSpice


*SYM=POWMOSP
.SUBCKT DMP2035U   10 20 30
*     TERMINALS:  D  G  S
M1   1  2  3  3  PMOS L=0.6U W=0.9
RD  10  1  15m
RS  30  3  4m
RG  20  2  9.45
CGS  2  3  1.42n
EGD 12  30  2  1  1
VFB 14  30  0
FFB  2  1  VFB  1
CGD 13 14  598p
R1  13  30  1.00
D1  13 12  DLIM
DDG 14 15  DCGD
R2  12 15  1.00
D2  30  15  DLIM
DSD 10  3  DSUB
.MODEL PMOS PMOS LEVEL=3 U0=300 VMAX=40k
+ ETA=0.1m VTO=-1.02 TOX=16.8n NSUB=2e17
.MODEL DCGD D CJO=598p VJ=0.150 M=0.340
.MODEL DSUB D IS=36.1n N=1.50 RS=21.8m BV=20
+ CJO=40p VJ=0.500 M=0.450
.MODEL DLIM D IS=100U
.ENDS


*March 14, 2005
*Doc. ID: 77621, S-50395, Rev. A
.SUBCKT Si2306BDS 4 1 2
M1  3 1 2 2 NMOS W=573319u L=0.50u  
M2  2 1 2 4 PMOS W=573319u L=0.35u  
R1  4 3     RTEMP 15E-3
CGS 1 2     110E-12
DBD 2 4     DBD
**************************************************************** 
.MODEL  NMOS        NMOS ( LEVEL  = 3           TOX    = 5E-8
+ RS     = 19.5E-3         RD     = 0           NSUB   = 1.7E17   
+ kp     = 3.3E-5          UO     = 650             
+ VMAX   = 0               XJ     = 5E-7        KAPPA  = 5E-1
+ ETA    = 1E-4            TPG    = 1  
+ IS     = 0               LD     = 0                             
+ CGSO   = 0               CGDO   = 0           CGBO   = 0 
+ TLEV   = 1               BEX    = -1.5        TCV    = 3.9E-3
+ NFS    = 0.8E12          DELTA  = 0.1)
**************************************************************** 
.MODEL  PMOS        PMOS ( LEVEL  = 3           TOX    = 5E-8
+NSUB    = 2E16            TPG    = -1)   
**************************************************************** 
.MODEL DBD D (CJO=95E-12 VJ=0.38 M=0.20 
+FC=0.1 IS=1E-12 TT=9.3E-8 N=1 BV=30.2)
**************************************************************** 
.MODEL RTEMP R (TC1=7E-3 TC2=5.5E-6)
**************************************************************** 
.ENDS


* Fairchild model
* 2N7002 ELECTRICAL MODEL (SOT-23 Single N-Ch DMOS)
* -----------------------
.SUBCKT  M2N7002  20  10  30
Rg     10    1    1
M1      2    1    3    3    DMOS    L=1u   W=1u
.MODEL DMOS NMOS (VTO='2.1*(-0.0016*TEMPER+1.04)'  KP=0.35  THETA=0.086 VMAX=2.2E5  LEVEL=3)
Cgs     1    3    60p
Rd     20    4    0.3 TC=0.0075
Dds     3    4    DDS
.MODEL     DDS    D(BV='60*(0.00072*TEMPER+0.982)'   M=0.36  CJO=23p   VJ=0.8)
Dbody   3   20    DBODY
.MODEL   DBODY    D(IS=1.4E-13   N=1   RS=40m   TT=100n)
Ra      4    2    0.4 TC=0.0075
Rs      3    5    10m
Ls      5    30   .5n
M2      1    8    6    6   INTER
E2      8    6    4    1   2
.MODEL   INTER    NMOS(VTO=0  KP=10   LEVEL=1)
Cgdmax  7    4    85p
Rcgd    7    4    10meg
Dgd     6    4    DGD
Rdgd    4    6    10meg
.MODEL     DGD    D(M=0.53   CJO=85p   VJ=0.12)
M3      7    9    1    1   INTER
E3      9    1    4    1   -2
.ENDS M2N7002

*SRC=1N4732A;DI_1N4732A;Diodes;Zener <=10V; 4.70V  1.00W   Diodes Inc.  Zener
*SYM=HZEN
.SUBCKT D1N4732A  1   2
*    Terminals    A   K
D1 1 2 DF
DZ 3 1 DR
VZ 2 3 2.99
.MODEL DF D ( IS=87.7p RS=0.620 N=1.10
+ CJO=214p VJ=0.750 M=0.330 TT=50.1n )
.MODEL DR D ( IS=17.5f RS=0.333 N=2.28 )
.ENDS

* Technology:        DISCRETE DEVICE			
* Device:            Zener Diode BZX 55C 5V1			
* Type:              Typical (nom)			
* Model established: 12.11.1996, by S.Reuter, TM1iC63-HN			
* Wafer:			
* Remarks:           Macro model			
* Revision:			
* Simulator:         PSPICE			
.SUBCKT BZX5V1 a c			

 DF a c DFOR			
 DR c a DREV			
 DB b a DBRE			
 EB c b POLY(1) d 0 3.6 1
 IB 0 d 1m
 RB 0 d 1k TC=3m

.MODEL DFOR D
 + IS =   1p  RS =  3.5  N  =  1.4  CJO= 178p
 + VJ = 610m  M  = 335m  FC = 700m  XTI=    3
 + EG =1.186

.MODEL DREV D
 + IS = 100f  N  =   30  XTI=    3  EG =1.186

.MODEL DBRE D
 + IS =  10f  RS =    6  N  =    1  XTI=    0
 + EG =1.186

.ENDS BZX5V1			

.model D1N751_2 D(Is=1.004f Rs=.5875 Ikf=0 N=1 Xti=3 Eg=1.11 Cjo=160p M=.5484 Vj=.75 Fc=.5 Isr=1.8n Nr=2 Bv=5.1 Ibv=27.721m Nbv=1.1779 Ibvl=1.1646m Nbvl=21.894 Tbv1=176.47u Vpk=5.1 )



*(+) 1N751     5.1 V  0.5W  CASE DO-35
.model D1N751  D(Is=1.004f Rs=.5875 Ikf=0 N=1 Xti=3 Eg=1.11 Cjo=160p M=.5484
+		Vj=.75 Fc=.5 Isr=1.8n Nr=2 Bv=5.1 Ibv=27.721m Nbv=1.1779
+		Ibvl=1.1646m Nbvl=21.894 Tbv1=176.47u)
*		Motorola	pid=1N751	case=DO-35
*		89-9-18-gjg
*		Vz = 5.1 @ 20mA, Zz = 175 @ 1mA, Zz = 8.2 @ 5mA, Zz = 2.2 @ 20mA

* 1N5230    4.7 Volt zener diode
.model D1N5230  D(Is=880.5E-18 Rs=.25 Ikf=0 N=1 Xti=3 Eg=1.11 Cjo=175p M=.5516
+               Vj=.75 Fc=.5 Isr=1.859n Nr=2 Bv=4.7 Ibv=20.245m Nbv=1.6989
+               Ibvl=1.9556m Nbvl=14.976 Tbv1=-21.28u)
*               Motorola        pid=1N5230      case=DO-35
*               89-9-18 gjg
*               Vz = 4.7 @ 20mA, Zz = 300 @ 1mA, Zz = 12.5 @ 5mA, Zz =2.6  @ 20mA

*1N758
*Ref: Motorola
*10V 500mW Si Zener pkg:DIODE0.4 1,2
.MODEL D1N758 D(IS=1E-11 RS=8.483 N=1.27 TT=5E-8 CJO=2.334E-10 VJ=0.75 
+ M=0.33 BV=9.83 IBV=0.01 )

*1N4744
*Ref: Motorola
*15V 1W Si Zener pkg:DIODE0.4 1,2
.MODEL D1N4744 D(IS=5.32E-14 RS=6.47 TT=5.01E-8 CJO=7.83E-11 M=0.33 
+ BV=14.89 IBV=0.017 )

*1N755
*Ref: Motorola
*7.5V 500mW Si Zener pkg:DIODE0.4 1,2
.MODEL D1N755 D(IS=1E-11 RS=3.359 N=1.27 TT=5E-8 CJO=2.959E-10 VJ=0.75 
+ M=0.33 BV=7.433 IBV=0.01 )

** node order
* 1: gnd
* 2: trig
* 3: out
* 4: reset#
* 5: ctrl
* 6: thres
* 7: dis
* 8: vcc
.SUBCKT ne555  34 32 30 19 23 33 1  21
*              G  TR O  R  F  TH D  V
Q4 25 2 3 QP
Q5 34 6 3 QP
Q6 6 6 8 QP
R1 9 21 4.7K
R2 3 21 830
R3 8 21 4.7K
Q7 2 33 5 QN
Q8 2 5 17 QN
Q9 6 4 17 QN
Q10 6 23 4 QN
Q11 12 20 10 QP
R4 10 21 1K
Q12 22 11 12 QP
Q13 14 13 12 QP
Q14 34 32 11 QP
Q15 14 18 13 QP
R5 14 34 100K
R6 22 34 100K
R7 17 34 10K
Q16 1 15 34 QN
Q17 15 19 31 QP
R8 18 23 5K
R9 18 34 5K
R10 21 23 5K
Q18 27 20 21 QP
Q19 20 20 21 QP
R11 20 31 5K
D1 31 24 DA
Q20 24 25 34 QN
Q21 25 22 34 QN
Q22 27 24 34 QN
R12 25 27 4.7K
R13 21 29 6.8K
Q23 21 29 28 QN
Q24 29 27 16 QN
Q25 30 26 34 QN
Q26 21 28 30 QN
D2 30 29 DA
R14 16 15 100
R15 16 26 220
R16 16 34 4.7K
R17 28 30 3.9K
Q3 2 2 9 QP
.MODEL DA D (RS=40 IS=1.0E-14 CJO=1PF)
.MODEL QP PNP (level=1 BF=20 BR=0.02 RC=4 RB=25 IS=1.0E-14 VA=50 NE=2)
+ CJE=12.4P VJE=1.1 MJE=.5 CJC=4.02P VJC=.3 MJC=.3 TF=229P TR=159N)
.MODEL QN NPN (level=1 IS=5.07F NF=1 BF=100 VAF=161 IKF=30M ISE=3.9P NE=2
+ BR=4 NR=1 VAR=16 IKR=45M RE=1.03 RB=4.12 RC=.412 XTB=1.5
+ CJE=12.4P VJE=1.1 MJE=.5 CJC=4.02P VJC=.3 MJC=.3 TF=229P TR=959P)
.ENDS

.SUBCKT BS250P 3 4 5
*              D G S
M1 3 2 5 5 MBS250
RG 4 2 160
RL 3 5 1.2E8
C1 2 5 47E-12
C2 3 2 10E-12
D1 3 5 DBS250
*
.MODEL MBS250 PMOS VTO=-3.193 RS=2.041 RD=0.697 IS=1E-15 KP=0.277
+CBD=105E-12 PB=1 LAMBDA=1.2E-2
.MODEL DBS250 D IS=2E-13 RS=0.309
.ENDS BS250P

.SUBCKT BS170 3 4 5
*             D G S
M1 3 2 5 5 N3306M
RG 4 2 270
RL 3 5 1.2E8
C1 2 5 28E-12
C2 3 2 3E-12 
D1 5 3 N3306D
*
.MODEL N3306M NMOS VTO=1.824 RS=1.572 RD=1.436 IS=1E-15 KP=.1233
+CBD=35E-12 PB=1
.MODEL N3306D D IS=5E-12 RS=.768
.ENDS BS170
*

.SUBCKT irf540 1 2 3
* Node 1 -> Drain
* Node 2 -> Gate
* Node 3 -> Source
M1 9 7 8 8 MM L=100u W=100u
* Default values used in MM:
* The voltage-dependent capacitances are
* not included. Other default values are:
* RS=0 RD=0 LD=0 CBD=0 CBS=0 CGBO=0
.MODEL MM NMOS LEVEL=1 IS=1e-32
+VTO=3.56362 LAMBDA=0.00291031 KP=25.0081
+CGSO=1.60584e-05 CGDO=4.25919e-07
RS 8 3 0.0317085
D1 3 1 MD
.MODEL MD D IS=1.02194e-10 RS=0.00968022 N=1.21527 BV=100
+IBV=0.00025 EG=1.2 XTI=3.03885 TT=1e-07
+CJO=1.81859e-09 VJ=1.1279 M=0.449161 FC=0.5
RDS 3 1 4e+06
RD 9 1 0.0135649
RG 2 7 5.11362
D2 4 5 MD1
* Default values used in MD1:
* RS=0 EG=1.11 XTI=3.0 TT=0
* BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=2.49697e-09 VJ=0.5 M=0.9 FC=1e-08
D3 0 5 MD2
* Default values used in MD2:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=0.4 RS=3e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 2.49697e-09
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 0 6 MD3
* Default values used in MD3:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=0.4
.ENDS




*LM5134A
*****************************************************************************
.SUBCKT LM5134A VDD PILOT OUT IN INB VSS
E_E2         INB_INT 0 INB VSS 1
C_U3_C1         U3_N16789866 U3_N16789873  5p  
X_U3_U5         VDD U3_N16789873 U3_N16790218 0 RVAR  RREF=1
C_U3_C3         OUT U3_N16789866  1p  

* E_U3_E1         U3_N16790231 OUT vol=' V(MGATE, 0) > 0.5? 5: -5'
E_U3_E1         U3_N16790231 OUT pwl(1) MGATE 0 0.49 -5 0.51 5

X_U3_U6         U3_N24836 VSS U3_N31827 0 RVAR   RREF=1
R_U3_R1         U3_N16790231 U3_N16789866  20  
M_U3_M2         U3_N24836 U3_N16789871 OUT OUT PMOS01           
E_U3_E4         U3_N16790218 0 pwl(1) VDD_INT 0 
+ 0 0, 4.5 0.9, 10  0.09, 12.6 0.08   
X_U3_U10         VSS OUT d_d1 
R_U3_R2         U3_N16789868 U3_N16789871  20  
C_U3_C2         U3_N24836 U3_N16789871  5p  
X_U3_U9         OUT VDD d_d1 
C_U3_C5         OUT VDD  10p  
C_U3_C6         OUT U3_N16789871  10p  
C_U3_C4         VSS OUT  10p  
M_U3_M1         U3_N16789873 U3_N16789866 OUT OUT NMOS01           
E_U3_E3         U3_N31827 0 pwl(1) VDD_INT 0 
+    0 0, 4.5  0.26, 10  0.01, 12.6 0.01   
E_U3_E2         OUT U3_N16789868 vol= 'V(MGATE, 0) > 0.5? -5: 5'
E_U4_ABM4         U4_N14683241 0 vol= 'V(U4_ON_INT) >=0.5?V(VDD_INT):0'
C_U4_C3         U4_N14683221 0  1n  
V_U4_V6         U4_N155225261 0 80m
R_U4_R2         U4_N14683241 U4_N14683221  1 
X_U4_U47         U4_N14683247 U4_N14683251 d_d1 
E_U4_ABM5         U4_N14683301 0 vol= 'V(U4_ON_INT)<0.5? V(VDD_INT):0'
C_U4_C1         U4_N14683247 0  1n  
X_U4_S1    U4_N14683159 0 U4_N14683247 0 PTON_TOFF_U4_S1 
X_U4_U44         U4_ON_INT PGATE U4_N14683147 AND2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
X_U4_U43         U4_N14683247 PGATE BUF_BASIC_GEN VDD=1 VSS=0
+  VTHRESH=0.5
R_U4_R3         U4_N14683301 U4_N14683281  1 
X_U4_U7         N18232068 U4_N15541612 INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
G_U4_G1         U4_N14683251 U4_N14683247 pwl(1) U4_N14683221 0 
+    0 0, 4.5 140m, 10 500m, 12.6 600m   
G_U4_G2         U4_N14683247 0 pwl(1) U4_N14683281 0 
+    0 0, 4.5 70m, 10 800m, 12.6 900m 
X_U4_U8         MGATE U4_N15541600 INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
C_U4_C4         U4_N14683281 0  1n  
X_U4_S2    U4_N14683147 0 U4_N14683251 U4_N14683247 PTON_TOFF_U4_S2 
X_U4_U48         U4_N155225261 U4_N14683247 d_d1 
X_U4_U45         U4_N15541600 U4_N15541612 U4_ON_INT AND2_BASIC_GEN 
+  VDD=1 VSS=0 VTHRESH=500E-3
V_U4_V5         U4_N14683251 0 1V
X_U4_U46         PGATE U4_ON_INT U4_N14683159 NOR2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
E_E1         IN_INT 0 IN VSS 1
E_E3         VDD_INT 0 VDD VSS 1
C_U2_C1         U2_N14683247 0  1n  
X_U2_U46         MGATE U2_ON_INT U2_N14683159 NOR2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
X_U2_U7         PGATE U2_N15532894 INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
G_U2_G1         U2_N14683251 U2_N14683247 pwl(1)  U2_N14683221 0 
+    0 0,  4.5 58m,  10 140m,  12.6 230m   

E_U2_ABM5         U2_N14683301 0 vol= 'V(U2_ON_INT) <0.5? V(VDD_INT):0'

X_U2_U43         U2_N14683247 MGATE BUF_BASIC_GEN VDD=1 VSS=0
+  VTHRESH=0.5
X_U2_U48         U2_N147032561 U2_N14683247 d_d1 
V_U2_V6         U2_N147032561 0 80m
X_U2_S2    U2_N14683147 0 U2_N14683251 U2_N14683247 MTON_TOFF_U2_S2 

E_U2_ABM4         U2_N14683241 0 vol='V(U2_ON_INT) >=0.5? V(VDD_INT):0  '

R_U2_R2         U2_N14683241 U2_N14683221  1 
G_U2_G2         U2_N14683247 0 PWL(1) U2_N14683281 0 
+    0 0, 4.5 32m, 10 90m, 12.6 160m   
V_U2_V5         U2_N14683251 0 1V
X_U2_U44         U2_ON_INT MGATE U2_N14683147 AND2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
C_U2_C3         U2_N14683221 0  1n  
X_U2_U45         N18232068 U2_N15532894 U2_ON_INT AND2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
X_U2_S1    U2_N14683159 0 U2_N14683247 0 MTON_TOFF_U2_S1 
R_U2_R3         U2_N14683301 U2_N14683281  1 
C_U2_C4         U2_N14683281 0  1n  
X_U2_U47         U2_N14683247 U2_N14683251 d_d1 
X_U1_U6         INB_INT U1_VIH U1_VHYS U1_N15517298 COMPHYS_BASIC_GEN 
+  VDD=1 VSS=0 VTHRESH=0.5
R_U1_R1         U1_N15521766 U1_VDD_UVLO  721.5 
X_U1_U7         U1_N15517298 U1_INB_OUT INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
X_U1_U9         U1_IN_OUT U1_INB_OUT U1_VDD_UVLO N18232068 AND3_BASIC_GEN
+   VDD=1 VSS=0 VTHRESH=500E-3
C_U1_C1         U1_VDD_UVLO 0  1n  
X_U1_U5         IN_INT U1_VIH U1_VHYS U1_IN_OUT COMPHYS_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=0.5
X_U1_U8         VDD_INT U1_N15521824 U1_N15521760 U1_N15521766
+  COMPHYS_BASIC_GEN  VDD=1 VSS=0 VTHRESH=0.5
V_U1_V1         U1_N15521824 0 3.6
E_U1_ABM3         U1_VHYS 0 vol= '0.34*V(VDD_INT)'
E_U1_ABM1         U1_VIH 0 vol='0.67*V(VDD_INT)'
V_U1_V2         U1_N15521760 0 0.36
X_U1_U35         U1_VDD_UVLO U1_N15521766 d_d1 
M_U5_M2         U5_N16789896 U5_N23038 PILOT PILOT PMOS02           
R_U5_R1         U5_N16790231 U5_N16789866  20  
X_U5_U6         U5_N16789896 VSS U5_N16802670 0 RVAR  RREF=1
M_U5_M1         U5_N08221 U5_N16789866 PILOT PILOT NMOS02           
C_U5_C4         VSS PILOT  10p  
C_U5_C2         U5_N16789896 U5_N23038  5p  
R_U5_R2         U5_N16789868 U5_N23038  20  
E_U5_E3         U5_N16802670 0 pwl(1) VDD_INT 0 
+    0 0, 4.5 2.8, 10 1.9, 12.6 1.6   
X_U5_U5         VDD U5_N08221 U5_N26349 0 RVAR RREF=1
C_U5_C1         U5_N08221 U5_N16789866  5p  
C_U5_C5         PILOT VDD  10p  
E_U5_E4         U5_N26349 0 pwl(1) VDD_INT 0 
+    0 0, 4.5 9.5, 10 2.88, 12.6 2.8   
X_U5_U9         PILOT VDD d_d1 
C_U5_C3         PILOT U5_N16789866  10p  
X_U5_U10         VSS PILOT d_d1 
E_U5_E2         PILOT U5_N16789868 vol= ' V(PGATE, 0) > 0.5? -5: 5'
E_U5_E1         U5_N16790231 PILOT vol= ' V(PGATE, 0) > 0.5? 5: -5'
C_U5_C6         PILOT U5_N23038  10p  
.ENDS LM5134A
*$
.SUBCKT PTON_TOFF_U4_S1 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U4_S1         1 2 1G
.ENDS PTON_TOFF_U4_S1
*$
.SUBCKT PTON_TOFF_U4_S2 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U4_S2         1 2 1G
.ENDS PTON_TOFF_U4_S2
*$
.SUBCKT MTON_TOFF_U2_S2 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U2_S2         1 2 1G
.ENDS MTON_TOFF_U2_S2
*$
.SUBCKT MTON_TOFF_U2_S1 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U2_S1         1 2 1G
.ENDS MTON_TOFF_U2_S1
*$
****************************** Basic Components **************************
.SUBCKT D_D1 1 2
D1 1 2 DD1
.MODEL DD1 D (IS=1e-15 Rs=0.001 N=0.1  TT=10p)
.ENDS D_D1
*$
.SUBCKT BUF_BASIC_GEN A  Y VDD=1 VSS=0 VTHRESH=0.5 
* E_ABMGATE    YINT 0 vol='V(A) > VTHRESH? VDD:VSS'
E_ABMGATE    YINT 0 pwl(1) A 0 'VTHRESH-0.01' VSS 'VTHRESH+0.01' VDD 
RINT YINT Y 1
CINT Y 0 1n
.ENDS BUF_BASIC_GEN
*$
.MODEL NMOS01 NMOS (VTO = 2 KP = 1.005 LAMBDA  = 0.001)
*$
.MODEL PMOS01 PMOS (VTO = -2 KP = 1.77 LAMBDA  = 0.001)
*$
.MODEL NMOS02 NMOS (VTO = 2 KP = 0.1814 LAMBDA = 0.001)
*$
.MODEL PMOS02 PMOS (VTO = -2 KP = 0.2497 LAMBDA = 0.001)
*$
.SUBCKT COMPHYS_BASIC_GEN INP INM HYS OUT VDD=1 VSS=0 VTHRESH=0.5	
EIN INP1 INM1 INP INM 1 
* EHYS INP1 INP2 vol='V(1) > VTHRESH? -V(HYS):0'
EHYS INP1 INP2 pwl(1) 1 0 'VTHRESH-0.01' 0 'VTHRESH+0.01' '-V(HYS)'

EOUT OUT 0 vol='V(INP2)>V(INM1)? VDD : VSS'
R1 OUT 1 1
C1 1 0 5n
RINP1 INP1 0 1K
.ENDS COMPHYS_BASIC_GEN
*$
.SUBCKT AND2_BASIC_GEN A B Y  VDD=1 VSS=0 VTHRESH=0.5 
E_ABMGATE    YINT 0 vol= 'V(A) > VTHRESH  && V(B) > VTHRESH ? VDD:VSS'
RINT YINT Y 1
CINT Y 0 1n
.ENDS AND2_BASIC_GEN
*$
.SUBCKT NOR2_BASIC_GEN A B Y  VDD=1 VSS=0 VTHRESH=0.5 
E_ABMGATE    YINT 0 vol= 'V(A) > VTHRESH  || V(B) > VTHRESH ? VSS:VDD'
RINT YINT Y 1
CINT Y 0 1n
.ENDS NOR2_BASIC_GEN
*$
.SUBCKT AND3_BASIC_GEN A B C Y VDD=1 VSS=0 VTHRESH=0.5 
E_ABMGATE    YINT 0 vol= 'V(A) > VTHRESH  &&  V(B) > VTHRESH && V(C) > VTHRESH ? VDD:VSS'
RINT YINT Y 1
CINT Y 0 1n
.ENDS AND3_BASIC_GEN
*$
.SUBCKT INV_BASIC_GEN A  Y VDD=1 VSS=0 VTHRESH=0.5 
* E_ABMGATE    YINT 0 vol='V(A) > VTHRESH ? VSS : VDD'
E_ABMGATE    YINT 0 pwl(1) A 0 'VTHRESH-0.01' VDD 'VTHRESH+0.01' VSS
RINT YINT Y 1
CINT Y 0 1n
.ENDS INV_BASIC_GEN
*$
.SUBCKT RVAR 101 102 201 202 RREF=1
* nodes : 101 102 : nodes between which variable resistance is placed
* 201 202 : nodes to whose voltage the resistance is proportional
* parameters : rref : reference value of the resistance
rin 201 202 1G   $$ input resistance
r 301 0 rref
fcopy 0 301 vsense 1 $$  copy output current thru Z
eout 101 106 poly(2) 201 202 301 0 0 0 0 0 1   $$ multiply VoverZ with Vctrl
vsense 106 102 0   $$ sense iout
.ENDS RVAR
*$


* (REV N/A)    SUPPLY VOLTAGE: 5V
* CONNECTIONS: NON-INVERTING INPUT
*              | INVERTING INPUT
*              | | POSITIVE POWER SUPPLY
*              | | | NEGATIVE POWER SUPPLY
*              | | | | OUTPUT
*              | | | | |
.SUBCKT LM324  1 2 3 4 5
*
  C1   11 12 5.544E-12
  C2    6  7 20.00E-12
  DC    5 53 DX
  DE   54  5 DX
  DLP  90 91 DX
  DLN  92 90 DX
  DP    4  3 DX
  EGND 99  0 POLY(2) (3,0) (4,0) 0 .5 .5
  FB    7 99 POLY(5) VB VC VE VLP VLN 0 15.91E6 -20E6 20E6 20E6 -20E6
  GA    6  0 11 12 125.7E-6
  GCM   0  6 10 99 7.067E-9
  IEE   3 10 DC 10.04E-6
  HLIM 90  0 VLIM 1K
  Q1   11  2 13 QX
  Q2   12  1 14 QX
  R2    6  9 100.0E3
  RC1   4 11 7.957E3
  RC2   4 12 7.957E3
  RE1  13 10 2.773E3
  RE2  14 10 2.773E3
  REE  10 99 19.92E6
  RO1   8  5 50
  RO2   7 99 50
  RP    3  4 30.31E3
  VB    9  0 DC 0
  VC 3 53 DC 2.100
  VE   54  4 DC .6
  VLIM  7  8 DC 0
  VLP  91  0 DC 40
  VLN   0 92 DC 40
.MODEL DX D(IS=800.0E-18)
.MODEL QX PNP(IS=800.0E-18 BF=250)
.ENDS


*LM317 TI voltage regulator - pin order: In, Adj, Out
*TI adjustable voltage regulator pkg:TO-3
.SUBCKT LM317 1 2 3 **Changes my be required on this line**
J1 1 3 4 JN
Q2 5 5 6 QPL .1
Q3 5 8 9 QNL .2
Q4 8 5 7 QPL .1
Q5 81 8 3 QNL .2
Q6 3 81 10 QPL .2
Q7 12 81 13 QNL .2
Q8 10 5 11 QPL .2
Q9 14 12 10 QPL .2
Q10 16 5 17 QPL .2
Q11 16 14 15 QNL .2
Q12 3 20 16 QPL .2
Q13 1 19 20 QNL .2
Q14 19 5 18 QPL .2
Q15 3 21 19 QPL .2
Q16 21 22 16 QPL .2
Q17 21 3 24 QNL .2
Q18 22 22 16 QPL .2
Q19 22 3 241 QNL 2
Q20 3 25 16 QPL .2
Q21 25 26 3 QNL .2
Q22A 35 35 1 QPL 2
Q22B 16 35 1 QPL 2
Q23 35 16 30 QNL 2
Q24A 27 40 29 QNL .2
Q24B 27 40 28 QNL .2
Q25 1 31 41 QNL 5
Q26 1 41 32 QNL 50
D1 3 4 DZ
D2 33 1 DZ
D3 29 34 DZ
R1 1 6 310
R2 1 7 310
R3 1 11 190
R4 1 17 82
R5 1 18 5.6K
R6 4 8 100K
R7 8 81 130
R8 10 12 12.4K
R9 9 3 180
R10 13 3 4.1K
R11 14 3 5.8K
R12 15 3 72
R13 20 3 5.1K
R14 2 24 12K
R15 24 241 2.4K
R16 16 25 6.7K
R17 16 40 12K
R18 30 41 130
R19 16 31 370
R20 26 27 13K
R21 27 40 400
R22 3 41 160
R23 33 34 18K
R24 28 29 160
R25 28 32 3
R26 32 3 .1
C1 21 3 30PF
C2 21 2 30PF
C3 25 26 5PF
CBS1 5 3 2PF
CBS2 35 3 1PF
CBS3 22 3 1PF
.MODEL JN NJF(BETA=1E-4 VTO=-7)
.MODEL DZ D(BV=6.3)
.MODEL QNL NPN(EG=1.22 BF=80 RB=100 CCS=1.5PF TF=.3NS TR=6NS CJE=2PF
+ CJC=1PF VAF=100)
.MODEL QPL PNP(BF=40 RB=20 TF=.6NS TR=10NS CJE=1.5PF CJC=1PF VAF=50)
.ENDS

.SUBCKT xxxxLM317 1 3 2
* IN ADJ OUT
IADJ 1 4 50U
VREF 4 3 1.25
RC 1 14 0.742
DBK 14 13 D1
CBC 13 15 2.479N
RBC 15 5 247
QP 13 5 2 Q1
RB2 6 5 124
DSC 6 11 D1
ESC 11 2 POLY(2) (13,5) (6,5) 2.85
+ 0 0 0 -70.1M
DFB 6 12 D1
EFB 12 2 POLY(2) (13,5) (6,5) 3.92
+ -135M 0 1.21M -70.1M
RB1 7 6 1
EB 7 2 8 2 2.56
CPZ 10 2 0.796U
DPU 10 2 D1
RZ 8 10 0.104
RP 9 8 100
EP 9 2 4 2 103.6
RI 2 4 100MEG
.MODEL Q1 NPN (IS=30F BF=100
+ VAF=14.27 NF=1.604)
.MODEL D1 D (IS=30F N=1.604)
.ENDS



.SUBCKT LM337 8 1 19
*Connections Input Adj. Output
*LM337 negative voltage regulator
.MODEL QN NPN (BF=50 TF=1N CJC=1P)
.MODEL QPOUT PNP (BF=50 TF=1N RE=.2 CJC=1P)
.MODEL QP PNP CJC=1P TF=2N
.MODEL DN D
.MODEL D2 D BV=12 IBV=100U
R10 25 6 1K
Q3 8 17 16 QPOUT
Q4 8 25 17 QP
R18 19 17 250
R19 19 16 .3
G1 8 6 1 18 .1
C7 6 2 .04U
R24 2 8 100
I_ADJ 0 1 65U
R26 8 25 200K
Q5 25 4 19 QP
R27 16 4 200
R28 7 4 7K
D1 8 7 D2
D2 8 6 DN
V1 18 19 1.25
.ENDS
" tclcommand="xschem edit_vi_prop" embed=true}
[
v {xschem version=3.0.0 file_version=1.2}
G {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
format="
@value
"}
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
