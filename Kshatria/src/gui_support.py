'''
Created on Aug 24, 2014
@author: Dynames

@brief    contains functions to support a GUI interface
04/20/2015 moved class HardwareTest out of this script and into test_kshatria.py
           renamed classes in gui_support to use CamelCase naming convention
'''
import gtk
import os
import ConfigParser
import protocolwrapper
import Communications
import time
import sys
framer = protocolwrapper.ProtocolWrapper()

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
        #
        'jog_z'  :{'command_number' : 0 , 'command_length' : 4}, #bit 0 = direction, bits 1-31 = number of steps
        'jog_y'  :{'command_number' : 1 , 'command_length' : 4}, 
        'jog_x'  :{'command_number' : 2 , 'command_length' : 4}, 
        'jog_xy' :{'command_number' : 3 , 'command_length' : 8}, #first 4 bytes = x_dir(1),x_step(31. next 4 bytes = y_dir(1), y_step(31) 
        'pw_z'   :{'command_number' : 4 , 'command_length' : 8}, #first 8 bytes pulse width high count, next 4 bytes pulse width low count
        'pw_y'   :{'command_number' : 5 , 'command_length' : 8},
        'pw_x'   :{'command_number' : 6 , 'command_length' : 8},

        'G1_XY'  :{'command_number' : 7 , 'command_length' : 8}, #GCode ommands 
        }
        print 'Gui support initialized'
        #self.cfg_file_handle.load_settings()
        host = "127.0.0.1"#"10.88.143.235" # set to IP address of target computer
        port = 2055
        addr = (host, port)
        #self._send_handle = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        #self._send_handle.connect(addr)
        
    
        self.get_bin = lambda x, n: x >= 0 and str(bin(x))[2:].zfill(n) or "-" + str(bin(x))[3:].zfill(n)
        self.gs_step_z = 0
        self.gs_step_y = 0
        self.gs_step_x = 0
        self.gs_dir_z = 0
        self.gs_dir_y = 0
        self.gs_dir_x = 0
        self.gs_pw_z_h = 0
        self.gs_pw_z_l = 0
        self.gs_pw_y_h = 0
        self.gs_pw_y_l = 0        
        self.gs_pw_x_h = 0
        self.gs_pw_x_l = 0
                

        
    def _send(self,command = 'ADEAD',payload = ''):
        '''
        payload should be a binary string in the format of the length of payload
        '''
        cmd_num = self.get_bin(self.command_list[command]['command_number'] ,self.command_num_bits)
        cmd_len =  self.get_bin(self.command_list[command]['command_length'],self.command_len_bits)
        full_command_bin = cmd_num + cmd_len + payload #build full binary string
        full_command_hex = format(int(full_command_bin,2),'#04x')[2:] #build full hex string
        #print 'full_command_decode',full_command_hex.decode('hex')
        #full_command_ascii = binascii.b2a_uu(full_command_bin)
        #self._send_handle.send(full_command_hex.decode('hex'))
        print 'command ',command
        Communications.transmit(full_command_hex.decode('hex'))
        print 'full_command binary:',full_command_bin
        #print 'full_command hex:',full_command_hex
        #print 'full_command ascii:',full_command_ascii
        
    def quit(self):
        print 'closed handle'
        self._send_handle.close()
        sys.exit()

    def move_z(self):
        self._send(command= 'jog_z')    
#     def GlobalDisable(self):
#         print 'send disable command'
#         self._send(command = 'GlobalDisable')
# 
#     def GlobalEnable(self):
#         print 'send enable command'
#         self._send(command = 'GlobalEnable')
# 
# 
#     def SetFrequency(self):
#         command = 'SetFrequency'
#         payload = self.get_bin(self.frequency,self.command_list[command]['command_length'] *self.bits_per_byte)
#         self._send(command, payload)  
#         
#     def ClusterControl(self):
#         print 'send enable command'
#         spare_bit = 0
#         payload =   self.get_bin(self.cl_address    , 2) + \
#                     self.get_bin(spare_bit          ,1) + \
#                     self.get_bin(self.cl_output_status,1)  + \
#                     self.get_bin(self.cl_amplitude_a, 11)  + \
#                     self.get_bin(self.cl_phase_a    , 9)   + \
#                     self.get_bin(self.cl_amplitude_b, 11)  + \
#                     self.get_bin(self.cl_phase_b    , 9)   + \
#                     self.get_bin(self.cl_amplitude_c, 11)  + \
#                     self.get_bin(self.cl_phase_c    , 9)
#         print 'clustercontrol payload:',payload
#         if len(payload) == 64: 
#             self._send(command = 'ClusterControl', payload = payload)
#         else:
#             print 'clustercontrol payload length:',len(payload)
#             print 'did not generate correct payload, check range of cluster data values'  
        
class Enum(set):
    def __getattr__(self, name):
        if name in self:
            return name
        raise AttributeError
    

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
        self.settings =['GCode_File_Location'] #a list of names of objects classfied as settings
        
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
            #print item
            if obj.__class__.__name__ == 'Entry':
                try:
                    self.config_object.set('settings', item,obj.get_text())
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
            try:
                obj.set_text(self.config_object.get('settings', item))
            except Exception, err:
                print 'load settings exception',err
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
    def __init__(self,builder,cfg_handle):
        '''initialize the combo box
        '''
        self.name = 'Com_channel_combo_box'
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
        self.Com_channel_combo.set_model(get_com_port_list())
        
    def rescan(self):
        #get a updated com port list and set as combo box options
        self.Com_channel_combo.set_model(get_com_port_list())
        
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
            