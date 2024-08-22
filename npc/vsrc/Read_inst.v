import "DPI-C" function void pmem_read_inst(
    input  int  raddr,
    output int  rdata,
);
module Read_inst(
    input [31:0] pc,
    output [31:0] inst
);
    always@(*)
        pmem_read_inst(pc,inst);
endmodule 
