v {xschem version=2.9.6 file_version=1.1}
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
V {reg iQ;

always @( D or G or RST) begin

if( RST ) begin
  iQ <= 0;
end
else if(G) begin
  iQ <= D;
end

end

assign #del Q = iQ; 
assign #del QN = ~iQ;
}
S {}
E {}
C {opin.sym} 280 -320 0 0 {name=p5 lab=Q}
C {ipin.sym} 120 -320 0 0 {name=p2 lab=G}
C {use.sym} 70 -720 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {ipin.sym} 120 -440 0 0 {name=p14 lab=RST}
C {opin.sym} 280 -440 0 0 {name=p1 lab=QN}
C {title.sym} 160 -30 0 0 {name=l17}
C {ipin.sym} 120 -380 0 0 {name=p3 lab=D}
