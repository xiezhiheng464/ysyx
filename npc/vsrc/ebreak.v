import "DPI-C" function void ebreak();
module ebreak (
    input [31:0] inst_i,
    input        clk
);
    always @(posedge clk) begin
        if (inst_i == 32'h00100073) ebreak();
    end
endmodule
