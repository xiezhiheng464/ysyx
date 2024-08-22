
import "DPI-C" function void pmem_read(
    input  int  raddr,
    output int  rdata,
    input  byte mask
);
import "DPI-C" function void pmem_write(
    input int  waddr,
    input int  wdata,
    input byte mask
);
module MEM (
    input ren,
    input wen,
    input clk,
    input [31:0] raddr_waddr,
    output reg [31:0] rdata,
    input [31:0] wdata,
    input [2:0] Mem_bits
);
    always @(posedge clk) begin
        if (ren) pmem_read(raddr_waddr, rdata, {5'b00000, Mem_bits});
        else rdata = 32'b0;
        if (wen) pmem_write(raddr_waddr, wdata, {5'b00000, Mem_bits});
    end
endmodule

