'''
Created on Aug 13, 2014

@author: Mike

@details    GUI interface allowing user to:
                 configure pulse width for each axis
                 jog the X, Y, Z axis or jog XY axis at the same time
                 send in a basic gcode file to begin routing
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


    def quit_program(self):
        self.cfg_file_handle.save_config_file()
        gtk.main_quit()
    
    def __init__(self):
        
        super(KshatriaGUI,self).__init__()
        self.gladefile = "Kshatria.glade"
        self.builder = gtk.Builder()
        self.builder.add_from_file(self.gladefile)
        self.builder.connect_signals(self)
        self.cfg_file_handle  = gui_support.CfgFile(self.builder)
         
        self.ComComboHandle = gui_support.ComCombo(self.builder,self.cfg_file_handle)

        self.GTKGCode_File = self.builder.get_object('GCode_File_Location')
        self.notebook = self.builder.get_object('notebook1')

        #pass CfData cfg_file_handle because we want those items to be saved to cfg file too
        self.CNCConfigData = gui_support.CfgData(self.builder,self.cfg_file_handle)
        
        self.CNCCommand = gui_support.CncCommand(self.builder,self.cfg_file_handle)
        self.SendSinleCFData = gui_support.SendSinleCFData(self.builder,self.cfg_file_handle)
        self.ManualControlData = gui_support.ManualControlData(self.builder,self.cfg_file_handle)
        
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
        self.feed_cut = self.builder.get_object('feed_cut')
        self.gcode_scale = self.builder.get_object('gcode_scale')
        self.layer_thickness    = self.builder.get_object('layer_thickness')
        self.layer_numbers      = self.builder.get_object('layer_numbers')
        self.speed_start    = self.builder.get_object('speed_start')
        self.speed_change      = self.builder.get_object('speed_change')                                                
                        
        self.cfg_file_handle.load_settings()
        
        self.set_gcode_file()
        self.window = self.builder.get_object("window1")
        #print 'class of window ',self.window.__class__
        self.window.show()
        #self._update_data()
        self.comthread = None

    def _update_data(self):
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


    ###################### Actions for all signals#########################
    def on_reverse_x_toggled(self,widget, data = None):
        self.axis_x.dir_reversed = widget.get_active()
        
    def on_reverse_y_toggled(self,widget, data = None):
        self.axis_y.dir_reversed = widget.get_active()

    def on_reverse_z_toggled(self,widget, data = None):
        self.axis_z.dir_reversed = widget.get_active()
        
    def on_servo_clicked(self, widget, data = None):
        
        self.SendSinleCFData.send(self.builder.get_object('servo_value'))
    def on_tst_gr1_button1_clicked(self,widget,data=None):
        pass
    def on_set_acceleration_clicked(self,widget):
        self.gs_speed_start = int(self.speed_start.get_text()) 
        self.gs_speed_change = int(self.speed_change.get_text()) 
        self.set_acceleration()        
    def on_jog_xy_clicked(self,widget):
        self._update_data()
        self.jog_xy()
        
    def on_jog_x_clicked(self,widget):
        self._update_data()
        self.axis_x.jog()
        
    def on_jog_y_clicked(self,widget):
        self._update_data()
        self.axis_y.jog()
        
    def on_jog_z_clicked(self,widget):
        self._update_data()
        self.axis_z.jog()

    def on_set_pw_z_clicked(self,widget):
        self._update_data()
        self.axis_z.set_pw_info()

    def on_set_pw_y_clicked(self,widget):
        self._update_data()
        self.axis_y.set_pw_info()

    def on_set_pw_x_clicked(self,widget):
        self._update_data()
        self.axis_x.set_pw_info()

    def on_start_routing_clicked(self, widget):
        self.gs_layer_thickness = int(self.layer_thickness.get_text()) 
        self.gs_layer_numbers = int(self.layer_numbers.get_text()  ) 
        self.set_layer()
        for i in range(3):
            print 'starting in ',3-i
            time.sleep(1)
        self.start_routing()
    
    def on_erase_coord_clicked(self,widget):
        self.erase_coordinates()
        
    def on_cancel_routing_clicked(self, widget):
        self.cancel_routing()

    def on_pause_routing_clicked(self, widget):
        self.pause_routing()
        #Communications.myQueue.put('hello')
            
    def on_Transfer_Coord_clicked(self, widget, data = None):
        print 'Transfer Coord button activated'
        self.gs_feed_cut = int(self.feed_cut.get_text())
        self.set_feed()
        print 'scale is ',int(self.gcode_scale.get_text())
        self.send_coordinates(int(self.gcode_scale.get_text()))
        #gui_support.send_file(self.GTKGCode_File.get_text())
        
    def on_rescan_coms_clicked(self,widget, data = None):
        self.ComComboHandle.rescan()
        
    def on_Com_channel_combo_box_changed(self,widget, data = None):
        #set combo box selection as active serial channel
        self.index = widget.get_active() #index indicate the nth item
        self.model = widget.get_model()
        self.item = self.model[self.index][1] #item is the text in combo box

        #set selection state 0 as a false state
        if not self.index == 0:
            Communications.consumer_portname = self.item
            Communications.serial_activated = True
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
    
    def set_gcode_file(self):
        self.gcode_file = self.GTKGCode_File.get_text()
        
    def on_Browse_For_GCode_pressed(self, widget, data = None):
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
if __name__ == "__main__":
    comthreadWriter = threading.Thread(target = Communications.set_writer)
    comthreadWriter.start()
    comthreadReader = threading.Thread(target = Communications.set_reader)
    comthreadReader.start()
#     keepsending = threading.Thread(target = Communications.set_keep_sending)
#     keepsending.start()
    
    main = KshatriaGUI()
    gtk.main()
    #main.comthread.join()
