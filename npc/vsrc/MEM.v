
import "DPI-C" function void pmem_read(
  input int raddr, output int rdata);
import "DPI-C" function void pmem_write(
  input int waddr, input int wdata, input byte mask);
module MEM(ren, wen, raddr, rdata, waddr, wdata, mask);
  input             ren;
  input             wen;
  input  reg [31:0] raddr;
  output reg [31:0] rdata;
  input  reg [31:0] waddr;
  input  reg [31:0] wdata;
  input  reg [7:0]  mask;

  always @(*) begin
    if (ren) pmem_read(raddr, rdata);
    else rdata = 32'b0;
    if (wen) pmem_write(waddr, wdata, mask);
  end
endmodule
    
