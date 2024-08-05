module top (
    input        clk,
    input        rst,
    input [31:0] inst,
    output reg  [31:0] pc 
);
    wire [ 6:0] optcode;
    wire [ 2:0] funcode;
    wire [ 4:0] rd;
    wire [ 4:0] rs1;
    wire [31:0] imm;
    wire [31:0] rdata1;
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
    RegisterFile #(
        .ADDR_WIDTH(5),
        .DATA_WIDTH(32))
    r_i (
        .clk(clk),
        .rst(rst),
        .wdata(rdata1+imm),
        .waddr(rd),
        .raddr(rs1),
        .rdata(rdata1),
        .wen(optcode == 7'b0010011 & funcode == 3'b000)
    );
    always @(posedge clk) begin
        if (rst) pc <= 32'h80000000;
        else begin
            pc <= pc + 32'd4;
        end
    end
endmodule

