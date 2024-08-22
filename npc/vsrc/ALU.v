module ALU (
    input      [ 3:0] ALU_control,
    input      [31:0] src1,
    input      [31:0] src2,
    output reg [31:0] ALU_result
);
    always @(*) begin
        case (ALU_control)
            4'b0000: ALU_result = src1 + src2;
            4'b0001: ALU_result = (src1[31] > src2[31] || src1[30:0] < src2[30:0]) ? 32'b1 : 32'b0;
            4'b0010: ALU_result = (src1 < src2) ? 32'b1 : 32'b0;
            4'b0011: ALU_result = (src1 ^ src2);
            4'b0100: ALU_result = (src1 | src2);
            4'b0101: ALU_result = (src1 & src2);
            4'b0110: ALU_result = src1 << src2[4:0];
            4'b0111: ALU_result = src1 >> src2[4:0];
            4'b1000: ALU_result = ($signed(src1)) >>> src2[4:0];
            4'b1001: ALU_result = src1 - src2;
            default: ALU_result = 32'b0;
        endcase
    end
endmodule
