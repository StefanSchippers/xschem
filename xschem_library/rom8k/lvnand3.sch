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
N 560 -350 770 -350 {lab=y}
N 430 -630 490 -630 {lab=VCCPIN}
N 430 -600 490 -600 {lab=VCCPIN}
N 430 -570 430 -350 {lab=y}
N 620 -130 640 -130 {lab=VSSBPIN}
N 370 -600 390 -600 {lab=a}
N 480 -540 500 -540 {lab=b}
N 490 -630 490 -600 {lab=VCCPIN}
N 620 -260 620 -240 {lab=#net1}
N 620 -290 640 -290 {lab=VSSBPIN}
N 620 -350 620 -320 {lab=y}
N 540 -570 600 -570 {lab=VCCPIN}
N 540 -540 600 -540 {lab=VCCPIN}
N 600 -570 600 -540 {lab=VCCPIN}
N 540 -510 540 -350 {lab=y}
N 430 -350 540 -350 {lab=y}
N 540 -350 560 -350 {lab=y}
N 190 -600 370 -600 {lab=a}
N 190 -540 480 -540 {lab=b}
N 650 -510 710 -510 {lab=VCCPIN}
N 650 -480 710 -480 {lab=VCCPIN}
N 710 -510 710 -480 {lab=VCCPIN}
N 190 -480 610 -480 {lab=c}
N 650 -450 650 -350 {lab=y}
N 620 -210 640 -210 {lab=VSSBPIN}
N 620 -180 620 -160 {lab=#net2}
N 320 -130 580 -130 {lab=a}
N 320 -600 320 -130 {lab=a}
N 340 -540 340 -210 {lab=b}
N 340 -210 580 -210 {lab=b}
N 360 -290 580 -290 {lab=c}
N 360 -480 360 -290 {lab=c}
C {lab_pin.sym} 620 -100 0 0 {name=lg1 lab=VSSPIN}
C {lab_pin.sym} 430 -630 0 0 {name=lg2 lab=VCCPIN}
C {opin.sym} 770 -350 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 190 -600 0 0 {name=p2 lab=a}
C {nlv4t.sym} 600 -130 0 0 {name=m1 model=cmosn w=wn l=lln  m=1
as="'0.270u*(wn)'"
ad="'0.270u*(wn)'"
ps="'2*0.270u+wn'"
pd="'2*0.270u+wn'"}
C {plv4t.sym} 410 -600 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 
as="'0.270u*(wp)'"
ad="'0.270u*(wp)'"
ps="'2*0.270u+wp'"
pd="'2*0.270u+wp'"}
C {plv4t.sym} 520 -540 0 0 {name=m3 model=cmosp w=wp l=lp  m=1 
as="'0.270u*(wp)'"
ad="'0.270u*(wp)'"
ps="'2*0.270u+wp'"
pd="'2*0.270u+wp'"}
C {nlv4t.sym} 600 -290 0 0 {name=m5 model=cmosn w=wn l=lln  m=1
as="'0.270u*(wn)'"
ad="'0.270u*(wn)'"
ps="'2*0.270u+wn'"
pd="'2*0.270u+wn'"}
C {ipin.sym} 190 -540 0 0 {name=p3 lab=b}
C {lab_pin.sym} 540 -570 0 0 {name=l1 lab=VCCPIN}
C {ipin.sym} 190 -480 0 0 {name=p4 lab=c}
C {plv4t.sym} 630 -480 0 0 {name=m4 model=cmosp w=wp l=lp  m=1 
as="'0.270u*(wp)'"
ad="'0.270u*(wp)'"
ps="'2*0.270u+wp'"
pd="'2*0.270u+wp'"}
C {lab_pin.sym} 650 -510 0 0 {name=l0 lab=VCCPIN}
C {nlv4t.sym} 600 -210 0 0 {name=m6 model=cmosn w=wn l=lln  m=1
as="'0.270u*(wn)'"
ad="'0.270u*(wn)'"
ps="'2*0.270u+wn'"
pd="'2*0.270u+wn'"}
C {lab_pin.sym} 640 -130 0 1 {name=l2 lab=VSSPIN}
C {lab_pin.sym} 640 -210 0 1 {name=l3 lab=VSSPIN}
C {lab_pin.sym} 640 -290 0 1 {name=l4 lab=VSSPIN}
C {title.sym} 160 0 0 0 {name=l5 author="Stefan Schippers"}
