v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {USB connector} 20 -730 0 0 0.6 0.6 {}
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
} 20 -300 0 0 0.3 0.3 {}
N 280 -680 300 -680 {lab=GND}
N 300 -680 300 -660 {lab=GND}
N 280 -740 360 -740 {lab=usb_power}
N 410 -590 490 -590 {lab=GND}
N 410 -590 410 -400 {lab=GND}
N 410 -510 490 -510 {lab=GND}
N 890 -550 950 -550 {lab=GND}
N 950 -550 950 -400 {lab=GND}
N 890 -470 950 -470 {lab=GND}
N 270 -550 490 -550 {lab=usb_iom}
N 270 -470 490 -470 {lab=usb_iop}
N 280 -720 380 -720 {lab=usb_iom}
N 380 -720 380 -550 {lab=usb_iom}
N 280 -700 350 -700 {lab=usb_iop}
N 350 -700 350 -470 {lab=usb_iop}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {conn_4x1.sym} 260 -710 0 0 {name=CONN2 footprint=conn_usb}
C {sn75240pw.sym} 690 -530 0 0 {name=U1}
C {gnd.sym} 300 -660 0 0 {name=l2 lab=GND}
C {opin.sym} 360 -740 0 0 {name=p1 lab=usb_power}
C {gnd.sym} 410 -400 0 0 {name=l3 lab=GND}
C {gnd.sym} 950 -400 0 1 {name=l4 lab=GND}
C {noconn.sym} 890 -510 0 1 {name=l5}
C {noconn.sym} 890 -590 0 1 {name=l6}
C {opin.sym} 270 -550 0 1 {name=p2 lab=usb_iom}
C {opin.sym} 270 -470 0 1 {name=p3 lab=usb_iop}
