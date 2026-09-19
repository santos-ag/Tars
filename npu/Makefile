# Detecção automática de ferramentas no PATH ou via Nix Flake (..#hardware)
HAVE_IVERILOG := $(shell command -v iverilog 2> /dev/null)

ifeq ($(HAVE_IVERILOG),)
  RUN = nix develop ..\#hardware -c
else
  RUN =
endif

BUILD_DIR = build

.PHONY: all sim sim2 test wave wave2 lint clean

all: test

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Simulação NPU Binária (main.sv + tb.sv)
sim: $(BUILD_DIR)
	$(RUN) iverilog -g2012 -o $(BUILD_DIR)/sim_binary.out main.sv tb.sv
	$(RUN) vvp $(BUILD_DIR)/sim_binary.out

# Simulação NPU Ternária (main2.sv + tb2.sv)
sim2: $(BUILD_DIR)
	$(RUN) iverilog -g2012 -o $(BUILD_DIR)/sim_ternary.out main2.sv tb2.sv
	$(RUN) vvp $(BUILD_DIR)/sim_ternary.out

# Roda ambas as simulações
test: sim sim2

# Visualizar ondas GTKWave
wave: sim
	$(RUN) gtkwave $(BUILD_DIR)/dump.vcd &

wave2: sim2
	$(RUN) gtkwave $(BUILD_DIR)/dump2.vcd &

# Análise estática com Verilator
lint:
	$(RUN) verilator --lint-only -Wall -Wno-DECLFILENAME -Wno-TIMESCALEMOD main.sv tb.sv
	$(RUN) verilator --lint-only -Wall -Wno-DECLFILENAME -Wno-TIMESCALEMOD main2.sv tb2.sv

clean:
	rm -rf $(BUILD_DIR) *.vvp
