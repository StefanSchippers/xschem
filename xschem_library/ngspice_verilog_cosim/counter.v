`timescale 1ns/1ns // needed for Icarus

module counter (
    input clk,
    output reg [3:0] count
);

initial begin
  $display("initial");
  count = 0;
end

always @(posedge clk) begin
    count <= count + 1;
    $display("clock event: count=%d", count);
end

endmodule
