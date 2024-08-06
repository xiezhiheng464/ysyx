import "DPI-C" function void ebreak();
module ebreak(
    input wire [31:0] inst_i
);
always @(*) begin
    if(inst_i == 32'h00100073) 
        ebreak();       
end
endmodule
