module npu_ternary (
  input logic clk,
  input logic rst,
  input logic start,
  input logic signed [31:0] bias,

  output logic done,
  output logic signed [31:0] result
  
);

logic signed [31:0] ram_a [0:3];
logic [1:0] ram_b [0:3];

logic [1:0] i;

logic signed [31:0] mult;
logic signed [31:0] acc;

always_comb begin
  
  case (ram_b[i])
      2'd0: mult = 32'sd0;
      2'd1: mult = ram_a[i];
      2'd3: mult = -ram_a[i];
      default: mult = 32'sd0;
  endcase

end

always_ff @(posedge clk or posedge rst) begin
  if (rst) begin
    i <= 0;
    acc <=0;
    result <= 0;
    done <= 0;
  end
  else if(start)begin
    done<=0;
    if(i==3)begin
      result <=acc + mult + bias;
      acc <= 0;
      done<=1;
    end else begin
      acc <= acc + mult;
    end
    i<=i+1;
  end

  
end
endmodule
