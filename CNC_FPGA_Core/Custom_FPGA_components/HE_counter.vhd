----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Michael House
-- 
-- Create Date:    14:04:39 12/22/2013 
-- Design Name: 
-- Module Name:    HE_counter - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: D-Flip-flop modules
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: a state machine that counts number of pulses
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity HE_counter is
	port(
			CLOCK_IN : in std_logic;
			reset_n	: in std_logic;
			pulse_signal : in std_logic;
			pulse_counts : out std_logic_vector(31 downto 0)
			);
			 
end HE_counter;

architecture Behavioral of HE_counter is

type state is 
(
	s_Rising, s_High, s_Falling, s_Low
);

signal measurement_state : state;
signal sig_pulse_counter : std_logic_vector(31 downto 0);
signal Q1 : std_logic;
signal Q2 : std_logic;

	component DFF
    Port ( D : in  STD_LOGIC;
           CLK : in  STD_LOGIC;
           Q : out  STD_LOGIC);
	end component;
	
begin
	inst_dff1 : DFF port map(D => pulse_signal, CLK => CLOCK_IN, Q => Q1);
	inst_dff2 : DFF port map(D => Q1, CLK => CLOCK_IN, Q => Q2);
process (CLOCK_IN,reset_n)
	begin
	if CLOCK_IN'event and CLOCK_IN = '1'then
		if reset_n = '0' then
				measurement_state <= s_Rising;
				sig_pulse_counter <= x"00000000";
				pulse_counts <= x"00000000";
		end if;
		case measurement_state is
			when s_Rising =>
				if Q1 = '1' and Q2 = '0' then --rising edge of pulse
					measurement_state <= s_High;
				end if;
			when s_High =>
				if Q1 = '1' and Q2 = '1' then -- pulse is high
					measurement_state <= s_Falling;
					sig_pulse_counter <= sig_pulse_counter +1;
					pulse_counts <= sig_pulse_counter;
				end if;
			when s_Falling =>
				if Q1 = '0' and Q2 = '1' then -- falling edge of pulse
					measurement_state <= s_Low;
				end if;
			when s_Low =>
				if Q1 = '0' and Q2 = '0' then -- pulse is low
					measurement_state <= s_Rising;
				end if;

		end case;
	end if;

end process;

end Behavioral;

