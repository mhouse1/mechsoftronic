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
ENTITY mux_4bit IS 
        GENERIC ( 
                bits : INTEGER := 8; 
                selections :   INTEGER := 4
        ); 
        PORT ( 
                a:      IN      std_logic_vector(bits - 1 downto 0); --1
					 b:      IN      std_logic_vector(bits - 1 downto 0); --2
					 c:      IN      std_logic_vector(bits - 1 downto 0); --3
					 d:      IN      std_logic_vector(bits - 1 downto 0);--4
					 e:      IN      std_logic_vector(bits - 1 downto 0);--5
					 f:      IN      std_logic_vector(bits - 1 downto 0);--6
					 g:      IN      std_logic_vector(bits - 1 downto 0);--7
					 h:      IN      std_logic_vector(bits - 1 downto 0);--8
					 i:      IN      std_logic_vector(bits - 1 downto 0);--9
					 j:      IN      std_logic_vector(bits - 1 downto 0);--10
					 k:      IN      std_logic_vector(bits - 1 downto 0);--11
					 l:      IN      std_logic_vector(bits - 1 downto 0);--12
					 m:      IN      std_logic_vector(bits - 1 downto 0);--13
					 n:      IN      std_logic_vector(bits - 1 downto 0);--14
					 o:      IN      std_logic_vector(bits - 1 downto 0);--15
					 p:      IN      std_logic_vector(bits - 1 downto 0);--16
                Output:      OUT     STD_LOGIC_VECTOR(bits - 1 DOWNTO 0); 
                sel :   IN      std_logic_vector( selections - 1 downto 0)
        ); 
END mux_4bit; 
ARCHITECTURE behaviour OF mux_4bit IS 
BEGIN 
		with sel select
		Output <= a when "0000",
					 b when "0001",
					 c when "0010",
					 d when "0011",
					 e when "0100",
					 f when "0101",
					 g when "0110",
					 h when "0111",
					 i when "1000",
					 j when "1001",
					 k when "1010",
					 l when "1011",
					 m when "1100",
					 n when "1101",
					 o when "1110",
					 p when "1111",
					std_logic_vector(to_unsigned(0,Output'length))	when others;
END behaviour; 