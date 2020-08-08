v {xschem version=2.9.7 file_version=1.2}
G {}
V {// test}
S {* test}
E {}
T {rrreal type:
-----------

rreal is a record type containing voltage value, drive strength and
capacitive loading of an electrical node.

rrreal is a resolved subtype of rreal.

The resolution function invoked by the simulator updates
voltages, strengths and capacitive loading of all nodes.

this allows to simulate voltage transients, charge sharing,
floating conditions and more.
the example uses bidirectional analog switches
and simulates charge pumps which have a finite
driving capability (output impedance)} 10 -410 0 0 0.3 0.3 {}
T {VHDL DESIGN EXAMPLE} 140 -1290 0 0 1 1 {}
T {set netlist mode to VHDL
- create netlist
- simulate with ghdl
- view waveforms} 110 -1200 0 0 0.6 0.6 {}
N 830 -680 900 -680 {lab=VXS}
N 450 -680 510 -680 {lab=VX}
N 450 -680 450 -570 {lab=VX}
N 1230 -680 1240 -680 {lab=SP}
N 340 -680 450 -680 {lab=VX}
N 830 -680 830 -570 {lab=VXS}
N 1230 -680 1230 -570 {lab=SP}
N 470 -800 480 -800 {lab=VX2}
N 810 -800 810 -680 {lab=VXS}
N 780 -800 810 -800 {lab=VXS}
N 350 -910 470 -910 {lab=VX2}
N 470 -910 470 -800 {lab=VX2}
N 810 -680 830 -680 {lab=VXS}
N 1200 -680 1230 -680 {lab=SP}
N 340 -800 470 -800 {lab=VX2}
C {code.sym} 600 -200 0 0 {name=CODE
vhdl_ignore=false
value="
-- these assignments are done to have the voltage values available
-- in the waveform file
V_VX <= VX.value;
V_VX2 <= VX2.value;
V_VXS <= VXS.value;
V_SP <= SP.value;


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


"
 embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
format="
@value
"}
V {}
S {}
E {}
L 4 20 30 60 30 {}
L 4 20 40 40 40 {}
L 4 20 50 60 50 {}
L 4 20 60 50 60 {}
L 4 20 70 50 70 {}
L 4 20 80 90 80 {}
L 4 20 90 40 90 {}
L 4 20 20 70 20 {}
L 4 20 10 40 10 {}
L 4 100 10 110 10 {}
L 4 110 10 110 110 {}
L 4 20 110 110 110 {}
L 4 20 100 20 110 {}
L 4 100 0 100 100 {}
L 4 10 100 100 100 {}
L 4 10 0 10 100 {}
L 4 10 0 100 0 {}
T {@name} 15 -25 0 0 0.3 0.3 {}
]
C {use.sym} 840 -220 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;

library work;
use work.rrreal.all;

 -- embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=use
template="
    library ieee;
    use ieee.std_logic_1164.all;
    use ieee.std_logic_arith.all;
    use ieee.std_logic_unsigned.all;

"}
V {}
S {}
E {}
L 4 -0 -10 355 -10 {}
T {VHDL USE} 5 -25 0 0 0.3 0.3 {}
T {@prop_ptr} 45 5 0 0 0.2 0.2 {}
]
C {pump.sym} 250 -680 0 0 {name=x4 conduct="1.0/20000.0" val=4.5 embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname"
template="name=x1 val=4.5 conduct=10.0"
generic_type="conduct=real val=real"}
V {}
S {}
E {}
L 4 -80 -10 70 -10 {}
L 4 -80 10 70 10 {}
L 4 -80 -10 -80 10 {}
L 4 70 -10 70 10 {}
L 4 70 0 90 0 {}
L 4 -100 0 -80 0 {}
B 5 87.5 -2.5 92.5 2.5 {name=USC sig_type=rreal verilog_type=wire dir=inout }
B 5 -102.5 -2.5 -97.5 2.5 {name=ING sig_type=std_logic verilog_type=wire dir=in }
T {@symname} -45 -6 0 0 0.3 0.3 {}
T {@name} 75 -22 0 0 0.2 0.2 {}
T {USC} 65 -4 0 1 0.2 0.2 {}
T {ING} -75 -4 0 0 0.2 0.2 {}
T {conduct=@conduct
val=@val} -65 -36 0 0 0.2 0.2 {}
]
C {lab_pin.sym} 150 -680 0 0 {name=l4  lab=ING embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
]
C {switch_rreal.sch} 660 -670 0 0 {name=x5 del="2 ns" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
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
L 7 -140 7.5 -137.5 10 {}
L 7 -140 12.5 -137.5 10 {}
L 7 -142.5 -10 -140 -12.5 {}
L 7 -142.5 -10 -140 -7.5 {}
L 7 -140 -12.5 -137.5 -10 {}
L 7 -140 -7.5 -137.5 -10 {}
L 7 140 -12.5 142.5 -10 {}
L 7 140 -7.5 142.5 -10 {}
L 7 137.5 -10 140 -12.5 {}
L 7 137.5 -10 140 -7.5 {}
B 5 -152.5 7.5 -147.5 12.5 {name=ENAB dir=in }
B 5 147.5 -12.5 152.5 -7.5 {name=B dir=inout sig_type=rreal}
B 5 -152.5 -12.5 -147.5 -7.5 {name=A dir=inout sig_type=rreal}
T {@symname} -49.5 -6 0 0 0.3 0.3 {}
T {@name} 65 -32 0 0 0.2 0.2 {}
T {$ENAB} -146.25 -6.25 0 0 0.2 0.2 {}
T {$A} -146.25 -26.25 0 0 0.2 0.2 {}
T {$B} 146.25 -26.25 0 1 0.2 0.2 {}
]
C {lab_pin.sym} 510 -660 0 0 {name=l5  lab=SW embed=true}
C {real_capa.sym} 450 -540 0 0 {name=x3 cap=30.0 embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname"
template="name=x1 cap=10.0"
generic_type="cap=real"
}
V {}
S {}
E {}
L 4 0 5 0 30 {}
L 4 0 -30 0 -5 {}
L 4 -10 -5 10 -5 {}
L 4 -10 5 10 5 {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
L 4 -5 30 5 30 {}
L 4 0 35 5 30 {}
L 4 -5 30 0 35 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=USC sig_type=rreal verilog_type=wire dir=inout }
T {@symname} 14.5 -6 0 0 0.3 0.3 {}
T {@name} 15 -17 0 0 0.2 0.2 {}
T {USC} -5 -24 0 1 0.2 0.2 {}
T {@cap pF} 14.5 14 0 0 0.3 0.3 {}
]
C {real_capa.sym} 830 -540 0 0 {name=x1 cap=100.0 embed=true}
C {switch_rreal.sch} 1050 -670 0 0 {name=x2 del="2 ns" embed=true}
C {lab_pin.sym} 900 -660 0 0 {name=l2  lab=SW1 embed=true}
C {lab_pin.sym} 1240 -680 0 1 {name=l3  lab=SP sig_type=rrreal embed=true}
C {real_capa.sym} 1230 -540 0 0 {name=x6 cap=20.0 embed=true}
C {lab_wire.sym} 860 -680 0 1 {name=l6  lab=VXS sig_type=rrreal
 embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -2.5 -1.25 2 0 0.27 0.27 {}
]
C {pump.sym} 250 -800 0 0 {name=x7 conduct="1.0/40000.0" val=3.0 embed=true}
C {lab_pin.sym} 150 -800 0 0 {name=l7  lab=ING1 embed=true}
C {switch_rreal.sym} 630 -790 0 0 {name=x8 del="2 ns" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname"
template="name=x1 del=\\"2 ns\\""
generic_type="del=time"}
V {}
S {}
E {}
L 4 -130 -20 130 -20 {}
L 4 -130 20 130 20 {}
L 4 -130 -20 -130 20 {}
L 4 130 -20 130 20 {}
L 4 -150 10 -130 10 {}
L 4 130 -10 150 -10 {}
L 4 -150 -10 -130 -10 {}
B 5 -152.5 7.5 -147.5 12.5 {name=ENAB dir=in }
B 5 147.5 -12.5 152.5 -7.5 {name=B sig_type=rreal dir=inout }
B 5 -152.5 -12.5 -147.5 -7.5 {name=A sig_type=rreal dir=inout }
T {@symname} -49.5 -6 0 0 0.3 0.3 {}
T {@name} 135 -32 0 0 0.2 0.2 {}
T {ENAB} -125 6 0 0 0.2 0.2 {}
T {B} 125 -14 0 1 0.2 0.2 {}
T {A} -125 -14 0 0 0.2 0.2 {}
]
C {lab_pin.sym} 480 -780 0 0 {name=l0  lab=SW2 embed=true}
C {lab_wire.sym} 400 -800 0 1 {name=l8  lab=VX2 sig_type=rrreal
 embed=true}
C {real_capa.sym} 350 -880 0 0 {name=x9 cap=40.0 embed=true}
C {package_not_shown.sym} 830 -340 0 0 {
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
 -- embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=package
template="

-- THIS IS A TEMPLATE, REPLACE WITH ACTUAL CODE OR REMOVE INSTANCE!!
    library ieee;
    use ieee.std_logic_1164.all;
    use ieee.std_logic_arith.all;
    use ieee.std_logic_unsigned.all;
 
    package aaa is
      type real_vector is array(natural range <>) of real;
      constant dx : real := 0.001 ;
 
    procedure assegna(
        signal A      : inout real;
        signal A_OLD  : in real;
               A_VAL  : in real
    );
 
    end aaa; -- end package declaration
 
 
    package body aaa is
 
 
    procedure assegna(
        signal A      : inout real;
        signal A_OLD  : in real;
               A_VAL  : in real ) is
    constant tdelay: time := 0.01 ns;
    begin
     if (A /=  A_VAL) then
        A <= A_OLD+dx, A_VAL after tdelay;
     end if;
    end assegna;
 
 
    end aaa; -- end package body
"}
V {}
S {}
E {}
L 4 0 -10 355 -10 {}
T {PACKAGE} 5 -25 0 0 0.3 0.3 {}
T {HIDDEN} 135 -5 0 0 0.3 0.3 {}
]
C {title.sym} 160 -40 0 0 {name=l9 author="Stefan Schippers" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=logo
template="name=l1 author=\\"Stefan Schippers\\""
verilog_ignore=true
vhdl_ignore=true
spice_ignore=true
tedax_ignore=true}
V {}
S {}
E {}
L 6 225 0 1020 0 {}
L 6 -160 0 -95 0 {}
T {@schname} 235 5 0 0 0.4 0.4 {}
T {@author} 235 -25 0 0 0.4 0.4 {}
T {@time_last_modified} 1020 -20 0 1 0.4 0.3 {}
T {SCHEM} 5 -25 0 0 1 1 {}
P 5 13 5 -30 -25 0 5 30 -15 30 -35 10 -55 30 -75 30 -45 0 -75 -30 -55 -30 -35 -10 -15 -30 5 -30 {fill=true}
]
C {arch_declarations.sym} 830 -280 0 0 {
signal V_VX, V_VX2, V_VXS, V_SP: real;

 -- embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=arch_declarations
template="

signal AAA: std_logic;

"}
V {}
S {}
E {}
L 4 -0 -10 355 -10 {}
T {ARCHITECTURE DECLARATIONS} 5 -25 0 0 0.3 0.3 {}
T {HIDDEN} 45 5 0 0 0.2 0.2 {}
]
C {lab_wire.sym} 430 -680 0 1 {name=l1  lab=VX sig_type=rrreal  embed=true}
