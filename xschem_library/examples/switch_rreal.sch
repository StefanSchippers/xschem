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

-- process
-- variable t : time;
-- variable C : rreal;
-- begin
--   wait on  A'transaction , B'transaction , ENAB'transaction until t /= now;
--   t := now;
-- 
--   A<=rreal'(0.0,0.0,0.0);
--   B<=rreal'(0.0,0.0,0.0);
--   for i in 1 to 6 loop
--     wait for 0 ns;
--   end loop;
-- 
--   if ENAB = '1' then
--     for i in 1 to 6 loop
--       C := resolved_real( (A, B) );
--       B <= C;
--       A <= C;
--       wait for 0 ns;
--     end loop;
--   end if;
-- 
-- end process;

process
  variable last : time;
  variable aa,bb : rreal;
begin

wait until ( SCHEDULE'transaction'event or A'event or B'event or ENAB'event ) and now>last;
last := now;
print(switch_rreal'PATH_NAME & " start switch process: " & time'image(now) );
if now= 0 ns then
  aa:=rreal'(0.0,0.0,0.0);
  bb:=rreal'(0.0,0.0,0.0);
  A <= aa ;
  B <= bb ;
elsif ENAB'event and ENAB='1' then SCHEDULE <= 4 after del;
elsif ENAB'event and ENAB='0' then SCHEDULE <= 3 after del;
elsif B'event  and ENAB='1' then SCHEDULE <= 2 after del;
elsif A'event  and ENAB='1' then SCHEDULE <= 1 after del;
elsif SCHEDULE'transaction'event and SCHEDULE=3 then -- ENAB=0 event
  print(switch_rreal'PATH_NAME & "     release outputs: " & time'image(now) );
  aa:=rreal'(0.0,0.0,0.0);
  bb:=rreal'(0.0,0.0,0.0);
  A <= aa ;
  B <= bb ;
elsif SCHEDULE'transaction'event  and SCHEDULE=4 then -- ENAB=1 event
  aa := B;
  bb := A;
  A <= aa ;
  print(switch_rreal'PATH_NAME & "     enab forcing A, done: " & time'image(now) );
  print2(A);
  B <= bb ;
  print(switch_rreal'PATH_NAME & "     enab forcing B, done: " & time'image(now) );
  print2(B);
elsif SCHEDULE'transaction'event and SCHEDULE=2 then -- B event
  print(switch_rreal'PATH_NAME & "     B'event: " & time'image(now) );
  aa.conduct := B.conduct - bb.conduct;
  aa.cap := B.cap - bb.cap;
  if aa.conduct /= 0.0 then
    aa.value := ( B.value * B.conduct - bb.value * bb.conduct) / aa.conduct;
  else
    aa.value := B.value;
  end if;
  A <= aa ;
elsif SCHEDULE'transaction'event  and SCHEDULE=1 then -- A event
  print(switch_rreal'PATH_NAME & "     A'event: " & time'image(now) );
  bb.conduct := A.conduct - aa.conduct;
  bb.cap := A.cap - aa.cap;
  if bb.conduct /= 0.0 then
    bb.value := ( A.value * A.conduct - aa.value * aa.conduct) / bb.conduct;
  else
    bb.value := A.value;
  end if;
  B <= bb ;
end if; -- now = 0
end process;
}
K {type=subcircuit
format="@name @pinlist @symname"
template="name=x1 del=\\"2 ns\\""
generic_type="del=time"}
V {}
S {}
E {}
L 4 -110 -20 110 -20 {}
L 4 -110 20 110 20 {}
L 4 -110 -20 -110 20 {}
L 4 110 -20 110 20 {}
L 4 110 -10 150 -10 {}
L 4 -150 -10 -110 -10 {}
L 4 -150 10 -110 10 {}
T {@symname} -49.5 -6 0 0 0.3 0.3 {}
T {@name} 65 -32 0 0 0.2 0.2 {}
C {ipin.sym} -150 10 0 0 {name=p3 lab=ENAB}
C {iopin.sym} -150 -10 0 1 {name=p2 lab=A sig_type=rreal}
C {arch_declarations.sym} 140 -160 0 0 {

signal SCHEDULE : integer ;


procedure print2( signal x : in rreal)  is
begin
  print("       " & x'SIMPLE_NAME & ".value=" & real'image(x.value)& " t= " & time'image(now) );
  print("       " & x'SIMPLE_NAME & ".cap=" & real'image(x.cap) & " t= " & time'image(now));
  print("       " & x'SIMPLE_NAME & ".conduct=" & real'image(x.conduct) & " t= " & time'image(now));
end print2;
}
C {iopin.sym} 150 -10 0 0 {name=p1 lab=B sig_type=rreal}
C {use.sym} -130 -260 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;

library work;
use work.rrreal_pkg.all;

}
