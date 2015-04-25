'''
Created on Aug 13, 2014

@author: Mike

04/20/2015 added support for z axis movement and testing buttons
'''
import gtk
# import os
# import ConfigParser
# import sys


import Communications
import gui_support
import test_kshatria

# path = os.path.dirname(gtk.__file__)
# print path
class Kshatria_GUI:


    def quit_program(self):
        self.cfg_file_handle.save_config_file()
        gtk.main_quit()
    
    def __init__(self):
        
        
        self.gladefile = "Kshatria.glade"
        self.builder = gtk.Builder()
        self.builder.add_from_file(self.gladefile)
        self.builder.connect_signals(self)
        self.cfg_file_handle  = gui_support.CfgFile(self.builder)
         
        self.ComComboHandle = gui_support.ComCombo(self.builder,self.cfg_file_handle)

        self.GTKGCode_File = self.builder.get_object('GCode_File_Location')
        

        #pass CfData cfg_file_handle because we want those items to be saved to cfg file too
        self.CNCConfigData = gui_support.CfgData(self.builder,self.cfg_file_handle)
        
        self.CNCCommand = gui_support.CncCommand(self.builder,self.cfg_file_handle)
        self.SendSinleCFData = gui_support.SendSinleCFData(self.builder,self.cfg_file_handle)
        self.ManualControlData = gui_support.ManualControlData(self.builder,self.cfg_file_handle)
        self.DirXComboHandle = gui_support.DirectionCombo(self.builder,'DirX')
        self.DirYComboHandle = gui_support.DirectionCombo(self.builder,'DirY')
        self.DirZComboHandle = gui_support.DirectionCombo(self.builder,'DirZ')

        self.window = self.builder.get_object("window1")
        #print 'class of window ',self.window.__class__
        self.window.show()
        
        self.cfg_file_handle.load_settings()
        
        self.hw_test = test_kshatria.HardwareTest(self.builder)
        
        
    ###################### Actions for all signals#########################
    
    def on_servo_clicked(self, widget, data = None):
        
        self.SendSinleCFData.send(self.builder.get_object('servo_value'))
    def on_tst_gr1_button1_clicked(self,widget,data=None):
        self.hw_test.test1()     
        
    def on_MoveXY_clicked(self, widget, data = None):
        self.ManualControlData.send(self.builder.get_object('StepNumX'))
        self.ManualControlData.send(self.builder.get_object('StepNumY'))
        self.CNCCommand.send(widget)
        print "move!"
    def on_MoveZ_clicked(self,widget, data=None):
        self.ManualControlData.send(self.builder.get_object('StepNumZ'))
        self.CNCCommand.send(widget)
        print "move Z!"
    def on_write_settings_clicked(self, widget, data = None):
        
        self.CNCCommand.send(widget)
        print "on write settings!"
    def on_Start_Routing_clicked(self, widget, data = None):
        print 'starting router'
        self.CNCCommand.send(widget)
    
    def on_StepNumX_activate(self,widget, data = None):
        pass
#         print 'enter key pressed from step num x text box'
#         self.ManualControlData.send(widget)
#         widget.get_toplevel().child_focus(gtk.DIR_TAB_FORWARD)
        
    
    def on_StepNumY_activate(self,widget,data=None):
        pass
#         self.ManualControlData.send(widget)
#         widget.get_toplevel().child_focus(gtk.DIR_TAB_FORWARD)
        
#     def on_StepNumX_focus_out_event(self, widget, data = None):
#         print "StepNumX submit"
#         self.ManualControlData.send(widget)
#         
#     def on_StepNumY_focus_out_event(self, widget, data = None):
#         print "StepNumY submit"
#         self.ManualControlData.send(widget)
#         
    def on_DirX_changed(self,widget, data=None):
        #set combo box selection as active serial channel
        self.ManualControlData.send(widget)
        
    def on_DirY_changed(self,widget, data=None):
        self.ManualControlData.send(widget)
    
    def on_DirZ_changed(self,widget, data=None):
        self.ManualControlData.send(widget)
        


    def on_StepNumZ_activate(self,widget, data=None):
        pass
        #self.ManualControlData.send(widget)
                
    def on_Transfer_Config_clicked(self, widget, data = None):
        self.CNCConfigData.send()
        #sent data is of the form:
#         [\[4d12e37f\]\[cfdata\]\[HMin\]\[fas\]]
#         [\[e4d37dfc\]\[cfdata\]\[HMax\]\[\]]
#         [\[c79a985\]\[cfdata\]\[LMin\]\[\]]
#         [\[69be47af\]\[cfdata\]\[LMax\]\[\]]
#         [\[6bc04b8b\]\[cfdata\]\[SPer\]\[\]]
#         [\[b8da039c\]\[cfdata\]\[HVal\]\[qwr\]]
#         [\[2f66a067\]\[cfdata\]\[LVal\]\[hello\]]
#         [\[8fa222b3\]\[cfdata\]\[SInc\]\[555\]]
        #if lets say LMin checksum does not match a request can be sent 
        #to only resend that data and not the whole message
        
    def on_Erase_FIFO_clicked(self, widget, data = None):
        pass

        
        #self.CNCConfigData.unpack()
#         print 'Erase FIFO button activated'
#         BytesInBuffer = Communications.active_serial.inWaiting()
#         print BytesInBuffer,' Bytes in buffer'
#         data = Communications.active_serial.read(BytesInBuffer)
#         print data
        
    def on_Transfer_Coord_clicked(self, widget, data = None):
        print 'Transfer Coord button activated'
        
        gui_support.send_file(self.GTKGCode_File.get_text())
        
    def on_rescan_coms_clicked(self,widget, data = None):
        self.ComComboHandle.rescan()
        
    def on_Com_channel_combo_box_changed(self,widget, data = None):
        #set combo box selection as active serial channel
        self.index = widget.get_active() #index indicate the nth item
        self.model = widget.get_model()
        self.item = self.model[self.index][1] #item is the text in combo box

        #set selection state
        selection_state = False if self.index == 0 else True
        
        Communications.Set_Active_Serial_Channel(port_name = self.item, valid = selection_state)
        #self.builder.get_object("label1").set_text(self.item)
    
    def notebook1_switch_page_cb(self,  notebook, page, page_num, data=None):
        self.tab = notebook.get_nth_page(page_num)
        self.switched_page = notebook.get_tab_label(self.tab).get_label()
        print 'switched to page ',self.switched_page
    
    def on_Quit_activate(self,widget, data = None):
        print 'quitting...'
        self.quit_program()
    
    def on_window1_destroy(self, widget, data = None):
        print 'quitting...'
        self.quit_program()
    
    def on_Browse_For_GCode_pressed(self, widget, data = None):
        print 'Browsing for GCode file'
        self.fcd = gtk.FileChooserDialog("Open...",None,gtk.FILE_CHOOSER_ACTION_OPEN,
                 (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                  gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        self.response = self.fcd.run()
        if self.response == gtk.RESPONSE_OK:
            self.GCodeFile = self.fcd.get_filename()
            print "Selected filepath: %s" % self.GCodeFile
            self.fcd.destroy()
            self.GTKGCode_File.set_text(self.GCodeFile)
    
    ###################### End of actions for all signals#################
if __name__ == "__main__":
    main = Kshatria_GUI()
    gtk.main()
