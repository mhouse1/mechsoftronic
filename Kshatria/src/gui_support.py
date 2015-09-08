'''
Created on Aug 24, 2014
@author: Dynames

@details  contains functions to support a GUI interface, by inheriting GuiSupport it is possible
          to access all the same functionality a GUI user would have by simply calling the functions.
          There is also a ConfigParser which saves and loads the user configurations in the GUI.
          All the communication is sent via a parallel process that is launched when the user
          configures the communication port.
'''
import gtk
import os
import ConfigParser
import sys

import protocolwrapper
import Communications
import parse_gcode
from parse_gcode import router_state
framer = protocolwrapper.ProtocolWrapper()


def get_bin_with_padding(number,padding):
    '''
    where number is a positive or negative integer
    and padding is number of fill bits
    '''
    if number < 0 :
        value = bin(int(number)& pow(2,padding)-1)[2:]
    else:
        value = ('0b'+("{:0%db}"%padding).format(number))[2:]
    return value
class Axi:
    '''
    creates a single axi object with its own data
    '''
    def __init__(self,sender=None,name = ''):
        '''
        constructor
        '''
        self.command_jog = 'JOG_'+name
        self.command_set_pw = 'SET_PW_'+name
        self.dir = 0
        self.dir_reversed = False
        self.step = 0
        self.pulse_width_high = 0
        self.pulse_width_low = 0
        self.send = sender
        #self.get_bin = lambda number, padding: number >= 0 and str(bin(number))[2:].zfill(padding) or "-" + str(bin(number))[3:].zfill(padding)
        self.get_bin = lambda number, padding: get_bin_with_padding(number,padding)
    def jog_payload(self):
        payload = self.get_bin(self.dir  +1 if self.dir_reversed else self.dir ,1)[-1] + \
                  self.get_bin(self.step,31)
        return payload
    
    def jog(self):
        self.send(self.command_jog,self.jog_payload())
    
    def set_pw_info(self):
        payload = self.get_bin(self.pulse_width_high,32) +\
                  self.get_bin(self.pulse_width_low,32)
        self.send(self.command_set_pw,payload)
    
class GuiSupport(object):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        self.command_num_bits = 8
        self.command_len_bits = 8
        self.bits_per_byte = 8
        
        #where command number is the number of command
        # and command_length is the number of bytes in payload
        self.command_list = {
        'JOG_Z'           :{'command_number' : 0 , 'command_length' : 4},
        'JOG_Y'           :{'command_number' : 1 , 'command_length' : 4},
        'JOG_X'           :{'command_number' : 2 , 'command_length' : 4},
        'JOG_XY'          :{'command_number' : 3 , 'command_length' : 8},
        'SET_PW_Z'        :{'command_number' : 4 , 'command_length' : 8},
        'SET_PW_Y'        :{'command_number' : 5 , 'command_length' : 8},
        'SET_PW_X'        :{'command_number' : 6 , 'command_length' : 8},
        'START_ROUTE'     :{'command_number' : 7 , 'command_length' : 0},
        'PAUSE'           :{'command_number' : 8 , 'command_length' : 0},
        'CANCEL'          :{'command_number' : 9 , 'command_length' : 0},
        'G_XY'            :{'command_number' : 10 , 'command_length' : 8},
        'FEED'            :{'command_number' : 11 , 'command_length' : 4},
        'ERASE_COORD'     :{'command_number' : 12 , 'command_length' : 0},  
        'SET_LAYER'       :{'command_number' : 13 , 'command_length' : 4},
        'SET_ACCEL'       :{'command_number' : 14 , 'command_length' : 8}, 
        'SET_ROUTE_STATE' :{'command_number' : 15 , 'command_length' : 1}, 
        'G_Z'             :{'command_number' : 16 , 'command_length' : 4}, 
        }
        #print 'GUI support initialized'
        #self.cfg_file_handle.load_settings()
        #host = "127.0.0.1"#"10.88.143.235" # set to IP address of target computer
        #port = 2055
        #addr = (host, port)
        #self._send_handle = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        #self._send_handle.connect(addr)
        self.gcode_file = ''
        #self.get_bin = lambda number, padding: number >= 0 and str(bin(number))[2:].zfill(padding) or "-" + str(bin(number))[3:].zfill(padding)
        self.get_bin = lambda number, padding: get_bin_with_padding(number,padding)

        self.axis_x = Axi(self._send,'X')
        self.axis_y = Axi(self._send,'Y')
        self.axis_z = Axi(self._send,'Z')
        
        self.gs_feed_cut =  30000
        self.gs_layer_thickness = 0
        self.gs_layer_numbers = 0
        self.gs_speed_start = 0
        self.gs_speed_change = 0
        
    def _send(self,command = 'ADEAD',payload = '', queue_priority = 0):
        '''
        payload should be a binary string in the format of the length of payload
        queue_priority == 0 is the fastest and the higher the number the slower 
        #note:set all gcode commands priority = 1, this will place gcode in slow queue
        #allowing user sent commands to be processed quickly even if routing is in progress
        '''
        payload_len = self.command_list[command]['command_length']
        received_payload_len = len(payload)/self.bits_per_byte
        cmd_num = self.get_bin(self.command_list[command]['command_number'] ,self.command_num_bits)
        
        cmd_len =  self.get_bin(payload_len,self.command_len_bits)
        
        #verify length of payload matches the specified length
        
        if not payload_len== received_payload_len:
            print 'specified payload {} does not match recevied payload size {}'.format(payload_len,received_payload_len)
        full_command_bin = cmd_num + cmd_len + payload #build full binary string
        hex_fill = '{0:0>'+str(len(full_command_bin)/4)+'}'
        #print '%x'% int(full_command_bin,2)
        full_command_hex = hex_fill.format('%x'% int(full_command_bin,2)) #build hex string zero fill
        
        #full_command_ascii = binascii.b2a_uu(full_command_bin)
        #self._send_handle.send(full_command_hex.decode('hex'))
        #print 'command ',command, cmd_num, cmd_len
        
        #print 'full_command binary:',command, cmd_num, cmd_len,payload
        #print 'full_command_decode',full_command_hex.decode('hex')
        #print 'full_command hex: ',full_command_hex
        #print 'full_command ascii:',full_command_ascii
#         producer1 = Communications.count_stuff(1,5,Communications.myQueue)
#         producer1.start()
        Communications.transmit(full_command_hex.decode('hex'), queue_priority)
        
    def quit(self):
        print 'closed handle'
        self._send_handle.close()
        sys.exit()

    def start_routing(self):
        ''' send command to start routing, no payload
        '''
        self._send('START_ROUTE')

    def pause_routing(self):
        ''' send command to pause routing, no payload
        '''
        self._send('PAUSE')

    def cancel_routing(self):
        ''' send command to cancel routing, no payload
        '''
        self._send('CANCEL')

    def erase_coordinates(self):
        '''tell firmware to erase the route
        '''
        Communications.slow_queue.queue.clear()
        self._send('ERASE_COORD')

    def set_layer(self):
        '''tell firmware how many layers to cut and how thick each layer is
        '''
        payload = self.get_bin(self.gs_layer_numbers,16) +\
                  self.get_bin(self.gs_layer_thickness,16)
        
        self._send('SET_LAYER',payload)        

    def set_acceleration(self):
        '''tell firmware starting speed to accelerate from and speed change rate
        '''
        payload = self.get_bin(self.gs_speed_start,32) +\
                  self.get_bin(self.gs_speed_change,32)
        
        self._send('SET_ACCEL',payload)        
            
    def set_feed(self):
        command = 'FEED'
        payload = self.get_bin(self.gs_feed_cut,32) 
        self._send(command,payload)
            
    def jog_xy(self):
        ''' payload contains direction and number of steps for x and y axis
        first 4 bytes
        bit 31 = direction x
        bit [30 to 0] = number of steps x
        next 4 bytes
        bit 31 = direction y
        bit [30 to 0] = number of steps y
        '''
        command= 'JOG_XY'
        
        self._send(command,self.axis_x.jog_payload()+self.axis_y.jog_payload())


    def send_coordinates(self,scale = 10000):
        '''
        parse gcode file to get coordinates then send the coordinates
        by putting them in the queue
        '''
        coordinates = parse_gcode.get_gcode_data(self.gcode_file, scale)
        total_num_coordinates = len(coordinates)
        
        #clear empty the gcode transmission queue incase its not empty
        Communications.slow_queue.queue.clear()
        for idx, coordinate_data in enumerate(coordinates):
            data1, data2 , cnc_state  = coordinate_data
            #print xpos, ypos
            if cnc_state == router_state.router_xy:
                command = 'G_XY'
                #print 'x ',int(data1), 'y', int(data2)
                payload = self.get_bin(int(data1),32) +\
                          self.get_bin(int(data2),32)
                          
                self._send(command, payload, 1)
            elif cnc_state == router_state.router_z:
                command = 'G_Z'
                payload = self.get_bin(data1,32)
                self._send(command, payload, 1)
            else:
                payload = self.get_bin(cnc_state,8)
                self._send('SET_ROUTE_STATE',payload, 1)
                #print 'tool status ', router_state.reverse_mapping[cnc_state]
            #print 'index ',idx
            
#             #print % progress of putting coordinates in queue
#             if str(idx)[-1] == '0':
#                 print "\r{0}".format((float(idx)/total_num_coordinates)*100),
        print 'finished putting coordinates in queue'

class CfgFile:
    ''' manages a GUI configuration file 
    has the ability to create, save, and load configuration file
    configuration data saved is typically data in text boxes, combo boxes, and other object states
    on GUI startup if a configuration file exists it is loaded 
    '''
    def __init__(self,builder):
        self.builder = builder
        self.config_object = ConfigParser.ConfigParser()
        self.config_file_name = 'config.ini'
        
        #a list of names of objects classfied as settings
        self.settings =['GCode_File_Location'
                        ,'reverse_x'
                        ,'reverse_y'
                        ,'reverse_z'
                        ,'feed_cut'
                        ,'speed_start'
                        ,'speed_change'
                        ,'gcode_scale'
                        ,'layer_thickness'
                        ,'layer_numbers'] 
        
    def create_config_file(self,config_object, config_file_name):
        f = open(config_file_name,'w')
        config_object.read(config_file_name)
        config_object.add_section('settings')
        config_object.write(f)
        f.close()
        print 'created Kshatria config file'
        
    def save_config_file(self):
        #self.config_object.set('settings', 'g-code file','none')
        for item in self.settings:
            obj = self.builder.get_object(item)
            #print obj.__class__.__name__
            if obj.__class__.__name__ == 'Entry':
                try:
                    self.config_object.set('settings', item,obj.get_text())
                except Exception, err:
                    print err
            elif obj.__class__.__name__ == 'CheckButton':
                try:
                    self.config_object.set('settings', item,obj.get_active())
                except Exception, err:
                    print err     
        self.config_object.write(open(self.config_file_name,'w'))
        print 'saved Kshatria config file'
        
    def load_settings(self):
        '''create and fill config file if it does not exist
        load_settings should be called only after self.settings has been appended to
        '''
        if not os.path.isfile(self.config_file_name) :
            
            self.create_config_file(self.config_object, self.config_file_name)
        else:#read config file
            self.config_object.read(self.config_file_name)
        #print self.settings
        #get object then set as value in config file
        for item in self.settings:
            obj = self.builder.get_object(item)
            
            #print item
            #if obj.__class__ == ''
            if obj.__class__.__name__ == 'Entry':
                if item == 'GCode_File_Location':
                    self.gcode_file = self.config_object.get('settings', item)
                    print 'set gcode file to ',self.gcode_file
                try:
                    obj.set_text(self.config_object.get('settings', item))
                except Exception, err:
                    print err
            elif obj.__class__.__name__ == 'CheckButton':
                try:
                    obj.set_active( 1 if 'True' == self.config_object.get('settings',item ) else 0)
                except Exception, err:
                    print err     
        #self.GTKGCode_File.set_text(self.config_object.get('settings', 'g-code file'))

        print 'loaded Kshatria config file'

class SendSinleCFData:
    def __init__(self,builder,cfg_file_handle):
        self.classification = 'cfdata'
        
    def send(self,widget):
        fields = [self.classification,'RouterPWM',widget.get_text()]#,item.get_text()]
        print fields
        Communications.transmit(framer.wrapfieldscrc(fields))
        
class CncCommand:
    def __init__(self,builder,cfg_file_handle):
        self.classification = 'command'
        name_of_command_data_objects = ['StepNumX',
                                        'StepNumY',
                                        'StepNumZ']#,
                                        #'DirXComboBox',
                                        #'DirYComboBox']
        
        cfg_file_handle.settings.extend(name_of_command_data_objects)
    def send(self,widget):
        #print widget
        self.PackedData = []
        fields = [self.classification,gtk.Buildable.get_name(widget)]#,item.get_text()]
        #print fields
        Communications.transmit(framer.wrapfieldscrc(fields))

class ManualControlData:
    def __init__(self, builder,cfg_handle):
        #cfg_file.__init__(self)
        self.CfObjects = []
        self.PackedData = []
        self.classification ='cfdata'
        
        name_of_command_data_objects = ['StepNumX',
                                        'StepNumY',
                                        'StepNumZ']#,
        
        cfg_handle.settings.extend(name_of_command_data_objects)

    def send(self,widget):
        self.PackedData = []
        #print widget
        #print 'widget class = ',widget.__class__.__name__
        if widget.__class__.__name__ == 'Entry':
            fields = [self.classification,gtk.Buildable.get_name(widget),widget.get_text()]
            #print fields #['cfdata', 'StepNumX', '2000']
            #raw_input('fields')
        elif widget.__class__.__name__ == 'ComboBox':
            index = widget.get_active() #indicate nth item selection
            model = widget.get_model()
            item = model[index][1]
            fields = [self.classification,gtk.Buildable.get_name(widget),item]
        #print fields
        if Communications.serial_activated:
            #fields = ['cfdata', 'StepNumX', '2000']
            #msg_encodded = pw.wrapfieldscrc(fields)
            #print 'encoded = ',msg_encodded
            ##encoded =  [#[F709D151#]#[###[cfdata###]###[StepNumX###]###[2000###]#]]
            value = framer.wrapfieldscrc(fields)
            #print 'class ManualControlData is sending: ',value
            Communications.transmit(value)
    
class CfgData:
    def __init__(self, builder,cfg_handle):
        #cfg_file.__init__(self)
        self.CfObjects = []
        self.PackedData = []
        self.classification ='cfdata'
        
        name_of_config_data_objects =[#'HMin','HMax',
                                      #'LMin','LMax',
                                      'pulsewidth_x_h','pulsewidth_x_l',
                                      'pulsewidth_y_h','pulsewidth_y_l',
                                      'pulsewidth_z_h','pulsewidth_z_l',
                                      #'SPer','SInc'
                                      ]
        
        cfg_handle.settings.extend(name_of_config_data_objects)

        #add all config objects to list
        for item in name_of_config_data_objects:
            self.addCfDataObject(builder.get_object(item))
        
#     def load_default(self, config_object, config_file_name):
#         if not os.path.isfile(config_file_name):
#             cfg_file.create_config_file()
#             
    def addCfDataObject(self,TxtObj):
        #objectDictionary = {gtk.Buildable.get_name(TxtObj):TxtObj}
        self.CfObjects.append(TxtObj)
    def All(self):
        return self.CfObjects

    def unpack(self):
        pw = protocolwrapper.ProtocolWrapper()
        print 'Erase FIFO button activated'
#         while Communications.active_serial.inWaiting():
#             #if there are BytesInBuffer then read one byte   
#             #read one byte 
#             byte = Communications.active_serial.read(1)
#             #use byte as input
#             if pw.input(byte) == protocolwrapper.ProtocolStatus.MSG_OK:
#                 #get fields if footer received
#                 fields_list = pw.get_fields()
#                 print fields_list
#      
    def get_packedData(self):
        #returns a list of framed data of the form
        #['[crc][classification][type][data]','[crc][classification][type][data]']
        # example data shown below:
        #['[\\[2261de11\\]\\[cfdata\\]\\[HMin\\]\\[high min width\\]]', 
        # '[\\[d79de256\\]\\[cfdata\\]\\[HMax\\]\\[asdfw\\]]', 
        # '[\\[fa91ef85\\]\\[cfdata\\]\\[LMin\\]\\[14989\\]]',  
        # '[\\[595a3145\\]\\[cfdata\\]\\[SInc\\]\\[dka38cni\\]]']

        
        self.PackedData = []
        for item in self.CfObjects:
            fields = [self.classification,gtk.Buildable.get_name(item),item.get_text()]
            #fields = ['hello','world','i','am','michael']
            self.PackedData.append(framer.wrapfieldscrc(fields))
        return self.PackedData
            
    def send(self):
        '''
        sends packed data over serial channel
        data will be in the format defined by protocol wrapper
        [Classification][Type][Data]
        [CRC32][Classification][Type][Data]
        
        call get_packedData to get the data to send
        '''
        #print self.get_packedData()[0]
        #if not Communications.active_serial == None:
#         print self.get_packedData()
#         raw_input('packed data')
#         for data in self.get_packedData():
#             C
        for data in self.get_packedData():
            Communications.transmit(data)

def send_file(filename):
    #create an object of protocol wrapper
    #if not Communications.active_serial == None:
    global framer
    PackedData = [] 
    file = open(filename,'r')
    for line in file:
        #print line
        stripped = line.rstrip('\r\n')#remove carriage returns
        if not (len(stripped)==0):#skip empty lines
            fields = ['gcode'] #initialieze fields list with classification
            fields.extend(stripped.split()) #tokenize on white spaces
            PackedData.append(framer.wrapfieldscrc(fields)) #wrap with crc
    for data in PackedData:
        Communications.transmit(data)#transmit
    
def get_com_port_list():
    
    #create an instance of Liststore with data
    liststore = gtk.ListStore(int,str)
    Com_List = Communications.list_serial_ports()
    liststore.append([0,"Select a valid serial port:"])
    for port_number in range(len(Com_List)):
        #print port_number
        liststore.append([port_number,Com_List[port_number]])
    return liststore
        
class ComCombo:
    '''deals with the combo box that allow selection of com port to use
    '''
    def __init__(self,builder):
        '''initialize the combo box
        '''
        self.name = 'Com_channel_combo_box'
        self.builder = builder
        #get an instance of the combo box
        self.Com_channel_combo = self.builder.get_object(self.name)
        #set the ListStore as the Model of the ComboBox
        
        
        #create an instance of the gtk CellRendererText object and pack into
        self.cell = gtk.CellRendererText()
        self.Com_channel_combo.pack_start(self.cell,True)
        self.Com_channel_combo.add_attribute(self.cell,'text',1)
        self.Com_channel_combo.set_active(0)
        self.Com_channel_combo.set_model(get_com_port_list())
        
    def rescan(self):
        #get a updated com port list and set as combo box options
        self.Com_channel_combo.set_model(get_com_port_list())
    

class GsComboBox:
    '''deals with the combo box that allow selection of com port to use
    '''
    def __init__(self,builder,obj_name, options):
        '''initialize the combo box
        '''
        self.name = obj_name
        self.builder = builder
        #get an instance of the combo box
        self.combo_obj = self.builder.get_object(self.name)
        #set the ListStore as the Model of the ComboBox
        
        
        #create an instance of the gtk CellRendererText object and pack into
        self.cell = gtk.CellRendererText()
        self.combo_obj.pack_start(self.cell,True)
        self.combo_obj.add_attribute(self.cell,'text',1)
        self.combo_obj.set_active(0)
        self.combo_obj.set_model(self._get_options(options))
        
    def _get_options(self,options):
        #create an instance of Liststore with data
        liststore = gtk.ListStore(int,str)
#         liststore.append([0,"set_direction"])
#         liststore.append([1,"up"])
#         liststore.append([2,"down"])
        for index, option in enumerate(options):
            liststore.append([index,option])
        #print 'list store',self.name,liststore
        return liststore
                    

    def get_selection(self):
        index = self.combo_obj.get_active()
        model = self.combo_obj.get_model()
        item = model[index][1]
        return item

    def get_selection_index(self):
        index = self.combo_obj.get_active()
        return index        
                    
class DirectionCombo:
    '''deals with the combo box that allow selection direction
    '''
    def __init__(self,builder,name):
        '''initialize the combo box
        '''
        self.name = name #'DirXComboBox'
        #cfg_handle.settings.extend(self.name)
        self.builder = builder
        #get an instance of the combo box
        self.Com_channel_combo = self.builder.get_object(self.name)
        #set the ListStore as the Model of the ComboBox
          
        
        #create an instance of the gtk CellRendererText object and pack into
        self.cell = gtk.CellRendererText()
        self.Com_channel_combo.pack_start(self.cell,True)
        self.Com_channel_combo.add_attribute(self.cell,'text',1)
        self.Com_channel_combo.set_active(0)
        self.Com_channel_combo.set_model(self.get_options())
    def get_options(self):
    
        #create an instance of Liststore with data
        liststore = gtk.ListStore(int,str)
        liststore.append([0,"set_direction"])
        liststore.append([1,"up"])
        liststore.append([2,"down"])
        return liststore
            
    def rescan(self):
        #get a updated com port list and set as combo box options
        self.Com_channel_combo.set_model(get_com_port_list())
            