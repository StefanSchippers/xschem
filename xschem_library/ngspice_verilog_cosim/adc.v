// Digital control for a successive approximation ADC with switched capacitors.

`timescale 100ns/100ns

module adc(Clk, Comp, Start, Sample, Done, Result);
   parameter		     Bits=6;

   input wire                Clk, Comp, Start;
   output reg		     Sample, Done;
   output reg [Bits - 1 : 0] Result;

   reg [Bits - 1 : 0]	     SR;
   reg			     Running;

   initial begin
      $display("ADC simulation starting");
      Done = 0;
      Sample = 0;
      Result = 0;
      Running = 0;
   end

   always @(posedge(Clk)) begin
      if (Running) begin
	 if (Sample) begin
	    Sample <= 0;
	    SR[Bits - 1] <= 1;
	    Result[Bits - 1] <= 1;
	 end else if (SR != 0) begin
	    Result <= (Comp ? (Result & ~SR) : Result) | (SR >> 1);
	    SR <= SR >> 1;
	    if (SR == 1) begin
	       Running <= 0;
	       Done <= 1;
	    end
	 end
      end else if (Start) begin
	 Running <= 1;
	 Sample <= 1;
	 Done <= 0;
	 SR <= 0;
	 Result <= 0;
      end
   end
endmodule
