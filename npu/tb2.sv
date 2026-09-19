`timescale 1ns/1ps

module tb2;
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

    // Instanciação da NPU Ternária do Arthur (main2.sv)
    npu_ternary dut (
        .clk(clk),
        .rst(rst),
        .start(start),
        .bias(bias),
        .done(done),
        .result(result)
    );

    // Função auxiliar para exibir código ternário de forma legível
    function automatic string tern_str(input logic [1:0] code);
        case (code)
            2'd1: return "+1";
            2'd0: return " 0";
            2'd3: return "-1";
            default: return " 0 (res)";
        endcase
    endfunction

    // Monitoramento ciclo a ciclo durante o cálculo
    always @(posedge clk) begin
        if (start) begin
            $display("   [Ciclo %0d] i=%0d | a[%0d]=%3d | w[%0d]=%s (cod=%0d) | mult=%4d | acc=%4d",
                     $time/10, dut.i, dut.i, dut.ram_a[dut.i], dut.i, tern_str(dut.ram_b[dut.i]), dut.ram_b[dut.i], dut.mult, dut.acc);
        end
    end

    initial begin
        $timeformat(-9, 0, " ns", 6);
        $dumpfile("build/dump2.vcd");
        $dumpvars(0, tb2);

        $display("=========================================================================");
        $display("🚀 SIMULAÇÃO NPU TERNÁRIA (Arthur - main2.sv) - Sem Multiplicadores");
        $display("=========================================================================");

        // Reset inicial
        rst = 1; start = 0; bias = 0;
        #20; rst = 0; #10;

        // ---------------------------------------------------------------------
        // TESTE 1: Pesos Ternários (+1, 0, -1, +1)
        // ---------------------------------------------------------------------
        dut.ram_a[0] = 32'sd10; dut.ram_b[0] = 2'd1; // +1
        dut.ram_a[1] = 32'sd20; dut.ram_b[1] = 2'd0; //  0
        dut.ram_a[2] = 32'sd30; dut.ram_b[2] = 2'd3; // -1
        dut.ram_a[3] = 32'sd40; dut.ram_b[3] = 2'd1; // +1
        bias = 32'sd5;

        $display("\n-------------------------------------------------------------------------");
        $display("📥 TESTE 1: Visão Geral de Entradas do Neurônio Ternário");
        $display("   Ativações (A): [ %0d, %0d, %0d, %0d ]",
                 dut.ram_a[0], dut.ram_a[1], dut.ram_a[2], dut.ram_a[3]);
        $display("   Pesos     (W): [ %s, %s, %s, %s ] (Códigos brutos: [%0d, %0d, %0d, %0d])",
                 tern_str(dut.ram_b[0]), tern_str(dut.ram_b[1]), tern_str(dut.ram_b[2]), tern_str(dut.ram_b[3]),
                 dut.ram_b[0], dut.ram_b[1], dut.ram_b[2], dut.ram_b[3]);
        $display("   Bias         : %0d", bias);
        $display("   Fórmula      : (10 * +1) + (20 * 0) + (30 * -1) + (40 * +1) + 5 = 10 + 0 - 30 + 40 + 5 = 25");
        $display("-------------------------------------------------------------------------");
        $display("⏱️  Passo a Passo da Execução:");

        @(negedge clk);
        start = 1;

        @(posedge clk);
        while (!done) @(posedge clk);

        @(negedge clk);
        start = 0;

        #1;
        $display("🎯 Resultado Final : %0d (Esperado: 25)", result);
        if (result === 32'sd25) begin
            $display("✅ STATUS: TESTE 1 TERNÁRIO PASSOU!");
        end else begin
            $display("❌ STATUS: TESTE 1 TERNÁRIO FALHOU!");
        end

        // Reset entre testes
        #10; rst = 1; #10; rst = 0; #10;

        // ---------------------------------------------------------------------
        // TESTE 2: Ativações Negativas + Ternário (-1, +1, 0, -1)
        // ---------------------------------------------------------------------
        dut.ram_a[0] = -32'sd15; dut.ram_b[0] = 2'd3; // -1
        dut.ram_a[1] =  32'sd25; dut.ram_b[1] = 2'd1; // +1
        dut.ram_a[2] = -32'sd35; dut.ram_b[2] = 2'd0; //  0
        dut.ram_a[3] =  32'sd45; dut.ram_b[3] = 2'd3; // -1
        bias = -32'sd10;

        $display("\n-------------------------------------------------------------------------");
        $display("📥 TESTE 2: Visão Geral com Ativações Negativas + Ternário");
        $display("   Ativações (A): [ %0d, %0d, %0d, %0d ]",
                 dut.ram_a[0], dut.ram_a[1], dut.ram_a[2], dut.ram_a[3]);
        $display("   Pesos     (W): [ %s, %s, %s, %s ] (Códigos brutos: [%0d, %0d, %0d, %0d])",
                 tern_str(dut.ram_b[0]), tern_str(dut.ram_b[1]), tern_str(dut.ram_b[2]), tern_str(dut.ram_b[3]),
                 dut.ram_b[0], dut.ram_b[1], dut.ram_b[2], dut.ram_b[3]);
        $display("   Bias         : %0d", bias);
        $display("   Fórmula      : (-15 * -1) + (25 * +1) + (-35 * 0) + (45 * -1) + (-10) = 15 + 25 + 0 - 45 - 10 = -15");
        $display("-------------------------------------------------------------------------");
        $display("⏱️  Passo a Passo da Execução:");

        @(negedge clk);
        start = 1;

        @(posedge clk);
        while (!done) @(posedge clk);

        @(negedge clk);
        start = 0;

        #1;
        $display("🎯 Resultado Final : %0d (Esperado: -15)", result);
        if (result === -32'sd15) begin
            $display("✅ STATUS: TESTE 2 TERNÁRIO PASSOU!");
        end else begin
            $display("❌ STATUS: TESTE 2 TERNÁRIO FALHOU!");
        end

        #30;
        $display("\n=========================================================================");
        $display("🎉 Simulação NPU Ternária Finalizada!");
        $display("=========================================================================");
        $finish;
    end
endmodule
