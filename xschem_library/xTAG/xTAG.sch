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
V {}
S {}
E {}
T {JTAG (IEEE1149.1) consist of tdi, tdo, tck, tms, (trst)} 420 -360 0 0 0.4 0.4 {}
T {xTAG - usb to jtag interface} 490 -950 0 0 0.8 0.8 {}
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
N 280 -780 280 -580 {lab=jtag_power}
N 280 -780 480 -780 {lab=jtag_power}
N 430 -440 670 -440 {lab=trst_ttl}
N 430 -460 670 -460 {lab=tck_ttl}
N 430 -480 670 -480 {lab=tms_ttl}
N 430 -500 670 -500 {lab=tdo_ttl}
N 430 -520 670 -520 {lab=tdi_ttl}
N 780 -750 1020 -750 {lab=pon_reset_}
N 1020 -750 1020 -520 {lab=pon_reset_}
N 970 -480 1080 -480 {lab=usb_iop}
N 970 -440 1080 -440 {lab=usb_iom}
N 1230 -780 1230 -580 {lab=usb_power}
N 780 -780 1230 -780 {lab=usb_power}
N 970 -520 1020 -520 {lab=pon_reset_}
C {xTAG-psu.sym} 630 -780 0 0 {name=x4}
C {xTAG-jtagio.sym} 280 -480 0 0 {name=x2}
C {lab_pin.sym} 280 -780 0 0 {name=l4 sig_type=std_logic lab=jtag_power}
C {lab_wire.sym} 470 -520 0 1 {name=p1 lab=tdi_ttl}
C {lab_wire.sym} 470 -500 0 1 {name=p1 lab=tdo_ttl}
C {lab_wire.sym} 470 -480 0 1 {name=p1 lab=tms_ttl}
C {lab_wire.sym} 470 -460 0 1 {name=p1 lab=tck_ttl}
C {lab_wire.sym} 470 -440 0 1 {name=p1 lab=trst_ttl}
C {xTAG-ucont.sym} 820 -480 0 0 {name=x1}
C {xTAG-consio.sym} 1230 -480 0 0 {name=x3}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_wire.sym} 1060 -480 0 0 {name=p2 lab=usb_iop}
C {lab_wire.sym} 1060 -440 0 0 {name=p3 lab=usb_iom}
C {lab_pin.sym} 1020 -750 0 1 {name=l2 lab=pon_reset_}
C {lab_pin.sym} 1230 -780 0 1 {name=p5 lab=usb_power}
