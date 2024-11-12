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
T {Copyright (C) 2008 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General Public License,
either verion 2 or (at your choice) any later version.} 40 -150 0 0 0.4 0.4 {}
C {ipin.sym} 280 -270 0 0 {name=p1 lab=ibcd[1:0]}
C {opin.sym} 500 -270 0 0 {name=p2 lab=oseg[6:0] verilog_type=reg}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {code.sym} 320 -310 0 0 {name=CODE value="// Copyright (C) 2008 DJ Delorie <dj delorie com>
// Distributed under the terms of the GNU General Public License,
// either verion 2 or (at your choice) any later version.

always @ (ibcd)
begin
  case (ibcd)  // abcdefg
    0 : oseg = 7'b1111110;
    1 : oseg = 7'b0110000;
    2 : oseg = 7'b1101101;
    default : oseg = 0;
  endcase
end
"}
