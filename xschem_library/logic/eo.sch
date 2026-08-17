v {xschem version=3.4.8RC file_version=1.3
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2026 Stefan Frederik Schippers
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
G {
Y <=  A xor B after delay ;}
K {}
V {
assign #del Y=A^B;}
S {}
F {}
E {}
C {ipin.sym} 150 -320 0 0 {name=p2 lab=A}
C {ipin.sym} 150 -350 0 0 {name=p3 lab=B}
C {opin.sym} 920 -350 0 0 {name=p0 lab=Y verilog_type=wire}
C {use.sym} 150 -650 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {title.sym} 160 -30 0 0 {name=l2}
C {noconn.sym} 920 -350 0 0 {name=l1}
C {noconn.sym} 150 -350 0 1 {name=l3}
C {noconn.sym} 150 -320 0 1 {name=l4}
