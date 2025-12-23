module regfile4x4 (
    input  wire        clk,
    input  wire        RegWrite,        // Global write enable
    input  wire [1:0]  Write_register,  // Register select (2 bits)
    input  wire [3:0]  write_data,      // Data to write

    output wire [3:0]  reg0_out,
    output wire [3:0]  reg1_out,
    output wire [3:0]  reg2_out,
    output wire [3:0]  reg3_out
);

    // Internal registers
    reg [3:0] register0;
    reg [3:0] register1;
    reg [3:0] register2;
    reg [3:0] register3;

    // 2-to-4 Decoder
    wire [3:0] decoder_out;
    assign decoder_out[0] = (Write_register == 2'b00);
    assign decoder_out[1] = (Write_register == 2'b01);
    assign decoder_out[2] = (Write_register == 2'b10);
    assign decoder_out[3] = (Write_register == 2'b11);

    // Write logic
    always @(posedge clk) begin
        if (RegWrite && decoder_out[0]) register0 <= write_data;
        if (RegWrite && decoder_out[1]) register1 <= write_data;
        if (RegWrite && decoder_out[2]) register2 <= write_data;
        if (RegWrite && decoder_out[3]) register3 <= write_data;
    end

    // Register outputs
    assign reg0_out = register0;
    assign reg1_out = register1;
    assign reg2_out = register2;
    assign reg3_out = register3;

endmodule

module top (
    // LED outputs
    output wire rs2_3,
    output wire rs2_2,
    output wire rs2_1,
    output wire rs2_0,

    output wire rs1_3,
    output wire rs1_2,
    output wire rs1_1,
    output wire rs1_0,

    // Switch inputs
    input wire ALU_data3,
    input wire ALU_data2,
    input wire ALU_data1,
    input wire ALU_data0,

    input wire Read_register1,
    input wire Read_register0,
    input wire Write_register1,
    input wire Write_register0,
    input wire RegWrite,
    input wire clk
);

    // Internal buses
    wire [3:0] alu_data;
    wire [1:0] write_register;
    wire [3:0] rs1;
    wire [3:0] rs2;
    wire [3:0] reg0_out, reg1_out, reg2_out, reg3_out;

    // Group switch inputs into buses
    assign alu_data       = {ALU_data3, ALU_data2, ALU_data1, ALU_data0};
    assign write_register = {Write_register1, Write_register0};

    // Drive LEDs from internal buses
    assign {rs1_3, rs1_2, rs1_1, rs1_0} = rs1;
    assign {rs2_3, rs2_2, rs2_1, rs2_0} = rs2;

    // Register file instance
    regfile4x4 reg_bank (
        .clk(clk),
        .RegWrite(RegWrite),
        .Write_register(write_register),
        .write_data(alu_data),
        .reg0_out(reg0_out),
        .reg1_out(reg1_out),
        .reg2_out(reg2_out),
        .reg3_out(reg3_out)
    );

    // External-style multiplexers (74HC157 behavior)
    // RS1 selects between register 0 and 1
    assign rs1 = Read_register0 ? reg1_out : reg0_out;

    // RS2 selects between register 2 and 3
    assign rs2 = Read_register1 ? reg3_out : reg2_out;

endmodule
