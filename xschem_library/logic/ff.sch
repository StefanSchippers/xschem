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
a: process( CK , RST )
begin
if ( RST = '1' ) then
  Q <= '0' after delay ;
elsif  CK'event AND CK = '1'  then 
  Q <= D after delay ;
end if ;
end process ;

 }
K {}
V {reg iQ;
always @(posedge CK or posedge RST) begin
  if(RST)    iQ=0;
  else     iQ=D;
end

assign #del Q=iQ;
}
S {}
F {}
E {}
C {ipin.sym} 60 -150 0 0 {name=p1 lab=D}
C {opin.sym} 130 -130 0 0 {name=p2 verilog_type=wire lab=Q}
C {ipin.sym} 60 -130 0 0 {name=p3 lab=CK}
C {ipin.sym} 60 -110 0 0 {name=p5 lab=RST}
C {title.sym} 160 -30 0 0 {name=l2}
C {use.sym} 100 -440 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {noconn.sym} 130 -130 0 0 {name=l1}
C {noconn.sym} 60 -150 0 1 {name=l3}
C {noconn.sym} 60 -130 0 1 {name=l4}
C {noconn.sym} 60 -110 0 1 {name=l5}
