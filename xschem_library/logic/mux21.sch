v {xschem version=3.4.6RC file_version=1.2
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
G {

a: process( G , D, RST )
begin
if( RST = '1') then
  Q <= '0' after delay ;
  QN <= '1' after delay ;
elsif ( G = '1' ) then
  Q <= D after delay ;
  QN <= not D after delay ;
end if ;
end process ;

 }
K {}
V {
assign #del Z = S ? B : A; 
}
S {}
E {}
C {opin.sym} 280 -320 0 0 {name=p5 lab=Z}
C {ipin.sym} 120 -320 0 0 {name=p2 lab=S}
C {use.sym} 70 -720 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {ipin.sym} 120 -440 0 0 {name=p14 lab=A}
C {title.sym} 160 -30 0 0 {name=l17}
C {ipin.sym} 120 -380 0 0 {name=p3 lab=B}
C {noconn.sym} 280 -320 2 1 {name=l4}
C {noconn.sym} 120 -440 2 0 {name=l2}
C {noconn.sym} 120 -380 2 0 {name=l3}
C {noconn.sym} 120 -320 2 0 {name=l5}
