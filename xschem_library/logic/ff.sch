v {xschem version=2.9.5_RC5 file_version=1.1}
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
V {reg iQ;
always @(posedge CK or posedge RST) begin
  if(RST)    iQ=0;
  else     iQ=D;
end

assign #del Q=iQ;
}
S {}
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
