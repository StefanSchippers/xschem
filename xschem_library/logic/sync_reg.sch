v {xschem version=2.9.5_RC5 file_version=1.1}
G {
process(CK, RESET) begin
  if RESET = '1' then
    D <= (others => '0');
  elsif CK'event and CK = '1' and LOAD = '1' then
    D <= DATA_IN;
  end if;
end process;

DATA_OUT <= D after delay;
}
V {reg [width-1:0] D;

always @(posedge CK or posedge RESET) begin
  if(RESET) begin
     D <= 0;
  end
  else begin
    if(LOAD) begin
      D <= DATA_IN;
    end
  end
end

assign #del DATA_OUT=D;
}
S {}
E {}
C {opin.sym} 440 -230 0 0 {name=p3 lab=DATA_OUT[width-1:0] verilog_type=wire}
C {ipin.sym} 280 -210 0 0 {name=p2 lab=CK}
C {ipin.sym} 280 -190 0 0 {name=p4 lab=RESET}
C {ipin.sym} 280 -250 0 0 {name=p7 lab=DATA_IN[width-1:0]}
C {ipin.sym} 280 -230 0 0 {name=p1 lab=LOAD}
C {verilog_timescale.sym} 360 -577.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {title.sym} 160 -30 0 0 {name=l2}
C {arch_declarations.sym} 360 -350 0 0 {  signal d : std_logic_vector(width-1 downto 0);
}
C {use.sym} 360 -460 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
