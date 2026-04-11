--------------------------------------------------------------------------------
-- Entity: mux_4bit
-- Date:2014-01-19 
-- Author: Mike     
--
-- Description: a 4 bit mux
--------------------------------------------------------------------------------

LIBRARY IEEE; 
USE IEEE.STD_LOGIC_1164.ALL; 
use ieee.numeric_std.all;
USE WORK.mult_dim_ports.ALL; 
ENTITY mux_4bit_1 IS 
        GENERIC ( 
                bits : INTEGER := 8; 
                selections :   INTEGER := 4
        ); 
        PORT ( 
                a:      IN      std_logic_vector(bits - 1 downto 0);
					 b:      IN      std_logic_vector(bits - 1 downto 0);
					 c:      IN      std_logic_vector(bits - 1 downto 0);
					 d:      IN      std_logic_vector(bits - 1 downto 0);
					 e:      IN      std_logic_vector(bits - 1 downto 0);
					 f:      IN      std_logic_vector(bits - 1 downto 0);
					 g:      IN      std_logic_vector(bits - 1 downto 0);
                Output:      OUT     STD_LOGIC_VECTOR(bits - 1 DOWNTO 0); 
                sel :   IN      std_logic_vector( selections - 1 downto 0)
        ); 
END mux_4bit_1; 
ARCHITECTURE behaviour OF mux_4bit_1 IS 
BEGIN 
		with sel select
		Output <= a when "0000",
					 b when "0001",
					 c when "0010",
					 d when "0011",
					 e when "0100",
					 f when "0101",
					 g when "0110",
					std_logic_vector(to_unsigned(0,Output'length))	when others;
END behaviour; 