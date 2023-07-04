	soc_system u0 (
		.audio_0_external_interface_ADCDAT     (<connected-to-audio_0_external_interface_ADCDAT>),     //     audio_0_external_interface.ADCDAT
		.audio_0_external_interface_ADCLRCK    (<connected-to-audio_0_external_interface_ADCLRCK>),    //                               .ADCLRCK
		.audio_0_external_interface_BCLK       (<connected-to-audio_0_external_interface_BCLK>),       //                               .BCLK
		.audio_0_external_interface_DACDAT     (<connected-to-audio_0_external_interface_DACDAT>),     //                               .DACDAT
		.audio_0_external_interface_DACLRCK    (<connected-to-audio_0_external_interface_DACLRCK>),    //                               .DACLRCK
		.audio_i2c_config_SDAT                 (<connected-to-audio_i2c_config_SDAT>),                 //               audio_i2c_config.SDAT
		.audio_i2c_config_SCLK                 (<connected-to-audio_i2c_config_SCLK>),                 //                               .SCLK
		.button_0_external_connection_export   (<connected-to-button_0_external_connection_export>),   //   button_0_external_connection.export
		.clk_clk                               (<connected-to-clk_clk>),                               //                            clk.clk
		.hex_0_external_connection_export      (<connected-to-hex_0_external_connection_export>),      //      hex_0_external_connection.export
		.hex_1_external_connection_export      (<connected-to-hex_1_external_connection_export>),      //      hex_1_external_connection.export
		.hex_2_external_connection_export      (<connected-to-hex_2_external_connection_export>),      //      hex_2_external_connection.export
		.hex_3_external_connection_export      (<connected-to-hex_3_external_connection_export>),      //      hex_3_external_connection.export
		.hex_4_external_connection_export      (<connected-to-hex_4_external_connection_export>),      //      hex_4_external_connection.export
		.hex_5_external_connection_export      (<connected-to-hex_5_external_connection_export>),      //      hex_5_external_connection.export
		.hps_0_ddr_mem_a                       (<connected-to-hps_0_ddr_mem_a>),                       //                      hps_0_ddr.mem_a
		.hps_0_ddr_mem_ba                      (<connected-to-hps_0_ddr_mem_ba>),                      //                               .mem_ba
		.hps_0_ddr_mem_ck                      (<connected-to-hps_0_ddr_mem_ck>),                      //                               .mem_ck
		.hps_0_ddr_mem_ck_n                    (<connected-to-hps_0_ddr_mem_ck_n>),                    //                               .mem_ck_n
		.hps_0_ddr_mem_cke                     (<connected-to-hps_0_ddr_mem_cke>),                     //                               .mem_cke
		.hps_0_ddr_mem_cs_n                    (<connected-to-hps_0_ddr_mem_cs_n>),                    //                               .mem_cs_n
		.hps_0_ddr_mem_ras_n                   (<connected-to-hps_0_ddr_mem_ras_n>),                   //                               .mem_ras_n
		.hps_0_ddr_mem_cas_n                   (<connected-to-hps_0_ddr_mem_cas_n>),                   //                               .mem_cas_n
		.hps_0_ddr_mem_we_n                    (<connected-to-hps_0_ddr_mem_we_n>),                    //                               .mem_we_n
		.hps_0_ddr_mem_reset_n                 (<connected-to-hps_0_ddr_mem_reset_n>),                 //                               .mem_reset_n
		.hps_0_ddr_mem_dq                      (<connected-to-hps_0_ddr_mem_dq>),                      //                               .mem_dq
		.hps_0_ddr_mem_dqs                     (<connected-to-hps_0_ddr_mem_dqs>),                     //                               .mem_dqs
		.hps_0_ddr_mem_dqs_n                   (<connected-to-hps_0_ddr_mem_dqs_n>),                   //                               .mem_dqs_n
		.hps_0_ddr_mem_odt                     (<connected-to-hps_0_ddr_mem_odt>),                     //                               .mem_odt
		.hps_0_ddr_mem_dm                      (<connected-to-hps_0_ddr_mem_dm>),                      //                               .mem_dm
		.hps_0_ddr_oct_rzqin                   (<connected-to-hps_0_ddr_oct_rzqin>),                   //                               .oct_rzqin
		.hps_0_io_hps_io_emac1_inst_TX_CLK     (<connected-to-hps_0_io_hps_io_emac1_inst_TX_CLK>),     //                       hps_0_io.hps_io_emac1_inst_TX_CLK
		.hps_0_io_hps_io_emac1_inst_TXD0       (<connected-to-hps_0_io_hps_io_emac1_inst_TXD0>),       //                               .hps_io_emac1_inst_TXD0
		.hps_0_io_hps_io_emac1_inst_TXD1       (<connected-to-hps_0_io_hps_io_emac1_inst_TXD1>),       //                               .hps_io_emac1_inst_TXD1
		.hps_0_io_hps_io_emac1_inst_TXD2       (<connected-to-hps_0_io_hps_io_emac1_inst_TXD2>),       //                               .hps_io_emac1_inst_TXD2
		.hps_0_io_hps_io_emac1_inst_TXD3       (<connected-to-hps_0_io_hps_io_emac1_inst_TXD3>),       //                               .hps_io_emac1_inst_TXD3
		.hps_0_io_hps_io_emac1_inst_RXD0       (<connected-to-hps_0_io_hps_io_emac1_inst_RXD0>),       //                               .hps_io_emac1_inst_RXD0
		.hps_0_io_hps_io_emac1_inst_MDIO       (<connected-to-hps_0_io_hps_io_emac1_inst_MDIO>),       //                               .hps_io_emac1_inst_MDIO
		.hps_0_io_hps_io_emac1_inst_MDC        (<connected-to-hps_0_io_hps_io_emac1_inst_MDC>),        //                               .hps_io_emac1_inst_MDC
		.hps_0_io_hps_io_emac1_inst_RX_CTL     (<connected-to-hps_0_io_hps_io_emac1_inst_RX_CTL>),     //                               .hps_io_emac1_inst_RX_CTL
		.hps_0_io_hps_io_emac1_inst_TX_CTL     (<connected-to-hps_0_io_hps_io_emac1_inst_TX_CTL>),     //                               .hps_io_emac1_inst_TX_CTL
		.hps_0_io_hps_io_emac1_inst_RX_CLK     (<connected-to-hps_0_io_hps_io_emac1_inst_RX_CLK>),     //                               .hps_io_emac1_inst_RX_CLK
		.hps_0_io_hps_io_emac1_inst_RXD1       (<connected-to-hps_0_io_hps_io_emac1_inst_RXD1>),       //                               .hps_io_emac1_inst_RXD1
		.hps_0_io_hps_io_emac1_inst_RXD2       (<connected-to-hps_0_io_hps_io_emac1_inst_RXD2>),       //                               .hps_io_emac1_inst_RXD2
		.hps_0_io_hps_io_emac1_inst_RXD3       (<connected-to-hps_0_io_hps_io_emac1_inst_RXD3>),       //                               .hps_io_emac1_inst_RXD3
		.hps_0_io_hps_io_sdio_inst_CMD         (<connected-to-hps_0_io_hps_io_sdio_inst_CMD>),         //                               .hps_io_sdio_inst_CMD
		.hps_0_io_hps_io_sdio_inst_D0          (<connected-to-hps_0_io_hps_io_sdio_inst_D0>),          //                               .hps_io_sdio_inst_D0
		.hps_0_io_hps_io_sdio_inst_D1          (<connected-to-hps_0_io_hps_io_sdio_inst_D1>),          //                               .hps_io_sdio_inst_D1
		.hps_0_io_hps_io_sdio_inst_CLK         (<connected-to-hps_0_io_hps_io_sdio_inst_CLK>),         //                               .hps_io_sdio_inst_CLK
		.hps_0_io_hps_io_sdio_inst_D2          (<connected-to-hps_0_io_hps_io_sdio_inst_D2>),          //                               .hps_io_sdio_inst_D2
		.hps_0_io_hps_io_sdio_inst_D3          (<connected-to-hps_0_io_hps_io_sdio_inst_D3>),          //                               .hps_io_sdio_inst_D3
		.hps_0_io_hps_io_uart0_inst_RX         (<connected-to-hps_0_io_hps_io_uart0_inst_RX>),         //                               .hps_io_uart0_inst_RX
		.hps_0_io_hps_io_uart0_inst_TX         (<connected-to-hps_0_io_hps_io_uart0_inst_TX>),         //                               .hps_io_uart0_inst_TX
		.hps_0_io_hps_io_i2c0_inst_SDA         (<connected-to-hps_0_io_hps_io_i2c0_inst_SDA>),         //                               .hps_io_i2c0_inst_SDA
		.hps_0_io_hps_io_i2c0_inst_SCL         (<connected-to-hps_0_io_hps_io_i2c0_inst_SCL>),         //                               .hps_io_i2c0_inst_SCL
		.hps_0_io_hps_io_gpio_inst_GPIO35      (<connected-to-hps_0_io_hps_io_gpio_inst_GPIO35>),      //                               .hps_io_gpio_inst_GPIO35
		.hps_0_io_hps_io_gpio_inst_GPIO48      (<connected-to-hps_0_io_hps_io_gpio_inst_GPIO48>),      //                               .hps_io_gpio_inst_GPIO48
		.hps_0_io_hps_io_gpio_inst_GPIO53      (<connected-to-hps_0_io_hps_io_gpio_inst_GPIO53>),      //                               .hps_io_gpio_inst_GPIO53
		.hps_0_io_hps_io_gpio_inst_GPIO54      (<connected-to-hps_0_io_hps_io_gpio_inst_GPIO54>),      //                               .hps_io_gpio_inst_GPIO54
		.leds_0_external_connection_export     (<connected-to-leds_0_external_connection_export>),     //     leds_0_external_connection.export
		.pll_0_sdram_clk                       (<connected-to-pll_0_sdram_clk>),                       //                    pll_0_sdram.clk
		.reset_reset_n                         (<connected-to-reset_reset_n>),                         //                          reset.reset_n
		.sdram_controller_0_wire_addr          (<connected-to-sdram_controller_0_wire_addr>),          //        sdram_controller_0_wire.addr
		.sdram_controller_0_wire_ba            (<connected-to-sdram_controller_0_wire_ba>),            //                               .ba
		.sdram_controller_0_wire_cas_n         (<connected-to-sdram_controller_0_wire_cas_n>),         //                               .cas_n
		.sdram_controller_0_wire_cke           (<connected-to-sdram_controller_0_wire_cke>),           //                               .cke
		.sdram_controller_0_wire_cs_n          (<connected-to-sdram_controller_0_wire_cs_n>),          //                               .cs_n
		.sdram_controller_0_wire_dq            (<connected-to-sdram_controller_0_wire_dq>),            //                               .dq
		.sdram_controller_0_wire_dqm           (<connected-to-sdram_controller_0_wire_dqm>),           //                               .dqm
		.sdram_controller_0_wire_ras_n         (<connected-to-sdram_controller_0_wire_ras_n>),         //                               .ras_n
		.sdram_controller_0_wire_we_n          (<connected-to-sdram_controller_0_wire_we_n>),          //                               .we_n
		.switches_0_external_connection_export (<connected-to-switches_0_external_connection_export>), // switches_0_external_connection.export
		.audio_pll_0_audio_clk_clk             (<connected-to-audio_pll_0_audio_clk_clk>)              //          audio_pll_0_audio_clk.clk
	);

