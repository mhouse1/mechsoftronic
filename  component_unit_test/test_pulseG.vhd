--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   22:21:50 03/26/2014
-- Design Name:   
-- Module Name:   C:/Users/Mike/Documents/FPGA_PROJECTS/Xilinx/PWM_Gen_V2/PWM_V2/test_pulseG.vhd
-- Project Name:  PWM_V2
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: pulse_generator
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
 
ENTITY test_pulseG IS
END test_pulseG;
 
ARCHITECTURE behavior OF test_pulseG IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT pulse_generator
    PORT(
         clk_50mhz : IN  std_logic;
         start_generation : IN  std_logic;
         pulse_signal : OUT  std_logic;
         generator_done : OUT  std_logic;
			speed_start, speed_change : in std_logic_vector(31 downto 0);
         pulse_width_high : IN  std_logic_vector(31 downto 0);
         pulse_width_low : IN  std_logic_vector(31 downto 0);
         number_of_steps : IN  std_logic_vector(31 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk_50mhz : std_logic := '0';
   signal start_generation : std_logic := '0';
   signal pulse_width_high : std_logic_vector(31 downto 0) := (others => '0');
   signal pulse_width_low : std_logic_vector(31 downto 0) := (others => '0');
   signal number_of_steps : std_logic_vector(31 downto 0) := (others => '0');
	signal speed_start, speed_change : std_logic_vector(31 downto 0) := (others => '0');

 	--Outputs
   signal pulse_signal : std_logic;
   signal generator_done : std_logic;

   -- Clock period definitions
   constant clk_50mhz_period : time := 20 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: pulse_generator PORT MAP (
          clk_50mhz => clk_50mhz,
          start_generation => start_generation,
          pulse_signal => pulse_signal,
          generator_done => generator_done,
          pulse_width_high => pulse_width_high,
			 speed_start => speed_start,
			 speed_change => speed_change,
          pulse_width_low => pulse_width_low,
          number_of_steps => number_of_steps
        );

   -- Clock process definitions
   clk_50mhz_process :process
   begin
		clk_50mhz <= '0';
		wait for clk_50mhz_period/2;
		clk_50mhz <= '1';
		wait for clk_50mhz_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.

		--test even number of steps target speed not reached
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"00000005";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"00000008";
		speed_start <= x"0000000F";
		speed_change <= x"00000002";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 2500 ns;
		
		--test even number of steps target speed reached
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"00000005";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"0000000F";
		speed_start <= x"0000000F";
		speed_change <= x"00000002";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 4000 ns;

		--test odd number of steps target speed reached
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"00000005";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"0000000E";
		speed_start <= x"0000000F";
		speed_change <= x"00000002";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 4000 ns;
		
		--test force target speed if next acceleration increment would 
		--make actual_speed faster than target speed. Test passes if
		--target speed reached for acceleration and successful deceleration from target speed
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"00000005";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"0000000F";
		speed_start <= x"0000000F";
		speed_change <= x"00000003";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 4000 ns;

		--test initial speed faster than target speed
		--if initial speed is faster than target speed then hold target speed
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"0000000F";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"0000000F";
		speed_start <= x"00000005";
		speed_change <= x"00000003";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 6000 ns;

		--test initial speed faster than target speed and even number of steps
		--if initial speed is faster than target speed then hold target speed
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"0000000F";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"00000010";
		speed_start <= x"00000005";
		speed_change <= x"00000003";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 6000 ns;

		--test initial speed and speed change are zero
      wait for clk_50mhz_period*5;
		pulse_width_high <= x"00000005";
		pulse_width_low <=  x"00000002";
		number_of_steps <=  x"00000020";
		speed_start <= x"00000000";
		speed_change <= x"00000000";
		
		
		wait for clk_50mhz_period;
		start_generation <= '1';
		wait for clk_50mhz_period;
		start_generation <= '0';
		
		wait for 5000 ns;																					
      wait;--wait required, otherwise simulation process will repeat
   end process;

END;
