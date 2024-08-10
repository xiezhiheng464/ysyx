module pc_gen(
    input Pc_normal,
    input [31:0] pc,
    input [2:0] Pc_control,
    input [31:0] imm,
    input [31:0] R_rs1,
    input [31:0] R_rs2,
    output [31:0] pc_next
);
    reg [31:0] pc_src1;
    reg [31:0] pc_src2;
    reg [31:0] pc_add;
    assign pc_src1 = (!Pc_normal && (Pc_control == 3'b011)) ? R_rs1 : pc ;
    assign pc_src2 = Pc_normal ? 'd4 :
             (Pc_control == 3'b010 || Pc_control == 3'b011) ? imm :
             (Pc_control == 3'b000) ? ((R_rs1 == R_rs2)?imm:'d4):
             (Pc_control == 3'b001) ? ((R_rs1 != R_rs2)?imm:'d4):
             (Pc_control == 3'b100) ? ((R_rs1[31] > R_rs2[31] || (R_rs1[31]==R_rs2[31] && R_rs1[30:0] < R_rs2[30:0]))?imm:'d4):
             (Pc_control == 3'b101) ? ((R_rs1[31] < R_rs2[31] || (R_rs1[31]==R_rs2[31] && R_rs1[30:0]>= R_rs2[30:0]))?imm:'d4):
             (Pc_control == 3'b110) ? ((R_rs1 <  R_rs2)?imm:'d4):
             (Pc_control == 3'b111) ? ((R_rs1 >= R_rs2)?imm:'d4):'d4;
    assign pc_add = pc_src1 + pc_src2 ;
    assign pc_next = (!Pc_normal && Pc_control == 3'b011)?{pc_add[31:1],1'b0}:pc_add;
endmodule

