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

process

variable buf: LINE;
variable last : time;
variable val : real;

begin
  if now = 0 ns then
    last := now;
    USC.cap <=cap;
    USC.conduct <=0.0;
    USC.value <= 0.0;
  end if;
  wait on  USC until last /=now;
  last := now;
  WRITE(buf,string'("start real_capa process"));
  WRITELINE(output,buf);

   USC.conduct <= 0.0;
   USC.cap <= cap ;
   
   val := USC.value'LAST_VALUE;
   if now = 0 ns then
     USC.value <= 0.0;
   else
     USC.value <= val;
   end if;
end process;
}
K {}
V {}
S {}
E {}
C {iopin.sym} 50 -80 0 0 {name=p1 lab=USC sig_type=rreal}
C {use.sym} -120 -340 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;

library work;
use work.rrreal_pkg.all;

}
