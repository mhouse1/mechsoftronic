----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Michael House
-- 
-- Create Date:    14:04:39 12/22/2013 
-- Design Name: 
-- Module Name:    pulse_width_measure - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: D-Flip-flop modules
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: this takes 2 extra cycles for count to be measured.
--	meaning on the third cycle pulse width can be read
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity pulse_width_measure is
	port(
			clk_50mhz : in std_logic;
--			Q1_O, Q2_O : out std_logic;
--			reset : in std_logic;
			pulse_signal : in std_logic;
--			pulse_counter_O : out std_logic_vector(31 downto 0);
			pulse_width : out std_logic_vector(31 downto 0)
		
			);
			 
end pulse_width_measure;

architecture Behavioral of pulse_width_measure is

type state is 
(
	s_Reset, s_Start, s_Count, s_Stop
);

signal measurement_state : state;
signal pulse_counter : std_logic_vector(31 downto 0);
signal Q1 : std_logic;
signal Q2 : std_logic;

	component DFF
    Port ( D : in  STD_LOGIC;
           CLK : in  STD_LOGIC;
           Q : out  STD_LOGIC);
	end component;
	
begin
	inst_dff1 : DFF port map(D => pulse_signal, CLK => clk_50mhz, Q => Q1);
	inst_dff2 : DFF port map(D => Q1, CLK => clk_50mhz, Q => Q2);
process (clk_50mhz)
	begin
--	if reset = '1' then
--		pulse_counter <= x"00000000";
--		pulse_width <= x"00000000";
--		measurement_state <= s_Reset;
	if clk_50mhz'event and clk_50mhz = '1'then
		case measurement_state is
			when s_Reset =>
				if Q1 = '1' and Q2 = '0' then
					measurement_state <= s_Start;
				end if;
			when s_Start =>
				if Q1 = '1' and Q2 = '1' then
					measurement_state <= s_Count;
				end if;
				pulse_counter <= x"00000000";
			when s_Count =>
				if Q1 = '0' and Q2 = '1' then
					measurement_state <= s_Stop;
				end if;
				pulse_counter <= pulse_counter +1;
			when s_Stop =>
				if Q1 = '0' and Q2 = '0' then
					measurement_state <= s_Reset;
				end if;
				pulse_width <= pulse_counter +1;
			when others => 
				measurement_state <= s_Reset;
				pulse_counter <= x"00000000";
				pulse_width <= x"00000000";
		end case;
	end if;
--	Q1_O <= Q1;
--	Q2_O <= Q2;
--	pulse_counter_O <= pulse_counter;
end process;

end Behavioral;

