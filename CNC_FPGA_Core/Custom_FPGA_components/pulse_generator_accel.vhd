--05/302015 pulse generator module with acceleration and deceleration profile
--intended to be only used with a single axis. this module is not suitable for multiple axis
--if synchronized movement is required.
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity pulse_generator_accel is
	port(
			clk_50mhz : in std_logic;
			start_generation : in std_logic;
			reset_n	: in std_logic;
			pulse_signal, generator_done : out std_logic;
			speed_start, speed_change : in std_logic_vector(31 downto 0);
			pulse_width_high, pulse_width_low, number_of_steps : in std_logic_vector(31 downto 0)
			);
end pulse_generator_accel;

architecture Behavioral of pulse_generator_accel is

type generator_state is
(
	gen_waiting, gen_high, gen_low
);

type speed_state is
(
	speed_initialize, speed_accelerate, speed_hold, speed_decelerate
);

signal sig_generator_state : generator_state;
signal sig_speed_state : speed_state;
signal period_counter : std_logic_vector(31 downto 0);
signal step_counter : std_logic_vector(31 downto 0);
--signal  speed_change : std_logic_vector(31 downto 0);
signal actual_speed, accelerated_count, steps_allow_to_accelerate, leftover_speed_change : std_logic_vector(31 downto 0);
signal done_accelerating : boolean;

begin

process (clk_50mhz,reset_n)
	begin

	if clk_50mhz'event and clk_50mhz = '1'then
		if reset_n = '0' then
				sig_generator_state <= gen_waiting;
				sig_speed_state <= speed_initialize;
				generator_done <= '1';
				pulse_signal <= '0';
				step_counter <= x"00000000";
		end if;

		case sig_generator_state is
			when gen_waiting =>
				pulse_signal <= '0';
				generator_done <= '1';
				step_counter <= x"00000000";
				if start_generation = '1' then
					--if no step to take then go to gen_waiting state
					-- and assert done
					if x"00000000" = number_of_steps then
						sig_generator_state <= gen_waiting;
						generator_done <= '1';
					else
						--set starting speed to 1000000
						--actual_speed <= speed_start;--x"0000000F";--x"000F4240";
						--speed_change <= speed_change;--x"00000003";--x"000007D0";
						sig_speed_state <= speed_initialize;
						accelerated_count <= x"00000000";
						--steps_allow_to_accelerate = number_of_steps divided by 2
						steps_allow_to_accelerate <= '0' & number_of_steps(31 downto 1);
						done_accelerating <= false;
						generator_done <= '0';
						period_counter <= x"00000000";
						sig_generator_state <= gen_high;
						
					end if;
				end if;
			when gen_high =>
				pulse_signal <= '1';
				period_counter <= period_counter +1;
				case sig_speed_state is
					when speed_initialize =>
						if pulse_width_high >= speed_start then
							actual_speed <= pulse_width_high;
						elsif (speed_start = 0) or (speed_change = 0) then
							actual_speed <= pulse_width_high;
							sig_speed_state <= speed_hold;
						else
							actual_speed <= speed_start;
						end if;
						sig_speed_state <= speed_accelerate;
					when speed_accelerate =>
						
						if (period_counter+1) = actual_speed then
							accelerated_count <= accelerated_count + 1;
							
							if ((actual_speed - speed_change) >= pulse_width_high) and not((accelerated_count + 1) = steps_allow_to_accelerate)  then
								--accelerate only if peak speed not reached
								actual_speed <= actual_speed - speed_change;
							elsif ((actual_speed - speed_change) < pulse_width_high) and not((accelerated_count + 1) = steps_allow_to_accelerate)  then
								--if the next acceleration would bring actual speed lower than pulse_width_high, use pulse_width_high as actual_speed
								accelerated_count <= accelerated_count + 1;

								leftover_speed_change <= actual_speed - pulse_width_high;
								actual_speed <= pulse_width_high;

								sig_speed_state <= speed_hold;
							elsif (actual_speed = pulse_width_high) then-- acceleration reached peak
								--there are two options
								--option 1: go to hold speed state
								accelerated_count <= accelerated_count + 1;
								sig_speed_state <= speed_hold;
								--option 2: go to deceleration speed state
							end if;
								
							period_counter <= x"00000000";
							sig_generator_state <= gen_low;							
						end if;
						if accelerated_count = steps_allow_to_accelerate then
							sig_speed_state <= speed_hold;
						end if;
						
					when speed_hold =>
						if (period_counter+1) = actual_speed then
							period_counter <= x"00000000";
							sig_generator_state <= gen_low;							
						end if;
						--if remaining steps left is less then equal
						--to counts accelerated then decelerate 1 count and go to decelerate state
						--else count periods until periods reached then go to gen_low state
						if  (number_of_steps - step_counter) <= (accelerated_count) then
							sig_speed_state <= speed_decelerate;
							--if for odd number of counts dont change actual_speed before going to decelerate speed_state
							if (number_of_steps - step_counter + 1) <= (accelerated_count) or (number_of_steps - step_counter) = (accelerated_count)then
								
							else
								actual_speed <= actual_speed + speed_change;		
							end if;
																
						end if;
					when speed_decelerate =>
						if leftover_speed_change > 0 then
							actual_speed <= actual_speed + leftover_speed_change;-- +speed_change;
							leftover_speed_change <= x"00000000";
						end if;
						if (period_counter+1) = actual_speed then
							actual_speed <= actual_speed + speed_change;
							period_counter <= x"00000000";
							sig_generator_state <= gen_low;							
						end if;
				end case;
	
--				if period_counter = (actual_speed -1) then
--					period_counter <= x"00000000";
--					sig_generator_state <= gen_low;
--					--if true then accelerate
--					if ((actual_speed - speed_change)  >= pulse_width_high) and (not done_accelerating) and (steps_allow_to_accelerate > step_counter) then
--						--(steps_allow_to_accelerate+1) = 6 then--
--						if (steps_allow_to_accelerate+1) = (number_of_steps - step_counter) then  -- step_counter = 4 then--step_counter =(steps_allow_to_accelerate-3) then--
--							done_accelerating <= true;
--						else
--							--increase speed by shortening high pulse width
--							actual_speed <= actual_speed - speed_change;
--							--steps_taken_to_reach_nominal_speed <= steps_taken_to_reach_nominal_speed + 1;
--						end if;
--						steps_taken_to_reach_nominal_speed <= steps_taken_to_reach_nominal_speed + 1;
--					--else if true then decelerate
--					elsif (number_of_steps - step_counter) <= (steps_taken_to_reach_nominal_speed +1) then
--						if leftover_speed_change > 0 then
--							actual_speed <= actual_speed + leftover_speed_change;-- +speed_change;
--							steps_taken_to_reach_nominal_speed<= steps_taken_to_reach_nominal_speed+1;
--							leftover_speed_change <= x"00000000";
--						else
--							actual_speed <= actual_speed + speed_change;
--						end if;
--					--keep speed same unless acceleration is not done
--					elsif (not done_accelerating) then
--						done_accelerating <= true;
--						--
--						if not ((number_of_steps - step_counter+1)=step_counter) then
--							leftover_speed_change <= actual_speed - pulse_width_high;
--							actual_speed <= pulse_width_high;
--						end if;
--						if leftover_speed_change >= 0 then
--							steps_taken_to_reach_nominal_speed <= steps_taken_to_reach_nominal_speed + 3;
--						end if;
--					end if;
--				else 
--					period_counter <= period_counter +1;
--				end if;
				
			when gen_low =>
				pulse_signal <= '0';				
				if (period_counter+1) = pulse_width_low then
					period_counter <= x"00000000";
					step_counter <= step_counter + 1;
					if (step_counter +1)   = number_of_steps then
						sig_generator_state <= gen_waiting;
					else
						sig_generator_state <= gen_high;
					end if;
					
				else
					period_counter <= period_counter +1;
				end if;
				
		end case;
	end if;
end process;	

end Behavioral;