module control_gen (
    input  [6:0] op,
    input  [2:0] func3,
    input  [6:0] func7,
    output [2:0] Type,
    output       Rd_Wr,
    output       Rd_Mem_ALU,
    output [1:0] ALU_src1_key,  //00 0;
    output [1:0] ALU_src2_key,  //00 imm;
    output [3:0] ALU_control,   //0000 +;0001 s<;0010 u<;0011 +0;0100 |;0101 &；0110 <<;0111 >>;1000 >>>;1001 -;
    output [2:0] Pc_control,
    output       Pc_normal,
    output       Mem_wr,
    output [2:0] Mem_bits
);
    localparam TypeI = 3'b000;
    localparam TypeU = 3'b001;
    localparam TypeS = 3'b010;
    localparam TypeJ = 3'b011;
    localparam TypeR = 3'b100;
    localparam TypeB = 3'b101;
    localparam TypeN = 3'b111;
    assign Type = (op == 7'b0110111 || op == 7'b0010111) ? TypeU : 
                  (op == 7'b0010011 || op == 7'b0000011 || op == 7'b1100111) ? TypeI : 
                  (op == 7'b0010011 || op == 7'b0110011) ? TypeR :
                  (op == 7'b1100011) ? TypeB :
                  (op == 7'b0100011) ? TypeS :
                  (op == 7'b1101111) ? TypeJ : TypeN;
    assign Rd_Wr = (Type == TypeU) || (Type == TypeI) || (Type == TypeJ) || (Type == TypeR);
    assign Rd_Mem_ALU = (op == 7'b0000011);
    assign Pc_normal = !((op == 7'b1100011) || (op == 7'b1101111) || (op == 7'b1100111));
    assign Pc_control = (op == 7'b1100011) ? func3 : (op == 7'b1101111) ? 3'b010 : (op == 7'b1100111) ? 3'b011 : 3'b000;
    assign Mem_wr = Type == TypeS;
    //ALU_control 0000 +;0001 s<;0010 u<;0011 +0;0100 |;0101 &；0110 <<;0111 >>;1000 >>>;1001 -;1010 *;1011 /;1100 %;1101 mulh;1110 remu;1111 divu
    //ALU_src1_key 00 0;01 pc; 10 R_src1;
    //ALU_src2_key 00 imm;01 R_src2;10 4 
    assign {ALU_control,ALU_src1_key,ALU_src2_key} = (op == 7'b0110111) ? {4'b0000,2'b00,2'b00}:
                                               (op == 7'b0010111) ? {4'b0000,2'b01,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b000) ? {4'b0000,2'b10,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b010) ? {4'b0001,2'b10,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b011) ? {4'b0010,2'b10,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b100) ? {4'b0011,2'b10,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b110) ? {4'b0100,2'b10,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b111) ? {4'b0101,2'b10,2'b00}:
                                               (op == 7'b0010011 && func3 == 3'b001) ? {4'b0110,2'b10,2'b00}:
                                               (func7[5]==1'b0 && op == 7'b0010011 && func3 == 3'b101) ? {4'b0111,2'b10,2'b00}:
                                               (func7[5]==1'b1 && op == 7'b0010011 && func3 == 3'b101) ? {4'b1000,2'b10,2'b00}:
                                               (func7[0]==1'b0 && func7[5]==1'b0 && op == 7'b0110011 && func3 == 3'b000) ? {4'b0000,2'b10,2'b01}:
                                               (func7[0]==1'b0 && func7[5]==1'b1 && op == 7'b0110011 && func3 == 3'b000) ? {4'b1001,2'b10,2'b01}:
                                               (func7[0]==1'b1 && op == 7'b0110011 && func3 == 3'b000) ? {4'b1010,2'b10,2'b01}:
                                               (func7[0]==1'b0 && op == 7'b0110011 && func3 == 3'b001) ? {4'b0110,2'b10,2'b01}:
                                               (func7[0]==1'b1 && op == 7'b0110011 && func3 == 3'b001) ? {4'b1101,2'b10,2'b01}:
                                               (op == 7'b0110011 && func3 == 3'b010) ? {4'b0001,2'b10,2'b01}:
                                               (op == 7'b0110011 && func3 == 3'b011) ? {4'b0010,2'b10,2'b01}:
                                               (func7[0]==1'b0 && op == 7'b0110011 && func3 == 3'b100) ? {4'b0011,2'b10,2'b01}:
                                               (func7[0]==1'b1 && op == 7'b0110011 && func3 == 3'b100) ? {4'b1011,2'b10,2'b01}:
                                               (func7[0]==1'b0 && func7[5]==1'b0 && op == 7'b0110011 && func3 == 3'b101) ? {4'b0111,2'b10,2'b01}:
                                               (func7[0]==1'b0 && func7[5]==1'b1 && op == 7'b0110011 && func3 == 3'b101) ? {4'b1000,2'b10,2'b01}:
                                               (func7[0]==1'b1 && op == 7'b0110011 && func3 == 3'b101) ? {4'b1111,2'b10,2'b01}:
                                               (func7[0]==1'b0 && op == 7'b0110011 && func3 == 3'b110) ? {4'b0100,2'b10,2'b01}:
                                               (func7[0]==1'b1 && op == 7'b0110011 && func3 == 3'b110) ? {4'b1100,2'b10,2'b01}:
                                               (func7[0]==1'b0 && op == 7'b0110011 && func3 == 3'b111) ? {4'b0101,2'b10,2'b01}:
                                               (func7[0]==1'b1 && op == 7'b0110011 && func3 == 3'b111) ? {4'b1110,2'b10,2'b01}:
                                               (op == 7'b1101111) ? {4'b0000,2'b01,2'b10}:
                                               (op == 7'b0100011 || op == 7'b0000011) ? {4'b0000,2'b10,2'b00}:
                                               (op == 7'b1100111 && func3 == 3'b000) ? {4'b0000,2'b01,2'b10}:8'b0;
    assign Mem_bits = (op == 7'b0100011 || op == 7'b0000011) ? func3 : 3'b000;
endmodule
