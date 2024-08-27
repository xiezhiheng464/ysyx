module ALU (
    input      [ 3:0] ALU_control,
    input      [31:0] src1,
    input      [31:0] src2,
    output reg [31:0] ALU_result
);
    wire [63:0] mulh;
    assign mulh = ($signed(src1) * $signed(src2));
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
            4'b1010: ALU_result = src1 * src2;
            4'b1011: ALU_result = src1 / src2;
            4'b1100: ALU_result = src1 % src2;
            4'b1101: ALU_result = (mulh[63:32]);
            4'b1110: ALU_result = $unsigned(src1) % $unsigned(src2);
            4'b1111: ALU_result = $unsigned(src1) / $unsigned(src2);
            //4'b1101: ALU_result = (((src1) * (src2)) >> 32); 
            default: ALU_result = 32'b0;
        endcase
    end
endmodule
