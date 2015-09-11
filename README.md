# mechsoftronic
This project contains the GUI, firmware, firmware component unit tests, FPGA top, FPGA components, and FPGA component unit tests for a FPGA based CNC machine controller. The goal is to create a machine with hardware and software support for multiple cutting tools (laser, plastic extruder, and drill) and other extensions not related to material shaping.<br>

<br>Four branches for this project: <br>
[**GUI_and_FW_Test**](https://github.com/mhouse1/mechsoftronic/tree/GUI_and_FW_Test) - contains the GUI developed in python to interface the Nios core firmware running on the FPGA. Also contains unit tests for firmware modules.

[**FPGA_Components**](https://github.com/mhouse1/mechsoftronic/tree/CNC_FPGA_Components) - custom FPGA components developed for the CNC machine which may be useful for other projects.

[**FPGA_Core**](https://github.com/mhouse1/mechsoftronic/tree/CNC_FPGA_Core/CNC_FPGA_Core) - The top project that links FPGA components to make one working product.

[**CNC_Firmware**](https://github.com/mhouse1/mechsoftronic/tree/CNC_Firmware/FPGA_PROJECT_BASE_SYSTEM_V3/software/RAPTOR_03) - c++ firmware running on RTOS MicroC OS II.  It interfaces with the FPGA to move the stepper motors and communicates with the GUI.

video of the initial prototyping hardware made from scrapped DVD drives and other bits and pieces can be found here,
[![mini cnc with laser mount](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/initial%20hardware.png)](https://www.youtube.com/watch?v=V51caXYTmaI)

<br>The small scale prototype has since been replaced with a much larger machine based on open hardware [**shapeoko v2**](https://github.com/shapeoko/Shapeoko_2). This hardware is used as base for development, the hardware will be heavily modified to supporting many more tools and functionalities and may look nothing like it is right now in the coming months.
[![CNC machine prototype hardware 2](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware_v2.png)]