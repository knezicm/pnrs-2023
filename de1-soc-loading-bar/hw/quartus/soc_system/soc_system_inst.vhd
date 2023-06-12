	component soc_system is
		port (
			clk_clk                           : in  std_logic                    := 'X'; -- clk
			leds_0_external_connection_export : out std_logic_vector(9 downto 0);        -- export
			reset_reset_n                     : in  std_logic                    := 'X'  -- reset_n
		);
	end component soc_system;

	u0 : component soc_system
		port map (
			clk_clk                           => CONNECTED_TO_clk_clk,                           --                        clk.clk
			leds_0_external_connection_export => CONNECTED_TO_leds_0_external_connection_export, -- leds_0_external_connection.export
			reset_reset_n                     => CONNECTED_TO_reset_reset_n                      --                      reset.reset_n
		);

