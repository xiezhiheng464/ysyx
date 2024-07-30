module IDU (
    input  [31:0] inst,
    output [ 6:0] optcode,
    output [ 2:0] funcode,
    output [ 4:0] rd,
    output [ 4:0] rs1,
    output [31:0] imm
);
    localparam TypeI = 3'b000;
    localparam TypeU = 3'b001;
    localparam TypeS = 3'b010;
    localparam TypeJ = 3'b011;
    //localparam TypeR = 3'b100;
    localparam TypeB = 3'b101;
    localparam TypeN = 3'b111;
    wire [ 2:0] Type;
    wire [31:0] imm_I;
    wire [31:0] imm_S;
    wire [31:0] imm_B;
    wire [31:0] imm_U;
    wire [31:0] imm_J;
    assign optcode = inst[6:0];
    assign funcode = inst[14:12];
    assign Type    = optcode == 7'b0010011 ? TypeI : TypeN;
    assign rd      = inst[11:7];
    assign rs1     = inst[19:15];
    // 所有立即数都是符号扩展
    assign imm_I   = {{20{inst[31]}}, inst[31:20]};
    assign imm_S   = {{20{inst[31]}}, inst[31:25], inst[11:7]};
    assign imm_B   = {{19{inst[31]}}, inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};
    assign imm_U   = {inst[31:12], 12'b0};
    assign imm_J   = {{11{inst[31]}}, inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};

    // imm MUX
    // NR_KEY: 键值对的数量 KEY_LEN: 键值的位宽
    MuxKey #(
        .NR_KEY  (6),
        .KEY_LEN (3),
        .DATA_LEN(32)
    ) imm_mux (
        .out(imm),
        .key(Type),
        .lut({TypeN, {32{1'b0}}, TypeI, imm_I, TypeS, imm_S, TypeB, imm_B, TypeU, imm_U, TypeJ, imm_J})
    );

endmodule
