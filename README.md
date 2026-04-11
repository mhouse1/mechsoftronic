# OpenSource CNC Controller
The goal of this project is to create an open source FPGA based CNC controller. 

For a more detailed overview see the document here:
https://github.com/mhouse1/mechsoftronic/blob/CNC_Machine_Complete/documentation/Overview_Project_Mechsoftronic.pdf

Otherwise keep reading for a brief overview.

**By using an FPGA it will allow:**
 * *creation of a dedicated device that would not require a separate PC to run.*
 * *any peripheral to be easily added in the future*
 * *smooth control by truly running machine axes in parallel*
  * *no more jitter caused by using parallel ports or microprocessors/microcontrollers*
  * *motion control is done by hardware instead of software*
 * *great hardware and software integration*
  * *by designing and implementing everything: UI, firmware, FPGA system.*

**Components of this project:**
  * [**CNC_Machine_Complete > CNC_FPGA_Core**](https://github.com/mhouse1/mechsoftronic/tree/CNC_Machine_Complete/CNC_FPGA_Core)
    * *includes the core FPGA project and custom FPGA modules created specifically for the CNC machine
    system. Implemented in [VHDL/Verilog]*
  * [**CNC_Machine_Complete > FPGA_PROJECT_BASE_SYSTEM_V3 > software > RAPTOR_03**](https://github.com/mhouse1/mechsoftronic/tree/CNC_Machine_Complete/FPGA_PROJECT_BASE_SYSTEM_V3/software/RAPTOR_03)
    * *Includes the MicroC OS II RTOS firmware that runs on the FPGA. Firmware communicates with the GUI, 
    interprets GCode, and interfaces the FPGA components to output motor motion control signals to move the 
    XYZ CNC machine axis. Implemented in [C++]*
  * [**CNC_Machine_Complete > Kshatria**](https://github.com/mhouse1/mechsoftronic/tree/CNC_Machine_Complete/Kshatria)
    * *The Graphical User Interface (GUI) that allows the user to communicate with the CNC controller; 
    allowing users to select GCode file to send to firmware, start, pause, and stop routing, and jog the axes. Implemented in [Python]*
  * [**CNC_Machine_Complete > test_raptor_firmware**](https://github.com/mhouse1/mechsoftronic/tree/CNC_Machine_Complete/test_raptor_firmware/test_firmware/src)
    * *Unit tests that test firmware components. Implemented in [C++]*


Video of the initial prototyping hardware made from scrapped DVD drives and other bits and pieces can be found here,
[![mini cnc with laser mount](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware.png)](https://www.youtube.com/watch?v=V51caXYTmaI)

<br>The small scale prototype's mechanics has since been replaced with a much larger mechanical platform based on a modified version of open hardware [**shapeoko v2**](https://github.com/shapeoko/Shapeoko_2).  The current mechanical system is only used as base for research and development, the intent of this CNC controller is to be adaptable to any hardware platform. For example, automating a [**manual lathe**](http://boltontool.com/combo-metal-lathe?search=BT500&gclid=Cj0KEQjwtaexBRCohZOAoOPL88oBEiQAr96eSE5P3HDibwYfC8uQpNoYUpIdWi2ZldI9dd1TcRwmJnsaAqJ98P8HAQ) that is already capable of machining metal parts and solidly built; by attaching stepper motors to its axes and controlling motion with this CNC controller.
[![CNC machine prototype hardware 2](https://raw.githubusercontent.com/mhouse1/mechsoftronic/GUI_and_FW_Test/Kshatria/R_and_D/prototyping_hardware_v2.png)

* *See videos of the project on youtube:*
  * *https://www.youtube.com/playlist?list=PLoPlnGMydurmCZgi33tBWLtC8rJmy5ksl* 
  * *or* 
  * *https://www.youtube.com/c/MHouseOne*


# required python packages
* to use the GUI the supporting packages required are contained in pygtk-all-in-one-2.22.6.win32-py2.7.msi file (~31.6MB)
* other python packges required
	* pip install pyserial
		* note do not install serial via "pip install serial" it will cause an error about exec() then you have to uninstall pyserial and serial then reinstall pyserial

	* pip install matplotlib
