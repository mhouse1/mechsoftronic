# mechsoftronic
The goal of this project is to create an OpenSource FPGA based CNC controller. 

* *Components of this project:*
* *CNC_Machine_Complete > CNC_FPGA_Core*
* *[VHDL/Verilog] includes the core FPGA project and custom FPGA modules created specifically for the CNC machine 
system*
* *CNC_Machine_Complete > FPGA_PROJECT_BASE_SYSTEM_V3 > software > RAPTOR_03*
* *[C++] includes the MicroC OS II RTOS firmware that runs on the FPGA. Firmware communicates with the GUI, 
interprets GCode, and interfaces the FPGA components to output motor motion control signals to move the 
XYZ CNC machine axis.*
* *CNC_Machine_Complete > Kshatria*
* *[Python] the Graphical User Interface (GUI) that allows the user to communicate with the CNC controller; 
allowing users to select GCode file to send to firmware, start, pause, and stop routing, jog the axis.*
* *CNC_Machine_Complete > test_raptor_firmware*
* *[C++] unit tests that test firmware components*


video of the initial prototyping hardware made from scrapped DVD drives and other bits and pieces can be found here,
[![mini cnc with laser mount](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware.png)](https://www.youtube.com/watch?v=V51caXYTmaI)

<br>The small scale prototype has since been replaced with a much larger machine based on open hardware [**shapeoko v2**](https://github.com/shapeoko/Shapeoko_2). This hardware is used as base for development, the hardware will be heavily modified to supporting many more tools and functionalities and may look nothing like it is right now in the coming months.
[![CNC machine prototype hardware 2](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware_v2.png)]

See video of cuts on youtube: https://www.youtube.com/watch?v=i7nqlkxr84E
