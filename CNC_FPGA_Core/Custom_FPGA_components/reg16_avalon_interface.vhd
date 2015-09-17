LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY reg16_avalon_interface IS
	PORT ( 			 clock, resetn : IN STD_LOGIC;
			read, write, chipselect : IN STD_LOGIC;
							  writedata : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
							 byteenable : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
								readdata : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
								Q_export : OUT STD_LOGIC_VECTOR(15 DOWNTO 0) );
END reg16_avalon_interface;

ARCHITECTURE Structure OF reg16_avalon_interface IS
	SIGNAL local_byteenable : STD_LOGIC_VECTOR(1 DOWNTO 0);
	SIGNAL to_reg, from_reg : STD_LOGIC_VECTOR(15 DOWNTO 0);

	COMPONENT reg16
		PORT ( clock, resetn : IN STD_LOGIC;
								 D : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
					 byteenable : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
								 Q : OUT STD_LOGIC_VECTOR(15 DOWNTO 0) );
END COMPONENT;
BEGIN
	to_reg <= writedata;
	WITH (chipselect AND write) SELECT
		local_byteenable <= byteenable WHEN '1', "00" WHEN OTHERS;
	reg_instance: reg16 PORT MAP (clock, resetn, to_reg, local_byteenable, from_reg);
	readdata <= from_reg;
	Q_export <= from_reg;
END Structure;

--verilog equivalent
--module reg16_avalon_interface (clock, resetn, writedata, readdata, write, read,
--	byteenable, chipselect, Q_export);
--	
--	// signals for connecting to the Avalon fabric
--	input clock, resetn, read, write, chipselect;
--	input [1:0] byteenable;
--	input [15:0] writedata;
--	output [15:0] readdata;
--	
--	// signal for exporting register contents outside of the embedded system
--	output [15:0] Q_export;
--	
--	wire [1:0] local_byteenable;
--	wire [15:0] to_reg, from_reg;
--	
--	assign to_reg = writedata;
--	
--	assign local_byteenable = (chipselect & write) ? byteenable : 2’d0;
--	
--	reg16 U1 ( .clock(clock), .resetn(resetn), .D(to_reg), .byteenable(local_byteenable),
--	.Q(from_reg) );
--	
--	assign readdata = from_reg;
--	assign Q_export = from_reg;
--endmodule