'''
Created on Aug 13, 2014

@author: Mike

@details    GUI interface allowing user to:
                 configure pulse width for each axis
                 jog the X, Y, Z axis or jog XY axis at the same time
                 send in a basic gcode file to begin routing
'''
import gtk

import Communications
import gui_support
from gui_support import GuiSupport

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
        zdirection_combo_options = ['counter-clock','clockwise']
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
                        
        self.cfg_file_handle.load_settings()
        
        self.window = self.builder.get_object("window1")
        #print 'class of window ',self.window.__class__
        self.window.show()
        #self._update_data()
        

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
        self.start_routing()

    def on_cancel_routing_clicked(self, widget):
        self.cancel_routing()

    def on_pause_routing_clicked(self, widget):
        self.pause_routing()
            
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

        #set selection state 0 as a false state
        if not self.index == 0:
            Communications.Set_Active_Serial_Channel(port_name = self.item)
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
    main = KshatriaGUI()
    gtk.main()
