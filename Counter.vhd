--*****************************************************************************
--*  DEFINE: Library                                                          *
--*****************************************************************************
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


--*****************************************************************************
--*  DEFINE: Entity                                                           *
--*****************************************************************************

entity counter is
   port(  
         clk         : in  std_logic;
         counter_out : out std_logic
       );
end entity counter;


--*****************************************************************************
--*  DEFINE: Architecture                                                     *
--*****************************************************************************

architecture syn of counter is

   --
   -- Define all components which are included here
   --

   --
   -- Define all local signals (like static data) here
   --
   
   signal counter_data : std_logic_vector(31 downto 0) := (others => '0');  
  
begin

   process(clk)
   begin
    
      if rising_edge(clk) then
         counter_data <= std_logic_vector(unsigned(counter_data) + 1);
      end if; 
    
   end process;
  
   counter_out <= counter_data(21); -- counter_data(n) where n is the bit number of the counter,
												-- increasing n makes counter_out change slower, vice versa

end architecture syn;

-- *** EOF ***
