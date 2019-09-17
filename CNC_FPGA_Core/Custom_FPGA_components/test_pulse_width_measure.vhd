--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   14:11:09 12/22/2013
-- Design Name:   
-- Module Name:   C:/Users/Mike/Dropbox/ECE446/lab 7/FSM/test_pulse_width_measure.vhd
-- Project Name:  FSM
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: pulse_width_measure
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY test_pulse_width_measure IS
END test_pulse_width_measure;
 
ARCHITECTURE behavior OF test_pulse_width_measure IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT pulse_width_measure
    PORT(
         clk_50mhz : IN  std_logic;
			Q1_O, Q2_O : out std_logic;
         reset : IN  std_logic;
         pulse_signal : IN  std_logic;
			pulse_counter_O : out std_logic_vector(31 downto 0);
         pulse_width : OUT  std_logic_vector(31 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk_50mhz : std_logic := '0';
   signal reset : std_logic := '0';
   signal pulse_signal : std_logic := '0';

 	--Outputs
   signal pulse_width : std_logic_vector(31 downto 0);
	signal pulse_counter_O : std_logic_vector(31 downto 0);
	signal Q1_out,Q2_out : std_logic:= '0';

   -- Clock period definitions
   constant clk_50mhz_period : time := 20 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: pulse_width_measure PORT MAP (
          clk_50mhz => clk_50mhz,
			 Q1_O => Q1_out,
			 Q2_O => Q2_out,
          reset => reset,
          pulse_signal => pulse_signal,
			 pulse_counter_O => pulse_counter_O,
          pulse_width => pulse_width
        );

   -- Clock process definitions
   clk_50mhz_process :process
   begin
		clk_50mhz <= '0';
		wait for clk_50mhz_period/2;
		clk_50mhz <= '1';
		wait for clk_50mhz_period/2;
   end process;
 

--   -- Stimulus process basic
--   stim_proc: process
--   begin		
--      -- hold reset state for 100 ns.
--		wait for 100 ns;	
--		reset <= '1';
--		wait for 20 ns;	
--		reset <= '0';
--      wait for 100.5 ns;	
--		pulse_signal <= '1';
--      wait for 40 ns;
--		pulse_signal <= '0';
--      wait for clk_50mhz_period*10;
--		pulse_signal <= '0';
--      wait for 100 ns;	
--		pulse_signal <= '1';
--		wait for 100 ns;	
--		pulse_signal <= '0';
--      -- insert stimulus here 
--
--      wait;
--   end process;

   -- Stimulus process ms
   stim_proc_ms: process
   begin		
		wait for 5 ns; --hold reset for 10ns
		reset <= '1';
		wait for 15 ns;		
		reset <= '0';
		
      wait for 100 ns;	
		pulse_signal <= '1'; --pulse high for 1000us
      wait for 1000 us;
		pulse_signal <= '0';
		
      wait for 3000 us;		--wait for 3ms
		
		pulse_signal <= '1'; --pulse high for 1400us
      wait for 1400 us;	
		pulse_signal <= '0';

      wait for 3000 us;		--wait for 3ms
		
		pulse_signal <= '1'; --pulse high for 2ms
      wait for 2000 us;	
		pulse_signal <= '0';

      -- insert stimulus here 

      wait;
   end process;

END;
