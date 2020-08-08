v {xschem version=2.9.5 file_version=1.1}
G {}
V {}
S {}
E {}
T {Copyright (C) 2008 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General Public License,
either verion 2 or (at your choice) any later version.} 40 -150 0 0 0.4 0.4 {}
C {ipin.sym} 330 -270 0 0 {name=p1 lab=ibcd[3:0]}
C {opin.sym} 500 -270 0 0 {name=p2 lab=oseg[6:0] verilog_type=reg}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {code.sym} 360 -310 0 0 {name=CODE value="// Copyright (C) 2008 DJ Delorie <dj delorie com>
// Distributed under the terms of the GNU General Public License,
// either verion 2 or (at your choice) any later version.

always @ (ibcd)
begin
  case (ibcd)  // abcdefg
    0 : oseg = 7'b1111110;
    1 : oseg = 7'b0110000;
    2 : oseg = 7'b1101101;
    3 : oseg = 7'b1111001;
    4 : oseg = 7'b0110011;
    5 : oseg = 7'b1011011;
    6 : oseg = 7'b1011111;
    7 : oseg = 7'b1110000;
    8 : oseg = 7'b1111111;
    9 : oseg = 7'b1111011;
    default : oseg = 0;
  endcase
end

"}
