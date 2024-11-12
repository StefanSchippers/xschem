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
variable del: time := 0.4 ns;
variable delay: time;
variable last : time;
variable lowvalue: real;

begin
wait on ING , USC.cap;

last := now;

delay := del * USC.cap;
print("start pump process: " & pump'PATH_NAME & " " & time'image(now) );

if(now=0 ns) then
      USC <= RREAL_0;
elsif ING'event and ING='1' then
      USC.conduct<=conduct;
      transition(USC,val,delay);
elsif ING'event and ING='0' then
      USC.conduct<=conduct;
      transition(USC,0.0,delay);
elsif USC.cap'event and USC.cap > USC.cap'last_value and ING='1' then
      lowvalue := USC.value * USC.cap'last_value / USC.cap;
      glitch(USC, lowvalue, val, delay);
end if;

end process;



}
K {}
V {}
S {}
E {}
C {iopin.sym} 50 -90 0 0 {name=p1 lab=USC sig_type="rreal"}
C {ipin.sym} -80 -90 0 0 {name=p2 lab=ING}
C {use.sym} -150 -260 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;

library work;
use work.rrreal_pkg.all;

}
