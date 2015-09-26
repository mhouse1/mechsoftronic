# OpenSource CNC Controller
The goal of this project is to create an open source FPGA based CNC controller. 

**By using an FPGA it will allow:**
 * *creation of a dedicated device that would not require a separate PC to run.*
 * *any peripheral to be easily added in the future*
 * *smooth control by truly running machine axes in parallel*
  * *no more jitter caused by using parallel ports or microprocessors/microcontrollers*
  * *motion control is done by hardware instead of software*
 * *great hardware and software integration*
  * *by designing and implementing everything: UI, firmware, FPGA system.*

**Components of this project:**
  * *CNC_Machine_Complete > CNC_FPGA_Core*
    * *[VHDL/Verilog] includes the core FPGA project and custom FPGA modules created specifically for the CNC machine 
    system*
  * *CNC_Machine_Complete > FPGA_PROJECT_BASE_SYSTEM_V3 > software > RAPTOR_03*
    * *[C++] includes the MicroC OS II RTOS firmware that runs on the FPGA. Firmware communicates with the GUI, 
    interprets GCode, and interfaces the FPGA components to output motor motion control signals to move the 
    XYZ CNC machine axis.*
  * *CNC_Machine_Complete > Kshatria*
    * *[Python] the Graphical User Interface (GUI) that allows the user to communicate with the CNC controller; 
    allowing users to select GCode file to send to firmware, start, pause, and stop routing, and jog the axes.*
  * *CNC_Machine_Complete > test_raptor_firmware*
    * *[C++] unit tests that test firmware components*


video of the initial prototyping hardware made from scrapped DVD drives and other bits and pieces can be found here,
[![mini cnc with laser mount](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware.png)](https://www.youtube.com/watch?v=V51caXYTmaI)

<br>The small scale prototype has since been replaced with a much larger machine based on a modified version of open hardware [**shapeoko v2**](https://github.com/shapeoko/Shapeoko_2). Some observed drawbacks to the shapeoko 2 are that: it is not very rigid which limits the range of materials you can shape, the motor mounting plates does not accomodate lager motors (NEMA 23's') very well, Z-axis routing depth is very small,  motor mounting plates should be extended to grip the belts better. I believe many of those drawbacks has been addressed in the Shapeoko 3 or X-Carve. The current mechanical system is used as base for research and development, the intent of this CNC controller project is to be adaptable to any hardware platform. For example, automating any cheap manual routers that is already capable of machining metal parts and solidly built; by attaching stepper motors to its axes and controlling motion with this CNC controller.
[![CNC machine prototype hardware 2](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware_v2.png)]

* *See videos of the project on youtube:*
  * *https://www.youtube.com/playlist?list=PLoPlnGMydurmCZgi33tBWLtC8rJmy5ksl* 
  * *or* 
  * *https://www.youtube.com/c/MHouseOne*

