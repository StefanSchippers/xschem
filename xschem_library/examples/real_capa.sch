v {xschem version=2.9.5_RC5 file_version=1.1}
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
V {}
S {}
E {}
C {iopin.sym} 50 -80 0 0 {name=p1 lab=USC sig_type=rreal}
C {use.sym} -120 -340 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;
use work.rrreal.all;

library work;
use work.rrreal.all;

}
