`timescale 1ns/1ps

module tb;
    logic        clk;
    logic        rst;
    logic        start;
    logic signed [31:0] bias;
    logic        done;
    logic signed [31:0] result;

    // Gerador de clock de 100 MHz
    initial begin
        clk = 0;
    end
    always #5 clk = ~clk;

    // Instanciação da NPU do Arthur (main.sv)
    npu dut (
        .clk(clk),
        .rst(rst),
        .start(start),
        .bias(bias),
        .done(done),
        .result(result)
    );

    // Monitoramento ciclo a ciclo durante o cálculo
    always @(posedge clk) begin
        if (start) begin
            $display("   [Ciclo %0d] i=%0d | a[%0d]=%3d | b[%0d]=%3d | mult=%4d | acc=%4d",
                     $time/10, dut.i, dut.i, dut.ram_a[dut.i], dut.i, dut.ram_b[dut.i], dut.mult, dut.acc);
        end
    end

    initial begin
        $timeformat(-9, 0, " ns", 6);
        $dumpfile("build/dump.vcd");
        $dumpvars(0, tb);

        $display("=========================================================================");
        $display("🚀 SIMULAÇÃO NPU BINÁRIA (Arthur - main.sv) - Produto Escalar 4D + Bias");
        $display("=========================================================================");

        // Reset inicial
        rst = 1; start = 0; bias = 0;
        #20; rst = 0; #10;

        // ---------------------------------------------------------------------
        // TESTE 1: Positivos
        // ---------------------------------------------------------------------
        dut.ram_a[0] = 32'sd1; dut.ram_b[0] = 32'sd5;
        dut.ram_a[1] = 32'sd2; dut.ram_b[1] = 32'sd6;
        dut.ram_a[2] = 32'sd3; dut.ram_b[2] = 32'sd7;
        dut.ram_a[3] = 32'sd4; dut.ram_b[3] = 32'sd8;
        bias = 32'sd10;

        $display("\n-------------------------------------------------------------------------");
        $display("📥 TESTE 1: Visão Geral de Entradas do Neurônio");
        $display("   Ativações (A): [ %0d, %0d, %0d, %0d ]",
                 dut.ram_a[0], dut.ram_a[1], dut.ram_a[2], dut.ram_a[3]);
        $display("   Pesos     (B): [ %0d, %0d, %0d, %0d ]",
                 dut.ram_b[0], dut.ram_b[1], dut.ram_b[2], dut.ram_b[3]);
        $display("   Bias         : %0d", bias);
        $display("   Fórmula      : (1*5) + (2*6) + (3*7) + (4*8) + 10 = 5 + 12 + 21 + 32 + 10 = 80");
        $display("-------------------------------------------------------------------------");
        $display("⏱️  Passo a Passo da Execução:");

        @(negedge clk);
        start = 1;

        @(posedge clk);
        while (!done) @(posedge clk);

        @(negedge clk);
        start = 0;

        #1;
        $display("🎯 Resultado Final : %0d (Esperado: 80)", result);
        if (result === 32'sd80) begin
            $display("✅ STATUS: TESTE 1 PASSOU!");
        end else begin
            $display("❌ STATUS: TESTE 1 FALHOU!");
        end

        // Reset entre testes
        #10; rst = 1; #10; rst = 0; #10;

        // ---------------------------------------------------------------------
        // TESTE 2: Negativos com Sinal
        // ---------------------------------------------------------------------
        dut.ram_a[0] = -32'sd2; dut.ram_b[0] =  32'sd4;
        dut.ram_a[1] =  32'sd3; dut.ram_b[1] = -32'sd2;
        dut.ram_a[2] = -32'sd4; dut.ram_b[2] =  32'sd3;
        dut.ram_a[3] =  32'sd5; dut.ram_b[3] = -32'sd1;
        bias = -32'sd5;

        $display("\n-------------------------------------------------------------------------");
        $display("📥 TESTE 2: Visão Geral de Entradas do Neurônio (Com Sinal)");
        $display("   Ativações (A): [ %0d, %0d, %0d, %0d ]",
                 dut.ram_a[0], dut.ram_a[1], dut.ram_a[2], dut.ram_a[3]);
        $display("   Pesos     (B): [ %0d, %0d, %0d, %0d ]",
                 dut.ram_b[0], dut.ram_b[1], dut.ram_b[2], dut.ram_b[3]);
        $display("   Bias         : %0d", bias);
        $display("   Fórmula      : (-2*4) + (3*-2) + (-4*3) + (5*-1) + (-5) = -8 - 6 - 12 - 5 - 5 = -36");
        $display("-------------------------------------------------------------------------");
        $display("⏱️  Passo a Passo da Execução:");

        @(negedge clk);
        start = 1;

        @(posedge clk);
        while (!done) @(posedge clk);

        @(negedge clk);
        start = 0;

        #1;
        $display("🎯 Resultado Final : %0d (Esperado: -36)", result);
        if (result === -32'sd36) begin
            $display("✅ STATUS: TESTE 2 PASSOU!");
        end else begin
            $display("❌ STATUS: TESTE 2 FALHOU!");
        end

        #30;
        $display("\n=========================================================================");
        $display("🎉 Simulação NPU Binária Finalizada!");
        $display("=========================================================================");
        $finish;
    end
endmodule
