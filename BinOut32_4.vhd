library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY BinOut32_4 IS
	PORT (hex : IN STD_LOGIC_VECTOR(31 downto 0);
			display_out : OUT STD_LOGIC_VECTOR(3 downto 0));
END BinOut32_4;

ARCHITECTURE Behavior OF BinOut32_4 IS
BEGIN

display_out<=hex(3 downto 0);

END Behavior;
