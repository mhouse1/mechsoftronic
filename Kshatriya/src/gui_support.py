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
framer = protocolwrapper.ProtocolWrapper()

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
                                      'LVal','HVal',
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
            