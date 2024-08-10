import "DPI-C" function void get_pc(input reg [31:0] pc []);
module top (
    input        clk,
    input        rst
);
    reg  [31:0] pc; 
    initial get_pc(pc);
    wire [31:0] inst;
    //control_gen
    wire [2:0] Type;
    wire Rd_Wr;
    wire Rd_Mem_ALU;
    wire [1:0] ALU_src1_key;
    wire [1:0] ALU_src2_key;
    wire [3:0] ALU_control;
    wire [2:0] Pc_control;
    wire Pc_normal;
    wire Mem_wr;
    wire [2:0] Mem_bits;
    //imm
    wire [31:0] imm;
    //pc_gen
    wire [31:0] R_rs1;
    wire [31:0] R_rs2;
    wire [31:0] pc_next;
    //ALU
    wire [31:0] ALU_src_1;
    wire [31:0] ALU_src_2;
    wire [31:0] ALU_result;
    ebreak e_i(
        .inst_i(inst),
        .clk(clk)
    );
    control_gen cont_i(
        .op(inst[6:0]),
        .func3(inst[14:12]),
        .func7(inst[31:25]),
        .Type(Type),
        .Rd_Wr(Rd_Wr),
        .Rd_Mem_ALU(Rd_Mem_ALU),
        .ALU_src1_key(ALU_src1_key),
        .ALU_src2_key(ALU_src2_key),
        .ALU_control(ALU_control),
        .Pc_control(Pc_control),
        .Pc_normal(Pc_normal),
        .Mem_wr(Mem_wr),
        .Mem_bits(Mem_bits)
    );
    imm_gen imm_i(
        .inst(inst),
        .Type(Type),
        .imm(imm)
    );
    pc_gen pc_i(
        .Pc_normal(Pc_normal),
        .pc(pc),
        .Pc_control(Pc_control),
        .imm(imm),
        .R_rs1(R_rs1),
        .R_rs2(R_rs2),
        .pc_next(pc_next)
    );
    MuxKey #(
       .NR_KEY(3),
       .KEY_LEN(2),
       .DATA_LEN(32)
   ) alu_s1(
       .key(ALU_src1_key),
       .out(ALU_src_1),
       .lut({2'b00,32'b0,2'b01,pc,2'b10,R_rs1})
   );
     MuxKey #(
       .NR_KEY(3),
       .KEY_LEN(2),
       .DATA_LEN(32)
   ) alu_s2(
       .key(ALU_src2_key),
       .out(ALU_src_2),
       .lut({2'b00,imm,2'b01,R_rs2,2'b10,32'd4})
   );
   ALU a_i (
        .ALU_control(ALU_control),
        .src1(ALU_src_1),
        .src2(ALU_src_2),
        .ALU_result(ALU_result)
   );
    RegisterFile #(
        .ADDR_WIDTH(5),
        .DATA_WIDTH(32))
    r_i (
        .clk(clk),
        .rst(rst),
        .wdata(ALU_result),
        .waddr(inst[11:7]),
        .raddr1(inst[19:15]),
        .rdata1(R_rs1),
        .raddr2(inst[24:20]),
        .rdata2(R_rs2),
        .wen(Rd_Wr)
    );
    MEM mem(
        .ren(1'b1),
        .wen(1'b0),
        .raddr(pc),
        .rdata(inst),
        .waddr('b0),
        .wdata('b0),
        .mask('b0)
    );
    always @(posedge clk)
    begin
        if(rst) pc<=32'h80000000;
        else
            pc<= pc_next;
    end
endmodule

