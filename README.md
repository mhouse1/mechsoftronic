# mechsoftronic
This project contains GUI, firmware, firmware unit test and FPGA parts for a FPGA based CNC machine

# branches
Four branches for this project: 

**GUI_and_FW_Test** - contains the GUI developed in python to interface the Nios core firmware running on the FPGA. Also contains unit tests for firmware modules.

**FPGA_Components** - custom written FPGA components used for the CNC machine which may be useful for other projects

**FPGA_Core** - The top project that links FPGA components to make one working product

**CNC_Firmware** - c++ firmware running on RTOS MicroC OS II.  It interfaces with the FPGA to move the stepper motors and communicates with the GUI.
