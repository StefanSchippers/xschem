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
L 7 980 -150 1160 -150 {}
P 4 7 210 -290 210 -370 220 -370 210 -390 200 -370 210 -370 210 -290 {}
T {Set tcl variable lvs_ignore to:
- 1 to short elements with lvs_ignore=short attribute set
    to disable elements with lvs_ignore=open attribute set
- 0 for normal netlisting} 60 -1030 0 0 1 1 {}
T {tcleval(lvs_ignore=$lvs_ignore)} 980 -190 0 0 0.6 0.6 {name=l1}
T {This component has attribute
lvs_ignore=open} 910 -370 0 0 0.4 0.4 {}
T {This component has attribute
lvs_ignore=short} 70 -560 0 0 0.4 0.4 {}
T {This is the lab_show component
it is used only to display the net
name it is attached to. This works if
Options->Show net names on symbol pins
is enabled.} 220 -350 0 0 0.4 0.4 {}
T {This component has
attribute
lvs_ignore=open} 470 -650 0 0 0.4 0.4 {}
N 850 -430 1010 -430 {
lab=#net1}
N 850 -430 850 -380 {
lab=#net1}
N 50 -540 50 -530 {
lab=VDD}
N 50 -470 50 -430 {
lab=#net2}
N 50 -430 280 -430 {
lab=#net2}
N 360 -430 430 -430 {
lab=STARTUP}
N 50 -430 50 -380 {
lab=#net2}
N 50 -320 50 -290 {
lab=GND}
N 850 -580 850 -570 {
lab=VDD}
N 850 -510 850 -430 {
lab=#net1}
N 1090 -430 1110 -430 {
lab=#net3}
N 850 -320 850 -290 {
lab=GND}
N 400 -520 490 -520 {
lab=STARTUP}
N 400 -520 400 -430 {
lab=STARTUP}
N 550 -520 700 -520 {
lab=STARTUP}
C {title.sym} 160 -30 0 0 {name=l1
author="Stefan Schippers"
}
C {launcher.sym} 750 -90 0 0 {name=h1
descr="Toggle lvs_ignore variable and
rebuild connectivity"
tclcommand="
if \{![info exists lvs_ignore]\} \{
   set lvs_ignore 1
\} else \{
  set lvs_ignore [expr \{!$lvs_ignore\}]
\}
xschem rebuild_connectivity
xschem unhilight_all
"}
C {vsource.sym} 850 -350 0 0 {name=V2 value=0
lvs_ignore=open}
C {inv_ngspice.sym} 1050 -430 0 0 {name=x5 ROUT=1000}
C {lab_show.sym} 920 -430 0 1 {name=l10 }
C {lab_show.sym} 1110 -430 0 1 {name=l3 }
C {gnd.sym} 850 -290 0 0 {name=l4 lab=GND}
C {res.sym} 50 -500 0 0 {name=R1
value=100MEG
footprint=1206
device=resistor
m=1
lvs_ignore=short}
C {vdd.sym} 50 -540 0 0 {name=l12 lab=VDD}
C {capa.sym} 50 -350 0 0 {name=C1
m=1
value=1p
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 50 -290 0 0 {name=l13 lab=GND}
C {lab_show.sym} 170 -430 2 0 {name=l14 }
C {lab_pin.sym} 430 -430 0 1 {name=p1 sig_type=std_logic lab=STARTUP}
C {inv_ngspice.sym} 320 -430 0 0 {name=x2 ROUT=1000}
C {res.sym} 850 -540 0 0 {name=R2
value=100MEG
footprint=1206
device=resistor
m=1
}
C {vdd.sym} 850 -580 0 0 {name=l6 lab=VDD}
C {short.sym} 520 -520 1 0 {name=x1 value=0.1 lvs_ignore=open}
C {lab_show.sym} 700 -520 0 1 {name=l2 }
