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
G {
process(CK, RESET) begin
  if RESET = '1' then
    D <= (others => '0');
  elsif CK'event and CK = '1' and LOAD = '1' then
    D <= DATA_IN;
  end if;
end process;

DATA_OUT <= D after delay;
}
V {reg [width-1:0] D;

always @(posedge CK or posedge RESET) begin
  if(RESET) begin
     D <= 0;
  end
  else begin
    if(LOAD) begin
      D <= DATA_IN;
    end
  end
end

assign #del DATA_OUT=D;
}
S {}
E {}
C {opin.sym} 440 -230 0 0 {name=p3 lab=DATA_OUT[width-1:0] verilog_type=wire}
C {ipin.sym} 280 -210 0 0 {name=p2 lab=CK}
C {ipin.sym} 280 -190 0 0 {name=p4 lab=RESET}
C {ipin.sym} 280 -250 0 0 {name=p7 lab=DATA_IN[width-1:0]}
C {ipin.sym} 280 -230 0 0 {name=p1 lab=LOAD}
C {verilog_timescale.sym} 360 -577.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {title.sym} 160 -30 0 0 {name=l2}
C {arch_declarations.sym} 360 -350 0 0 {  signal d : std_logic_vector(width-1 downto 0);
}
C {use.sym} 360 -460 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
