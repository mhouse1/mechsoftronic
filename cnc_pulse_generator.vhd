--------------------------------------------------------------------------------
-- Entity: cnc_pulse_generator
-- Date:2014-03-16
-- Author: Mike     
--
-- Description: generates step and direction signals for stepper drivers
--------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity cnc_pulse_generator is
	port  (
		clk : in std_logic;        -- input clock, xx MHz.
		pulse_width_high, pulse_width_low, num_of_steps : in std_logic_vector(15 downto 0);-- pulse info
		dir : in std_logic;
		generator_done, step_output, dir_output : out std_logic
	);
end cnc_pulse_generator;

architecture arch of cnc_pulse_generator is

begin



end arch;

