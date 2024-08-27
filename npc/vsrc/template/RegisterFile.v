import "DPI-C" function void get_regs(input logic [31:0] regs[]);
module RegisterFile #(
    ADDR_WIDTH = 1,
    DATA_WIDTH = 1
) (
    input                  clk,
    input                  rst_n,
    input [DATA_WIDTH-1:0] wdata,
    input [ADDR_WIDTH-1:0] waddr,
    input [ADDR_WIDTH-1:0] raddr1,
    input [ADDR_WIDTH-1:0] raddr2,
    input                  wen,
    output [DATA_WIDTH-1:0] rdata1,
    output [DATA_WIDTH-1:0] rdata2
);
    integer i;
    initial get_regs(rf);
    assign rdata1= rf[raddr1];
    assign rdata2= rf[raddr2];
    reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) 
            for (i=0;i<2**ADDR_WIDTH;i=i+1) //赋初值
                rf[i]<='d0;
        else
        begin 
            if (wen && (waddr != 5'b0 )) rf[waddr] <= wdata;
        end
    end
endmodule

