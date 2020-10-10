v {xschem version=2.9.5_RC5 file_version=1.1}
G {}
V {// This is a transport (non inertial) delay
reg x;
always @(inp) x <= #del inp;
assign outp = x;

}
S {}
E {}
C {ipin.sym} 60 -50 0 0 {name=p1 lab=inp}
C {opin.sym} 160 -50 0 0 {name=p2 lab=outp verilog_type=wire}
