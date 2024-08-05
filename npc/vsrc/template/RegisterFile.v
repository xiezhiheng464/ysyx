import "DPI-C" function void get_regs(input logic [31:0] regs[]);
module RegisterFile #(
    ADDR_WIDTH = 1,
    DATA_WIDTH = 1
) (
    input                  clk,
    input                  rst,
    input [DATA_WIDTH-1:0] wdata,
    input [ADDR_WIDTH-1:0] waddr,
    input [ADDR_WIDTH-1:0] raddr,
    input                  wen,
    output [DATA_WIDTH-1:0] rdata
);
    integer i;
    initial get_regs(rf);
    assign rdata= rf[raddr];
    reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
    always @(posedge clk) begin
        if (rst) 
            for (i=0;i<2**ADDR_WIDTH;i=i+1) //赋初值
                rf[i]<='d0;
        else
        begin 
            if (wen) rf[waddr] <= wdata;
        end
    end
endmodule

