'''
Created on Aug 13, 2014

@author: Mike

@details    GUI interface allowing user to:
                configure pulse width for each axis
                jog the X, Y, Z axis or jog XY axis at the same time
                transmit parsed gcode to firmware for routing, start
                pause and clear route planned.
'''
import gtk
import time

import Communications
import gui_support
from gui_support import GuiSupport
import threading
import multiprocessing
import gobject

#Allow only the main thread to touch the GUI (gtk) part, while letting other threads do background work.
gobject.threads_init()

class KshatriaGUI(GuiSupport):    
    def __init__(self):
        '''
        inherits GUI support object and links it to GUI signals
        to provide an action to a user response.
        '''
        super(KshatriaGUI,self).__init__()
        
        #read *.glade xml file that has the gui layout info
        #build the gui interface using xml file
        #connect gui signals to call back functions
        self.gladefile = "Kshatria.glade"
        self.builder = gtk.Builder()
        self.builder.add_from_file(self.gladefile)
        self.builder.connect_signals(self)
        
        #use builder object to get a handle to configuration file
        self.cfg_file_handle  = gui_support.CfgFile(self.builder)
        
        #use builder to get a handle to the COM port drop down object
        self.ComComboHandle = gui_support.ComCombo(self.builder)

        #use builder to get gcode file path text box object
        self.GTKGCode_File = self.builder.get_object('GCode_File_Location')
        
        #use builder to get GUI tab/pages object
        self.notebook = self.builder.get_object('notebook1')

        #use builder and config file handle to get config data
        #pass CfData cfg_file_handle because we want those items to be saved to cfg file too
        self.CNCConfigData = gui_support.CfgData(self.builder,self.cfg_file_handle)
        
        self.CNCCommand = gui_support.CncCommand(self.builder,self.cfg_file_handle)
        self.SendSinleCFData = gui_support.SendSinleCFData(self.builder,self.cfg_file_handle)
        self.ManualControlData = gui_support.ManualControlData(self.builder,self.cfg_file_handle)
        
        #drop down GUI objects used to set routing direction
        xdirection_combo_options = ['left','right']
        ydirection_combo_options = ['backward','forward']
        zdirection_combo_options = ['counter-clockwise','clockwise']
        self.DirXComboHandle = gui_support.GsComboBox(self.builder,'DirX',xdirection_combo_options)
        self.DirYComboHandle = gui_support.GsComboBox(self.builder,'DirY',ydirection_combo_options)
        self.DirZComboHandle = gui_support.GsComboBox(self.builder,'DirZ',zdirection_combo_options)
        
        #text box objects
        self.StepNumZ       = self.builder.get_object('StepNumZ')
        self.StepNumY       = self.builder.get_object('StepNumY')
        self.StepNumX       = self.builder.get_object('StepNumX')
        self.pulsewidth_z_h = self.builder.get_object('pulsewidth_z_h')
        self.pulsewidth_z_l = self.builder.get_object('pulsewidth_z_l')
        self.pulsewidth_y_h = self.builder.get_object('pulsewidth_y_h')
        self.pulsewidth_y_l = self.builder.get_object('pulsewidth_y_l')
        self.pulsewidth_x_h = self.builder.get_object('pulsewidth_x_h')
        self.pulsewidth_x_l = self.builder.get_object('pulsewidth_x_l')
        self.feed_cut       = self.builder.get_object('feed_cut')
        self.gcode_scale    = self.builder.get_object('gcode_scale')
        self.layer_thickness= self.builder.get_object('layer_thickness')
        self.layer_numbers  = self.builder.get_object('layer_numbers')
        self.speed_start    = self.builder.get_object('speed_start')
        self.speed_change   = self.builder.get_object('speed_change')                                                
        
        #load GUI default values from a settings file                
        self.cfg_file_handle.load_settings()
        
        #set current gcode file to whatever file path is 
        #displayed in the GCode file path text box
        self.set_gcode_file()
        
        #show the GUI window
        self.window = self.builder.get_object("window1")
        self.window.show()
        self.comthread = None


    ###################### Actions for all signals#########################
    def on_reverse_x_toggled(self,widget, data = None):
        '''
        if reversed check box is toggled,
        use software to reverse axis direction of movement
        note: this just reverses the direction being sent
              it does not toggle the firmware reverse bit.
        '''
        self.axis_x.dir_reversed = widget.get_active()
        
    def on_reverse_y_toggled(self,widget, data = None):
        '''
        if reversed check box is toggled,
        use software to reverse axis direction of movement
        note: this just reverses the direction being sent
              it does not toggle the firmware reverse bit.
        '''
        self.axis_y.dir_reversed = widget.get_active()

    def on_reverse_z_toggled(self,widget, data = None):
        '''
        if reversed check box is toggled,
        use software to reverse axis direction of movement
        note: this just reverses the direction being sent
              it does not toggle the firmware reverse bit.
        '''
        self.axis_z.dir_reversed = widget.get_active()
        
    def on_servo_clicked(self, widget, data = None):
        '''
        to be used in the future to control router RPM
        '''
        self.SendSinleCFData.send(self.builder.get_object('servo_value'))
        
    def on_tst_gr1_button1_clicked(self,widget,data=None):
        '''
        dummy button: use this to run whatever code you wish
        '''
        #print how many items are left in slow_queue
        print Communications.slow_queue.qsize(),' in slow queue'
    
    def on_set_acceleration_clicked(self,widget):
        '''
        sets the acceleration profile for all axis
        '''
        self.gs_speed_start = int(self.speed_start.get_text()) 
        self.gs_speed_change = int(self.speed_change.get_text()) 
        self.set_acceleration()    
            
    def on_jog_xy_clicked(self,widget):
        '''
        sends command to jog x and y axis using data in GUI for
        number of setps and pulse width info
        '''
        self._update_data()
        self.jog_xy()
        
    def on_jog_x_clicked(self,widget):
        '''
        sends command to jog x axis using data in GUI for
        number of setps and pulse width info
        '''
        self._update_data()
        self.axis_x.jog()
        
    def on_jog_y_clicked(self,widget):
        '''
        sends command to jog y axis using data in GUI for
        number of setps and pulse width info
        '''
        self._update_data()
        self.axis_y.jog()
        
    def on_jog_z_clicked(self,widget):
        '''
        sends command to jog z axis using data in GUI for
        number of setps and pulse width info
        '''
        self._update_data()
        self.axis_z.jog()

    def on_set_pw_z_clicked(self,widget):
        '''
        set z axis pulse width
        '''
        self._update_data()
        self.axis_z.set_pw_info()

    def on_set_pw_y_clicked(self,widget):
        '''
        set y axis pulse width
        '''
        self._update_data()
        self.axis_y.set_pw_info()

    def on_set_pw_x_clicked(self,widget):
        '''
        set x axis pulse width
        '''
        self._update_data()
        self.axis_x.set_pw_info()

    def on_start_routing_clicked(self, widget):
        '''
        will clear the cancel and pause routing bits in firmware
        
        if routing is paused it will resume routing
        this function is called when the transfer gcode button is pressed
        '''
        self.start_routing()
        pass
#         self.gs_layer_thickness = int(self.layer_thickness.get_text()) 
#         self.gs_layer_numbers = int(self.layer_numbers.get_text()  ) 
#         self.set_layer()
#         for i in range(3):
#             print 'starting in ',3-i
#             time.sleep(1)
#         self.start_routing()
    
    def on_erase_coord_clicked(self,widget):
        '''
        tell firmware to erase all coordinates in queue
        '''
        
        self.erase_coordinates()
        
    def on_cancel_routing_clicked(self, widget):
        '''
        tell firmware to stop routing and erase whatever
        route is planned in the queue
        '''
        self.cancel_routing()

    def on_pause_routing_clicked(self, widget):
        '''
        tell firmware to pause routing
            to resume routing send resume command
        '''
        self.pause_routing()
            
    def on_Transfer_Coord_clicked(self, widget, data = None):
        '''
        parse gcode and transmit routing instructions to firmware.
        
        sequence:
        a start_routing command is sent to reset routing pause
        and routing cancel incase they are set. 
        
        a set_feed command is sent to set the feedrate for routing
        this determines how fast the router will move.
        
        parse gcode into coordinates and push data into slow transmission
        queue so firmware can still accept commands sent to fast
        transmission queue.
        
        '''
        
        #do a wait count down before routing
        for i in xrange(3):
            print 'starting in:',3-i
            time.sleep(1)
            
        print 'Transfer Coord button activated'
        self.start_routing()
        
        self.gs_feed_cut = int(self.feed_cut.get_text())
        self.set_feed()
        print 'scale is ',int(self.gcode_scale.get_text())
        self.send_coordinates(int(self.gcode_scale.get_text()))
        #gui_support.send_file(self.GTKGCode_File.get_text())
        
    def on_rescan_coms_clicked(self,widget, data = None):
        '''
        rescan for available serial ports and update drop down box
        to display serial ports available
        '''
        self.ComComboHandle.rescan()
        
    def on_Com_channel_combo_box_changed(self,widget, data = None):
        '''
        set combo box selection as active serial channel
        '''
        self.index = widget.get_active() #index indicate the nth item
        self.model = widget.get_model()
        self.item = self.model[self.index][1] #item is the text in combo box

        #set selection state 0 as a false state
        if not self.index == 0:
            Communications.consumer_portname = self.item
            Communications.serial_activated = True
        #self.builder.get_object("label1").set_text(self.item)
    
    def notebook1_switch_page_cb(self,  notebook, page, page_num, data=None):
        '''
        do something when the gui changes from one tab to another
        this currently does nothing
        '''
        self.tab = notebook.get_nth_page(page_num)
        self.switched_page = notebook.get_tab_label(self.tab).get_label()
        print 'switched to page ',self.switched_page
    
    def on_Quit_activate(self,widget, data = None):
        '''
        exit gui and save config file
        '''
        print 'quitting...'
        self._quit_program()
    
    def on_window1_destroy(self, widget, data = None):
        '''
        exit gui and save config file
        '''
        print 'quitting...'
        self._quit_program()
    
    def set_gcode_file(self):
        '''
        set current gcode file to whatever file path is 
        displayed in the GCode file path text box
        '''
        
        self.gcode_file = self.GTKGCode_File.get_text()
        
    def on_Browse_For_GCode_pressed(self, widget, data = None):
        '''
        on button press open file chooser window to allow user to select 
        gcode file to use for routing
        '''
        print 'Browsing for GCode file'
        self.fcd = gtk.FileChooserDialog("Open...",None,gtk.FILE_CHOOSER_ACTION_OPEN,
                 (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                  gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        self.response = self.fcd.run()
        if self.response == gtk.RESPONSE_OK:
            self.gcode_file = self.fcd.get_filename()
            print "Selected filepath: %s" % self.gcode_file
            self.fcd.destroy()
            self.GTKGCode_File.set_text(self.gcode_file)
    
    ###################### End of actions for all signals#################
    def _quit_program(self):
        '''
        save a configuration file before terminating GUI thread
        '''
        self.cfg_file_handle.save_config_file()
        gtk.main_quit()
        
    def _update_data(self):
        '''
        get values from GUI objects and apply to gui_support variables
        '''
        self.axis_z.dir = self.DirZComboHandle.get_selection_index()
        self.axis_y.dir = self.DirYComboHandle.get_selection_index()
        self.axis_x.dir = self.DirXComboHandle.get_selection_index()
        self.axis_z.step = int(self.StepNumZ.get_text())      
        self.axis_y.step = int(self.StepNumY.get_text())      
        self.axis_x.step = int(self.StepNumX.get_text())      
        self.axis_z.pulse_width_high = int(self.pulsewidth_z_h.get_text())
        self.axis_z.pulse_width_low = int(self.pulsewidth_z_l.get_text())
        self.axis_y.pulse_width_high = int(self.pulsewidth_y_h.get_text())
        self.axis_y.pulse_width_low = int(self.pulsewidth_y_l.get_text())
        self.axis_x.pulse_width_high = int(self.pulsewidth_x_h.get_text())
        self.axis_x.pulse_width_low = int(self.pulsewidth_x_l.get_text())

if __name__ == "__main__":
    #start communication for reading and writing
    comthreadWriter = threading.Thread(target = Communications.set_writer)
    comthreadWriter.daemon = True #terminate thread when program ends
    comthreadWriter.start()
    comthreadReader = threading.Thread(target = Communications.set_reader)
    comthreadReader.daemon = True #terminate when program ends
    comthreadReader.start()

    
    #GUI thread    
    main = KshatriaGUI()
    gtk.main()
