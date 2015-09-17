--pulse generator module that generates a specified number of pulses with constant pulse widths
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity pulse_generator is
	port(
			clk_50mhz : in std_logic;
			start_generation : in std_logic;
			pulse_signal, generator_done : out std_logic;
			pulse_width_high, pulse_width_low, number_of_steps : in std_logic_vector(31 downto 0)
			);
end pulse_generator;

architecture Behavioral of pulse_generator is

type generator_state is
(
	gen_waiting, gen_high, gen_low
);

signal sig_generator_state : generator_state;
signal period_counter : std_logic_vector(31 downto 0);
signal step_counter : std_logic_vector(31 downto 0);

begin

process (clk_50mhz)
	begin

	if clk_50mhz'event and clk_50mhz = '1'then
		case sig_generator_state is
			when gen_waiting =>
				pulse_signal <= '0';
				generator_done <= '1';
				step_counter <= x"00000000";
				if start_generation = '1' then
					generator_done <= '0';
					--pulse_signal <= '1';
					period_counter <= x"00000000";
					if x"00000000" = number_of_steps then
						sig_generator_state <= gen_waiting;
						generator_done <= '0';
					else
						sig_generator_state <= gen_high;
					end if;
				end if;
			when gen_high =>
				pulse_signal <= '1';
				period_counter <= period_counter +1;
				if (period_counter + 1) = pulse_width_high then
					--pulse_signal <= '0';
					period_counter <= x"00000000";
					sig_generator_state <= gen_low;
				end if;
			when gen_low =>
				pulse_signal <= '0';
				period_counter <= period_counter +1;
				
				if (period_counter + 1) = pulse_width_low then
					--pulse_signal <= '1';
					period_counter <= x"00000000";
					step_counter <= step_counter + 1;
					if (step_counter  + 1) = number_of_steps then
						--generator_done <= '1';
						sig_generator_state <= gen_waiting;
					else
						sig_generator_state <= gen_high;
					end if;
				end if;
			when others =>
				sig_generator_state <= gen_waiting;
		end case;
	end if;
end process;	

end Behavioral;

