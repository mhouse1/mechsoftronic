
--*****************************************************************************
--*  DEFINE: Library                                                          *
--*****************************************************************************
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


--*****************************************************************************
--*  DEFINE: Entity                                                           *
--*****************************************************************************

entity mynios2 is
  port ( 
         --
         -- Input clock 
         --
         CLK_50      : in  std_logic;
        
         --
         -- EPCS
         --
         EPCS_DCLK     : out   std_logic;
         EPCS_SCE     : out   std_logic;
         EPCS_SDO     : out   std_logic;
         EPCS_DATA0    : in    std_logic
                  
       );
end entity mynios2;


--*****************************************************************************
--*  DEFINE: Architecture                                                     *
--*****************************************************************************

architecture syn of mynios2 is

   --
   -- Define all components which are included here
   --
   
    component mynios2 is
        port (
            clk_clk       : in  std_logic := 'X'; -- clk
            reset_reset_n : in  std_logic := 'X'; -- reset_n
            epcs_dclk     : out std_logic;        -- dclk
            epcs_sce      : out std_logic;        -- sce
            epcs_sdo      : out std_logic;        -- sdo
            epcs_data0    : in  std_logic := 'X'  -- data0
        );
    end component mynios2;

      
   --
   -- Define all local signals (like static data) here
   --
   signal clk_10     : std_logic;
   signal clk_sys    : std_logic;
   signal pll_locked : std_logic;
   signal sdram_ba   : std_logic_vector(1 downto 0);
   signal sdram_dqm  : std_logic_vector(1 downto 0);
   signal vs_pio     : std_logic_vector(2 downto 0);
   signal spi_cs_n   : std_logic;
	  
begin

    u0 : component mynios2
        port map (
            clk_clk       => CLK_50,       --   clk.clk
--            reset_reset_n => CONNECTED_TO_reset_reset_n, -- reset.reset_n
            epcs_dclk     => EPCS_DCLK,     --  epcs.dclk
            epcs_sce      => EPCS_SCE,      --      .sce
            epcs_sdo      => EPCS_SDO,      --      .sdo
            epcs_data0    => EPCS_DATA0     --      .data0
        );



end architecture syn;

-- *** EOF ***
