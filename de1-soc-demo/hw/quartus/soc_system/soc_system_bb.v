
module soc_system (
	buttons_0_external_connection_export,
	clk_clk,
	hex_0_external_connection_export,
	hex_1_external_connection_export,
	hex_2_external_connection_export,
	hex_3_external_connection_export,
	hex_4_external_connection_export,
	hex_5_external_connection_export,
	hps_0_ddr_mem_a,
	hps_0_ddr_mem_ba,
	hps_0_ddr_mem_ck,
	hps_0_ddr_mem_ck_n,
	hps_0_ddr_mem_cke,
	hps_0_ddr_mem_cs_n,
	hps_0_ddr_mem_ras_n,
	hps_0_ddr_mem_cas_n,
	hps_0_ddr_mem_we_n,
	hps_0_ddr_mem_reset_n,
	hps_0_ddr_mem_dq,
	hps_0_ddr_mem_dqs,
	hps_0_ddr_mem_dqs_n,
	hps_0_ddr_mem_odt,
	hps_0_ddr_mem_dm,
	hps_0_ddr_oct_rzqin,
	hps_0_io_hps_io_emac1_inst_TX_CLK,
	hps_0_io_hps_io_emac1_inst_TXD0,
	hps_0_io_hps_io_emac1_inst_TXD1,
	hps_0_io_hps_io_emac1_inst_TXD2,
	hps_0_io_hps_io_emac1_inst_TXD3,
	hps_0_io_hps_io_emac1_inst_RXD0,
	hps_0_io_hps_io_emac1_inst_MDIO,
	hps_0_io_hps_io_emac1_inst_MDC,
	hps_0_io_hps_io_emac1_inst_RX_CTL,
	hps_0_io_hps_io_emac1_inst_TX_CTL,
	hps_0_io_hps_io_emac1_inst_RX_CLK,
	hps_0_io_hps_io_emac1_inst_RXD1,
	hps_0_io_hps_io_emac1_inst_RXD2,
	hps_0_io_hps_io_emac1_inst_RXD3,
	hps_0_io_hps_io_sdio_inst_CMD,
	hps_0_io_hps_io_sdio_inst_D0,
	hps_0_io_hps_io_sdio_inst_D1,
	hps_0_io_hps_io_sdio_inst_CLK,
	hps_0_io_hps_io_sdio_inst_D2,
	hps_0_io_hps_io_sdio_inst_D3,
	hps_0_io_hps_io_uart0_inst_RX,
	hps_0_io_hps_io_uart0_inst_TX,
	hps_0_io_hps_io_gpio_inst_GPIO35,
	hps_0_io_hps_io_gpio_inst_GPIO53,
	hps_0_io_hps_io_gpio_inst_GPIO54,
	leds_0_external_connection_export,
	pll_0_sdram_clk,
	reset_reset_n,
	sdram_controller_0_wire_addr,
	sdram_controller_0_wire_ba,
	sdram_controller_0_wire_cas_n,
	sdram_controller_0_wire_cke,
	sdram_controller_0_wire_cs_n,
	sdram_controller_0_wire_dq,
	sdram_controller_0_wire_dqm,
	sdram_controller_0_wire_ras_n,
	sdram_controller_0_wire_we_n,
	switches_0_external_connection_export,
	leds_1_external_connection_export);	

	input	[3:0]	buttons_0_external_connection_export;
	input		clk_clk;
	output	[6:0]	hex_0_external_connection_export;
	output	[6:0]	hex_1_external_connection_export;
	output	[6:0]	hex_2_external_connection_export;
	output	[6:0]	hex_3_external_connection_export;
	output	[6:0]	hex_4_external_connection_export;
	output	[6:0]	hex_5_external_connection_export;
	output	[14:0]	hps_0_ddr_mem_a;
	output	[2:0]	hps_0_ddr_mem_ba;
	output		hps_0_ddr_mem_ck;
	output		hps_0_ddr_mem_ck_n;
	output		hps_0_ddr_mem_cke;
	output		hps_0_ddr_mem_cs_n;
	output		hps_0_ddr_mem_ras_n;
	output		hps_0_ddr_mem_cas_n;
	output		hps_0_ddr_mem_we_n;
	output		hps_0_ddr_mem_reset_n;
	inout	[31:0]	hps_0_ddr_mem_dq;
	inout	[3:0]	hps_0_ddr_mem_dqs;
	inout	[3:0]	hps_0_ddr_mem_dqs_n;
	output		hps_0_ddr_mem_odt;
	output	[3:0]	hps_0_ddr_mem_dm;
	input		hps_0_ddr_oct_rzqin;
	output		hps_0_io_hps_io_emac1_inst_TX_CLK;
	output		hps_0_io_hps_io_emac1_inst_TXD0;
	output		hps_0_io_hps_io_emac1_inst_TXD1;
	output		hps_0_io_hps_io_emac1_inst_TXD2;
	output		hps_0_io_hps_io_emac1_inst_TXD3;
	input		hps_0_io_hps_io_emac1_inst_RXD0;
	inout		hps_0_io_hps_io_emac1_inst_MDIO;
	output		hps_0_io_hps_io_emac1_inst_MDC;
	input		hps_0_io_hps_io_emac1_inst_RX_CTL;
	output		hps_0_io_hps_io_emac1_inst_TX_CTL;
	input		hps_0_io_hps_io_emac1_inst_RX_CLK;
	input		hps_0_io_hps_io_emac1_inst_RXD1;
	input		hps_0_io_hps_io_emac1_inst_RXD2;
	input		hps_0_io_hps_io_emac1_inst_RXD3;
	inout		hps_0_io_hps_io_sdio_inst_CMD;
	inout		hps_0_io_hps_io_sdio_inst_D0;
	inout		hps_0_io_hps_io_sdio_inst_D1;
	output		hps_0_io_hps_io_sdio_inst_CLK;
	inout		hps_0_io_hps_io_sdio_inst_D2;
	inout		hps_0_io_hps_io_sdio_inst_D3;
	input		hps_0_io_hps_io_uart0_inst_RX;
	output		hps_0_io_hps_io_uart0_inst_TX;
	inout		hps_0_io_hps_io_gpio_inst_GPIO35;
	inout		hps_0_io_hps_io_gpio_inst_GPIO53;
	inout		hps_0_io_hps_io_gpio_inst_GPIO54;
	output	[4:0]	leds_0_external_connection_export;
	output		pll_0_sdram_clk;
	input		reset_reset_n;
	output	[12:0]	sdram_controller_0_wire_addr;
	output	[1:0]	sdram_controller_0_wire_ba;
	output		sdram_controller_0_wire_cas_n;
	output		sdram_controller_0_wire_cke;
	output		sdram_controller_0_wire_cs_n;
	inout	[15:0]	sdram_controller_0_wire_dq;
	output	[1:0]	sdram_controller_0_wire_dqm;
	output		sdram_controller_0_wire_ras_n;
	output		sdram_controller_0_wire_we_n;
	input	[9:0]	switches_0_external_connection_export;
	output	[4:0]	leds_1_external_connection_export;
endmodule
