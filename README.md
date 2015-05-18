# mechsoftronic
This project contains GUI, firmware, firmware unit test and FPGA parts for a FPGA based CNC machine<br>
Four branches for this project: <br>

[**GUI_and_FW_Test**](https://github.com/mhouse1/mechsoftronic/tree/GUI_and_FW_Test) - contains the GUI developed in python to interface the Nios core firmware running on the FPGA. Also contains unit tests for firmware modules.

[**FPGA_Components**](https://github.com/mhouse1/mechsoftronic/tree/CNC_FPGA_Components) - custom written FPGA components used for the CNC machine which may be useful for other projects

[**FPGA_Core**](https://github.com/mhouse1/mechsoftronic/tree/CNC_FPGA_Core/CNC_FPGA_Core) - The top project that links FPGA components to make one working product

[**CNC_Firmware**](https://github.com/mhouse1/mechsoftronic/tree/CNC_Firmware/FPGA_PROJECT_BASE_SYSTEM_V3/software/RAPTOR_03) - c++ firmware running on RTOS MicroC OS II.  It interfaces with the FPGA to move the stepper motors and communicates with the GUI.
