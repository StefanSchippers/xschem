v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {JTAG
CONNECTOR} 1060 -650 0 0 0.6 0.6 {}
T {Place near the VCC
pin for U20} 1140 -1010 0 0 0.3 0.3 {}
T {This schematic is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This schematic is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
(C) 2001 Stefan Petersen (spe@stacken.kth.se)
} 20 -310 0 0 0.3 0.3 {}
N 910 -670 1010 -670 {lab=#net1}
N 910 -870 910 -670 {lab=#net1}
N 610 -870 910 -870 {lab=#net1}
N 870 -650 1010 -650 {lab=#net2}
N 870 -750 870 -650 {lab=#net2}
N 610 -750 870 -750 {lab=#net2}
N 610 -630 1010 -630 {lab=#net3}
N 870 -610 1010 -610 {lab=#net4}
N 870 -610 870 -510 {lab=#net4}
N 610 -510 870 -510 {lab=#net4}
N 910 -590 1010 -590 {lab=#net5}
N 910 -590 910 -390 {lab=#net5}
N 610 -390 910 -390 {lab=#net5}
N 990 -550 1010 -550 {lab=#net6}
N 990 -550 990 -530 {lab=#net6}
N 950 -570 1010 -570 {lab=GND}
N 950 -570 950 -510 {lab=GND}
N 950 -690 1010 -690 {lab=jtag_power}
N 950 -1030 950 -690 {lab=jtag_power}
N 260 -1030 950 -1030 {lab=jtag_power}
N 260 -870 530 -870 {lab=tdi_ttl}
N 260 -750 530 -750 {lab=tdo_ttl}
N 260 -630 530 -630 {lab=tms_ttl}
N 260 -630 530 -630 {lab=tms_ttl}
N 260 -510 530 -510 {lab=tck_ttl}
N 260 -390 530 -390 {lab=trst_ttl}
N 1130 -920 1130 -900 {lab=GND}
N 1130 -1000 1130 -980 {lab=jtag_power}
N 480 -1030 480 -990 {lab=jtag_power}
N 480 -930 480 -750 {lab=tdo_ttl}
N 420 -1030 420 -990 {lab=jtag_power}
N 420 -930 420 -630 {lab=tms_ttl}
N 360 -1030 360 -990 {lab=jtag_power}
N 360 -930 360 -510 {lab=tck_ttl}
N 300 -1030 300 -990 {lab=jtag_power}
N 300 -930 300 -390 {lab=trst_ttl}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {7414.sym} 570 -870 0 1 {name=U20:1 
power=jtag_power 
ground=GND}
C {7414.sym} 570 -750 0 0 {name=U20:2 
power=jtag_power 
ground=GND}
C {7414.sym} 570 -630 0 0 {name=U20:3 
power=jtag_power 
ground=GND}
C {7414.sym} 570 -510 0 0 {name=U20:4 
power=jtag_power 
ground=GND}
C {7414.sym} 570 -390 0 0 {name=U20:5 
power=jtag_power 
ground=GND}
C {conn_8x1.sym} 1030 -670 0 1 {name=CONN4 footprint=conn_jtag}
C {noconn.sym} 990 -530 0 0 {name=l2}
C {gnd.sym} 950 -510 0 0 {name=l3 lab=GND}
C {opin.sym} 260 -1030 0 1 {name=p1 lab=jtag_power}
C {opin.sym} 260 -870 0 1 {name=p2 lab=tdi_ttl}
C {ipin.sym} 260 -750 0 0 {name=p3 lab=tdo_ttl}
C {ipin.sym} 260 -630 0 0 {name=p4 lab=tms_ttl}
C {ipin.sym} 260 -510 0 0 {name=p5 lab=tck_ttl}
C {ipin.sym} 260 -390 0 0 {name=p6 lab=trst_ttl}
C {capa.sym} 1130 -950 0 0 {name=C20
m=1
value=100n
footprint=sm1206
device="ceramic capacitor"}
C {lab_pin.sym} 1130 -1000 0 0 {name=l4 sig_type=std_logic lab=jtag_power}
C {gnd.sym} 1130 -900 0 0 {name=l5 lab=GND}
C {res.sym} 480 -960 0 0 {name=R23
value=10k
footprint=sm0805
device=resistor
m=1}
C {res.sym} 420 -960 0 0 {name=R22
value=10k
footprint=sm0805
device=resistor
m=1}
C {res.sym} 360 -960 0 0 {name=R21
value=10k
footprint=sm0805
device=resistor
m=1}
C {res.sym} 300 -960 0 0 {name=R20
value=10k
footprint=sm0805
device=resistor
m=1}
