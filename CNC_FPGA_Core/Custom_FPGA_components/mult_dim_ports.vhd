--------------------------------------------------------------------------------
-- Entity: mult_dim_ports
-- Date:2014-01-19  ${time}
-- Author: Mike     
--
-- Description: 
--------------------------------------------------------------------------------
LIBRARY IEEE; 
USE IEEE.STD_LOGIC_1164.ALL; 
PACKAGE mult_dim_ports IS 
        TYPE std_logic_array IS ARRAY (NATURAL RANGE <>, NATURAL RANGE <> ) OF STD_LOGIC; 
        FUNCTION To_StdLogicVector  ( a :  std_logic_array; sel : NATURAL ) RETURN std_logic_vector; 
END; 
PACKAGE BODY mult_dim_ports IS 
        FUNCTION To_StdLogicVector  ( a :  std_logic_array; sel : NATURAL ) RETURN std_logic_vector IS 
                VARIABLE result : std_logic_vector ( a'RANGE(2) ); 
        BEGIN 
                FOR i IN a'RANGE(2) LOOP 
                        result(i) := a(sel, i); 
                END LOOP; 
                RETURN result; 
        END;     
END; 
