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
N 90 -460 130 -460 { lab=D}
N 500 -570 520 -570 { lab=DI}
N 500 -570 500 -460 { lab=DI}
N 870 -460 1030 -460 { lab=QI}
N 970 -570 990 -570 { lab=QI}
N 970 -570 970 -460 { lab=QI}
N 1110 -460 1160 -460 { lab=Q}
N 990 -460 990 -360 { lab=QI}
N 990 -300 990 -260 { lab=0}
N 790 -460 870 -460 { lab=QI}
N 690 -460 730 -460 { lab=#net1}
N 670 -460 690 -460 { lab=#net1}
N 460 -460 590 -460 { lab=DI}
N 340 -460 400 -460 { lab=#net2}
N 520 -460 520 -360 { lab=DI}
N 520 -300 520 -260 { lab=0}
N 210 -460 280 -460 { lab=#net3}
C {ipin.sym} 70 -240 0 0 {name=p1 lab=D}
C {ipin.sym} 70 -200 0 0 {name=p2 lab=CLK}
C {ipin.sym} 70 -160 0 0 {name=p3 lab=RST}
C {opin.sym} 250 -200 0 0 {name=p4 lab=Q}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {keeper_ngspice.sym} 560 -570 0 0 {name=x2}
C {switch_ngspice.sym} 310 -460 1 1 {name=S1 model=SWITCH}
C {lab_pin.sym} 310 -420 0 1 {name=l4 sig_type=std_logic lab=VCC}
C {lab_pin.sym} 290 -420 0 0 {name=l5 sig_type=std_logic lab=CLK}
C {lab_pin.sym} 90 -460 0 0 {name=l6 sig_type=std_logic lab=D}
C {switch_ngspice.sym} 760 -460 1 1 {name=S2 model=SWITCH}
C {lab_pin.sym} 740 -420 0 0 {name=l7 sig_type=std_logic lab=0}
C {lab_pin.sym} 760 -420 0 1 {name=l8 sig_type=std_logic lab=CLK}
C {keeper_ngspice.sym} 1030 -570 0 0 {name=x3}
C {lab_pin.sym} 500 -480 0 1 {name=l9 sig_type=std_logic lab=DI}
C {buf_ngspice.sym} 1070 -460 0 0 {name=x4 RUP=100 RDOWN=100}
C {lab_pin.sym} 970 -480 0 0 {name=l10 sig_type=std_logic lab=QI}
C {lab_pin.sym} 1160 -460 0 1 {name=l11 sig_type=std_logic lab=Q}
C {switch_ngspice.sym} 990 -330 0 0 {name=S3 model=SWITCH}
C {lab_pin.sym} 990 -260 0 0 {name=l2 sig_type=std_logic lab=0}
C {lab_pin.sym} 950 -310 0 0 {name=l3 sig_type=std_logic lab=0}
C {lab_pin.sym} 950 -330 0 0 {name=l12 sig_type=std_logic lab=RST}
C {buf_ngspice.sym} 630 -460 0 0 {name=x1
RUP=1000}
C {switch_ngspice.sym} 430 -460 1 1 {name=S4 model=SWITCH}
C {lab_pin.sym} 430 -420 0 1 {name=l13 sig_type=std_logic lab=VCC}
C {lab_pin.sym} 410 -420 0 0 {name=l14 sig_type=std_logic lab=RST}
C {switch_ngspice.sym} 520 -330 0 0 {name=S5 model=SWITCH}
C {lab_pin.sym} 520 -260 0 0 {name=l15 sig_type=std_logic lab=0}
C {lab_pin.sym} 480 -310 0 0 {name=l16 sig_type=std_logic lab=0}
C {lab_pin.sym} 480 -330 0 0 {name=l17 sig_type=std_logic lab=RST}
C {buf_ngspice.sym} 170 -460 0 0 {name=x5
RUP=1000}
