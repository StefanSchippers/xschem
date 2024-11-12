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
B 2 10 -900 810 -500 {flags=graph
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
B 2 10 -1300 810 -900 {flags=graph
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
  write pcb_current_protection.raw
.endc
"}
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan"}
C {lab_pin.sym} 90 -280 0 1 {name=p8 lab=VCC}
C {conn_8x1.sym} 70 -260 0 0 {name=C2}
C {lab_pin.sym} 90 -260 0 1 {name=p9 lab=VSS}
C {lm358.sym} 530 -270 0 0 {name=U3:1
spiceprefix=X
VCC=VCC 
VNN=VSS
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=3&ved=2ahUKEwi8_s-Z5KTfAhUmy4UKHbDOBUIQFjACegQICBAC&url=https%3A%2F%2Fwww.sparkfun.com%2Fdatasheets%2FComponents%2FGeneral%2FLM358.pdf&usg=AOvVaw0pOnlomz_hkzyk7OvZBPFv"
}
C {res.sym} 360 -160 0 0 {name=R3 m=1 value=4k footprint=1206}
C {res.sym} 360 -290 0 0 {name=R4 m=1 value=100k footprint=1206}
C {gnd.sym} 360 -110 0 0 {name=l0 lab=VSS}
C {nmos.sym} 930 -270 0 0 {name=M2
spiceprefix=X}
C {res.sym} 950 -360 0 0 {name=R5 m=1 value=1 footprint=1206}
C {gnd.sym} 950 -240 0 0 {name=l6 lab=VSS}
C {vdd.sym} 360 -350 0 0 {name=l7 lab=VCC}
C {vdd.sym} 950 -580 0 0 {name=l8 lab=VCC}
C {lab_pin.sym} 950 -430 0 0 {name=p1 lab=VSSLOAD}
C {lab_pin.sym} 360 -210 0 0 {name=p2 lab=VREF}
C {res.sym} 950 -490 0 0 {name=RLOAD m=1 value="R='1/(time+1e-7)'" footprint=1206}
C {ammeter.sym} 950 -550 0 0 {name=vmeasure}
C {lab_wire.sym} 890 -270 0 0 {name=l12 lab=G}
C {lab_pin.sym} 950 -320 0 0 {name=p3 lab=D}
C {res.sym} 690 -270 1 0 {name=R2 m=1 value=20k footprint=1206}
C {npn.sym} 720 -150 0 0 {name=Q1 model=q2n2222
url="http://html.alldatasheet.com/html-pdf/17919/PHILIPS/MMBT2222/499/2/MMBT2222.html"
}
C {gnd.sym} 740 -120 0 0 {name=l16 lab=VSS}
C {lab_pin.sym} 590 -150 0 0 {name=p4 lab=RST}
C {res.sym} 650 -150 1 0 {name=R6 m=1 value=20k footprint=1206}
C {lab_pin.sym} 90 -240 0 1 {name=p5 lab=RST}
C {npn.sym} 790 -200 0 0 {name=Q2 model=q2n2222
url="http://html.alldatasheet.com/html-pdf/17919/PHILIPS/MMBT2222/499/2/MMBT2222.html"
}
C {gnd.sym} 810 -170 0 0 {name=l11 lab=VSS}
C {res.sym} 810 -280 2 0 {name=R1 m=1 value=20k footprint=1206}
C {vdd.sym} 810 -310 0 0 {name=l13 lab=VCC}
C {launcher.sym} 340 -460 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/pcb_current_protection.raw tran"
}
