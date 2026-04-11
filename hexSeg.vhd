library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY hexSeg IS
	PORT (hex : IN STD_LOGIC_VECTOR(15 downto 0);
			display_out : OUT STD_LOGIC_VECTOR(3 downto 0));
END hexSeg;

ARCHITECTURE Behavior OF hexSeg IS
BEGIN

display_out<=hex(3 downto 0);
--	PROCESS(hex)
--	BEGIN
--		CASE hex IS
--			WHEN x"0000" => display_out <=x"0";
--			WHEN x"0003" => display_out <=x"3";
--			WHEN x"0004" => display_out <=x"4";
--			WHEN x"0005" => display_out <=x"5";
--			WHEN OTHERS => display_out <=x"5";
--		END CASE;
--	END PROCESS;
END Behavior;