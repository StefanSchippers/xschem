v {xschem version=3.4.6 file_version=1.2
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
L 4 110 -270 110 -210 {}
L 4 110 -210 120 -210 {}
L 4 110 -190 120 -210 {}
L 4 100 -210 110 -190 {}
L 4 100 -210 110 -210 {}
B 2 590 -550 1390 -150 {flags=graph
y1=-0.0011
y2=0.0014
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=1e-10
x2=0.0008
divx=5
subdivx=1
node="\\"Cap. power; i(v1) a b - *\\"
\\"Avg Cap. power running average; i(v1) a b - * 40u ravg()\\""
color="4 7"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 590 -810 1390 -550 {flags=graph
y1=-0.0019
y2=0.0007
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=1e-10
x2=0.0008
divx=5
subdivx=1


dataset=-1
unitx=1
logx=0
logy=0
color=8
node=i(v1)}
B 2 590 -1070 1390 -810 {flags=graph
y1=-0.7
y2=3
ypos1=0.22808833
ypos2=1.7756503
divy=5
subdivy=1
unity=1
x1=1e-10
x2=0.0008
divx=5
subdivx=1


dataset=-1
unitx=1
logx=0
logy=0

color="4 8 6 7"
node="c
b
a
d"
hilight_wave=-1
digital=1}
T {I(V1)} 50 -250 0 0 0.4 0.4 {}
N 150 -480 150 -380 {lab=B}
N 150 -200 350 -200 {lab=0}
N 150 -320 150 -260 {lab=C}
N 150 -580 350 -580 {lab=A}
N 350 -580 350 -480 {lab=A}
N 150 -580 150 -540 {lab=A}
N 350 -280 350 -200 {
lab=0}
N 350 -420 350 -340 {
lab=D}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {launcher.sym} 320 -100 0 0 {name=h1
descr="NGSPICE Manual" 
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwiVvafVvtzfAhVHExoKHaZLAIQQFjAAegQICRAC&url=http%3A%2F%2Fngspice.sourceforge.net%2Fdocs%2Fngspice-manual.pdf&usg=AOvVaw3n8fjRjAJRgWMjg1Y04Ht8"}
C {code_shown.sym} 0 -930 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="
.control 
  
  save all
  tran 10n 800u uic
  let pow = i(v1) * v(a,b)
  meas tran cap_avg_pow avg pow from=600u to=800u
  write  rlc.raw
.endc
"}
C {capa.sym} 150 -510 0 0 {name=C1 m=1 value=50nF footprint=1206 device="ceramic capacitor"}
C {lab_pin.sym} 150 -290 2 1 {name=l2 sig_type=std_logic lab=C}
C {lab_pin.sym} 150 -560 2 0 {name=l4 sig_type=std_logic lab=A}
C {lab_pin.sym} 350 -200 2 0 {name=l5 sig_type=std_logic lab=0}
C {res.sym} 350 -450 0 0 {name=R1 m=1 value=1k footprint=1206 device=resistor
}
C {ind.sym} 150 -350 0 0 {name=L1 value=10mH}
C {vsource.sym} 150 -230 0 0 {name=V1 
xvalue="pwl 0 0 100u 0 101u 3" 
value="pulse 0 3 0 100n 100n 9.9u 20u"

}
C {ammeter.sym} 350 -310 0 0 {name=Vmeas}
C {lab_pin.sym} 150 -450 2 0 {name=l6 sig_type=std_logic lab=B}
C {lab_pin.sym} 350 -380 2 0 {name=l7 sig_type=std_logic lab=D}
C {launcher.sym} 760 -90 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/rlc.raw tran"
}
