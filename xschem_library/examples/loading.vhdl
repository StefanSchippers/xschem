
    library ieee, std;
    use std.textio.all;
 
    package rrreal is



    type rreal is
    record
      value : real;
      conduct : real;
      cap : real;
    end record;



    type rreal_vector is array (natural range <>) of rreal;
    function resolved_real( r: rreal_vector ) return rreal; 
    procedure print(s : in string);
    subtype rrreal is resolved_real rreal;
    type rrreal_vector is array (natural range <>) of rrreal;

    CONSTANT RREAL_X : rreal := rreal'(value=> 0.0, cap=>0.0, conduct=>-1.0);
    CONSTANT RREAL_Z : rreal := rreal'(value=> 0.0, cap=>0.0, conduct=>0.0);
    CONSTANT RREAL_0 : rreal := rreal'(value=> 0.0, cap=>0.0, conduct=>10.0);
    CONSTANT REAL_Z : real := 20.0;
    CONSTANT REAL_X : real := 20.0;


    procedure transition(
        signal sig: INOUT rreal; 
        constant endval: IN real; 
        constant del: IN time
    );

    procedure glitch(
        signal sig: INOUT rreal;
        constant lowval: IN real;
        constant endval: IN real;
        constant del: IN time
    );

    end rrreal; -- end package declaration
 
 
    package body rrreal is

procedure print(s : in string) is
variable outbuf: line;
begin
  write(outbuf, s);
  writeline(output, outbuf);
end procedure;

 
--      function resolved_real( r:rreal_vector) return rreal is
--      VARIABLE result : rreal := RREAL_Z;
--      begin
--        IF    (r'LENGTH = 1) THEN    RETURN r(r'LOW);
--        ELSE
--            FOR i IN r'RANGE LOOP
--              result.cap := result.cap + r(i).cap ;
--              IF r(i).value /=REAL_Z THEN
--                IF result.value /=REAL_Z THEN 
--                  result.value := REAL_X ;
--                ELSE
--                  result.value := r(i).value ;
--                END IF;
--              END IF ;
--            END LOOP;
--        END IF;
--        RETURN result;
--      end resolved_real;
 
    function resolved_real( r:rreal_vector) return rreal is
    VARIABLE result : rreal := RREAL_Z;
    variable vcapshare : real := 0.0;
    begin
      IF    (r'LENGTH = 1) THEN    RETURN r(r'LOW);
      ELSE
          FOR i IN r'RANGE LOOP
            if r(i).conduct = -1.0 then
              result := RREAL_X;
              exit;
            end if;
           
            -- only process initialized (valid) data
            if r(i).value > -30.0 and r(i).value < 30.0 then
              if r(i).cap  > -1.0e12 and r(i).cap  < 1.0e12 then
                if r(i).conduct  > -1.0e12 and r(i).conduct  < 1.0e12 then
                  vcapshare := vcapshare + r(i).value * r(i).cap;
                  result.value := result.value + r(i).value *  r(i).conduct;
                  result.cap := result.cap + r(i).cap ;
                  if(r(i).conduct > 0.0 ) then
                    -- result.conduct := result.conduct + 1.0/r(i).conduct ;
                    result.conduct := result.conduct + r(i).conduct ;
                  end if;
                end if;
              end if;
            end if;

          END LOOP;
      END IF;

      if result.conduct /= 0.0 then 
        result.value := result.value / result.conduct ;     -- conductance
        -- result.value := result.value * result.conduct ;     -- resistance
        -- result.conduct := 1.0 / result.conduct;
      elsif result.cap >0.0 then
        result.value := vcapshare / result.cap;
      else 
        result.value:=0.0;
      end if;

      RETURN result;
    end resolved_real;

    procedure transition(
        signal sig: INOUT rreal; 
        constant endval: IN real; 
        constant del: IN time) is

        variable step: real;
        variable startval: real;
        variable del2: time;
    begin
     del2 := del;
     if del2 = 0 fs then
       del2 := 1 ns;
     end if;
     startval := sig.value;
     step := (endval-startval);
     if abs(endval-startval) < 0.01 then --do not propagate events if endval very close to startval
       return;
     end if;
--       sig.value <= endval after del;
        sig.value <= startval,
                     startval+0.25*step after del2*0.1,
                     startval+0.45*step after del2*0.2,
                     startval+0.60*step after del2*0.3,
                     startval+0.72*step after del2*0.4,
                     startval+0.80*step after del2*0.5,
                     startval+0.86*step after del2*0.6,
                     startval+0.90*step after del2*0.7,
                     startval+0.94*step after del2*0.8,
                     startval+0.97*step after del2*0.9,
                     endval             after del2;
    end transition;


    procedure glitch(
        signal sig: INOUT rreal;
        constant lowval: IN real;
        constant endval: IN real;
        constant del: IN time) is

        variable step: real;
        variable step2: real;
        variable startval: real;
        variable del2 : time;
    begin
     del2 := del;
     if del2 = 0 fs then
       del2 := 1 ns;
     end if;
     startval := sig.value;
     step := (lowval-startval);
     step2 := (lowval-endval);
     if abs(lowval-startval) < 0.01 then --do not propagate events if endval very close to startval
       return;
     end if;
        sig.value <= 
--                     startval,
--                     startval+0.25*step after del*0.05,
--                     startval+0.45*step after del*0.1,
--                     startval+0.60*step after del*0.15,
--                     startval+0.72*step after del*0.2,
--                     startval+0.80*step after del*0.25,
--                     startval+0.86*step after del*0.3,
--                     startval+0.90*step after del*0.35,
--                     startval+0.94*step after del*0.4,
--                     startval+0.97*step after del*0.45,
--                     lowval after del*0.5,
--                     lowval-0.25*step2 after del*0.55,
--                     lowval-0.45*step2 after del*0.6,
--                     lowval-0.60*step2 after del*0.65,
--                     lowval-0.72*step2 after del*0.7,
--                     lowval-0.80*step2 after del*0.75,
--                     lowval-0.86*step2 after del*0.8,
--                     lowval-0.90*step2 after del*0.85,
--                     lowval-0.94*step2 after del*0.9,
--                     lowval-0.97*step2 after del*0.95,
--                     endval after del;
                     lowval,
                     lowval-0.25*step2 after del2*0.1,
                     lowval-0.45*step2 after del2*0.2,
                     lowval-0.60*step2 after del2*0.3,
                     lowval-0.72*step2 after del2*0.4,
                     lowval-0.80*step2 after del2*0.5,
                     lowval-0.86*step2 after del2*0.6,
                     lowval-0.90*step2 after del2*0.7,
                     lowval-0.94*step2 after del2*0.8,
                     lowval-0.97*step2 after del2*0.9,
                     endval after del2;


    end glitch;


    end rrreal; -- end package body

library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;

library work;
use work.rrreal.all;


entity loading is
end loading ;

architecture arch_loading of loading is

component pump 
generic (
val : real := 4.5 ;
conduct : real := 10.0
);
port (
  USC : inout rreal ;
  ING : in std_logic
);
end component ;

component switch_rreal 
generic (
del : time := 2 ns
);
port (
  ENAB : in std_logic ;
  B : inout rreal ;
  A : inout rreal
);
end component ;

component real_capa 
generic (
cap : real := 10.0
);
port (
  USC : inout rreal
);
end component ;



signal SW : std_logic ;
signal VXS : rrreal ;
signal SW1 : std_logic ;
signal VX : rrreal ;
signal SW2 : std_logic ;
signal ING1 : std_logic ;
signal ING : std_logic ;
signal SP : rrreal ;
signal VX2 : rrreal ;


begin
x4 : pump
generic map (
   conduct => 1.0/20000.0 ,
   val => 4.5
)
port map (
   USC => VX ,
   ING => ING
);

x5 : switch_rreal
generic map (
   del => 2 ns
)
port map (
   ENAB => SW ,
   B => VXS ,
   A => VX
);

x3 : real_capa
generic map (
   cap => 30.0
)
port map (
   USC => VX
);

x1 : real_capa
generic map (
   cap => 100.0
)
port map (
   USC => VXS
);

x2 : switch_rreal
generic map (
   del => 2 ns
)
port map (
   ENAB => SW1 ,
   B => SP ,
   A => VXS
);

x6 : real_capa
generic map (
   cap => 20.0
)
port map (
   USC => SP
);

x7 : pump
generic map (
   conduct => 1.0/40000.0 ,
   val => 3.0
)
port map (
   USC => VX2 ,
   ING => ING1
);

x8 : switch_rreal
generic map (
   del => 2 ns
)
port map (
   ENAB => SW2 ,
   B => VXS ,
   A => VX2
);

x9 : real_capa
generic map (
   cap => 40.0
)
port map (
   USC => VX2
);


 

process
begin
ING<='0';
ING1<='0';
SW <= '0';
SW1<='0';
SW2<='0';
--VX <= rreal'(4.5,10.0,0.0);
--VX2 <= rreal'(3.0, 5.0, 0.0);
wait for 200 ns;
ING1<='1';
wait for 200 ns;
ING<='1';
wait for 200 ns;
SW<='1';
wait for 200 ns;
SW2<='1';
wait for 200 ns;
SW1<='1';
wait for 200 ns;
SW1<='0';
wait for 200 ns;
SW2<='0';
wait for 200 ns;
SW1<='1';
wait for 200 ns;
SW<='1';
wait for 200 ns;
ING <='0';
wait for 200 ns;
SW1<= '0';
wait for 200 ns;
SW<='1';
wait for 200 ns;
ING<='1';
wait for 200 ns;
SW <= '0';
wait for 200 ns;
SW1<= '1';
wait for 200 ns;
ING<='1';
wait for 200 ns;
SW<='0';
wait for 200 ns;
SW1<='0';
wait for 200 ns;
SW<='1';
wait for 200 ns;
SW1<='1';
wait for 200 ns;
SW1<='0';
wait for 200 ns;
SW1<='1';
wait for 200 ns;



wait;
end process;


end arch_loading ;


-- expanding   symbol:  templates/pump # of pins=2

library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;
use work.rrreal.all;

library work;
use work.rrreal.all;


entity pump is
generic (
val : real := 4.5 ;
conduct : real := 10.0
);
port (
  USC : inout rreal ;
  ING : in std_logic
);
end pump ;

architecture arch_pump of pump is




begin


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




end arch_pump ;


-- expanding   symbol:  templates/switch_rreal # of pins=3

    library ieee,work;
    use ieee.std_logic_1164.all;
    use work.rrreal.all;
    use std.textio.all;

entity switch_rreal is
generic (
del : time := 2 ns
);
port (
  ENAB : in std_logic ;
  B : inout rreal ;
  A : inout rreal
);
end switch_rreal ;

architecture arch_switch_rreal of switch_rreal is




signal SCHEDULE : integer ;


procedure print2( signal x : in rreal)  is
begin
  print("       " & x'SIMPLE_NAME & ".value=" & real'image(x.value)& " t= " & time'image(now) );
  print("       " & x'SIMPLE_NAME & ".cap=" & real'image(x.cap) & " t= " & time'image(now));
  print("       " & x'SIMPLE_NAME & ".conduct=" & real'image(x.conduct) & " t= " & time'image(now));
end print2;



begin
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



end arch_switch_rreal ;


-- expanding   symbol:  templates/real_capa # of pins=1

library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;
use work.rrreal.all;

library work;
use work.rrreal.all;


entity real_capa is
generic (
cap : real := 10.0
);
port (
  USC : inout rreal
);
end real_capa ;

architecture arch_real_capa of real_capa is




begin


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
   
   val := USC'LAST_VALUE.value;
   if now = 0 ns then
     USC.value <= 0.0;
   else
     USC.value <= val;
   end if;
end process;

end arch_real_capa ;

