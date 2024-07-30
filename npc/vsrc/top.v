module top (
    input        clk,
    input        rst,
    input [31:0] inst,
    output reg  [31:0] pc
);
    reg  [31:0] gpr     [0:31];
    wire [ 6:0] optcode;
    wire [ 2:0] funcode;
    wire [ 4:0] rd;
    wire [ 4:0] rs1;
    wire [31:0] imm;
    IDU idu_i (
        .inst   (inst),
        .optcode(optcode),
        .funcode(funcode),
        .rd     (rd),
        .rs1    (rs1),
        .imm    (imm)
    );
    ebreak e_i(
        .inst_i(inst)
    );
    always @(posedge clk) begin
        if (rst) gpr[0] <= 32'b0;
        else begin
            if (optcode == 7'b0010011 & funcode == 3'b000) gpr[rd] <= gpr[rs1] + imm;
        end
    end
    always @(posedge clk) begin
        if (rst) pc <= 32'h80000000;
        else begin
            pc <= pc + 32'd4;
        end
    end
endmodule

