'''
Created on Aug 24, 2014
I love michelle
@author: Dynames
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
    
class hardware_test:
    
    def __init__(self,builder):
        builder_object = builder
        cfg_file_handle  = cfg_file(builder_object)

        self.CNCConfigData = CfData(builder_object, cfg_file_handle)
        self.ManualControlData = ManualControlData(builder_object,cfg_file_handle)
        self.CNCCommand = CncCommand(builder_object, cfg_file_handle)
        
        self.pulse_width_high = builder_object.get_object('HVal')
        self.pulse_width_low = builder_object.get_object('LVal')
        self.x_steps = builder_object.get_object('StepNumX')
        self.y_steps = builder_object.get_object('StepNumY')
        self.move_command = builder_object.get_object('Move')
        self.DirX = builder_object.get_object('DirX')
        self.DirY = builder_object.get_object('DirY')


        print 'hardware_test initialized'
    def test1(self):
        self.test_draw_square(test_name ='draw square')
#         self.test_decrease_pulse_high_width(test_name = 'hpw test1', start = 20000,stop=5000, step = -5000, low_pulse_width = 15000)
#         self.test_decrease_pulse_high_width(test_name = 'hpw test2', start = 200,stop=50, step = -50, low_pulse_width = 15000)
#         self.test_decrease_pulse_high_width(test_name = 'hpw test3', start = 200,stop=50, step = -50, low_pulse_width = 7000)


    def test_draw_square(self,test_name):   
        print " "      
        print "#####################################################################"
        print 'running ',test_name
        self.set_pulse_config(pulse_width_high = self.pulse_width_high.get_text(), pulse_width_low = self.pulse_width_low.get_text())
        self.set_steps(stepXnum = 2000, stepYnum = 1)
        self.set_xdir(dir = 'up')
        self.move()
        self.set_steps(stepXnum = 1, stepYnum = 2000)
        self.set_ydir(dir = 'up')
        self.move()
        self.set_steps(stepXnum = 2000, stepYnum = 1)
        self.set_xdir(dir = 'down')
        self.move()
        self.set_steps(stepXnum = 1, stepYnum = 2000)
        self.set_ydir(dir = 'down')
        self.move()        

    def test_decrease_pulse_high_width(self,test_name,start,stop,step,low_pulse_width):   
        print " "      
        print "#####################################################################"
        print 'running ',test_name
        pulse_width_low = low_pulse_width
        for pulse_high_width in range (start, stop, step):
            print "=================================================================="
            self.set_pulse_config(pulse_width_high = pulse_high_width, pulse_width_low = pulse_width_low)
            self.set_steps(stepXnum = 2000, stepYnum = 2000)
            direction = 'up'
            self.set_xdir(dir = direction)
            self.set_ydir(dir = direction)
            print 'moving ',direction,' with pulse_high_width = ',pulse_high_width, 'pulse_low_width = ',pulse_width_low

            self.move()
            time.sleep(2)
            direction = 'down'
            self.set_xdir(dir = direction)
            self.set_ydir(dir = direction)
            print 'moving ',direction,' with pulse_high_width = ',pulse_high_width, 'pulse_low_width = ',pulse_width_low            
            self.move()
            time.sleep(2)
        print 'completed ',test_name
        print "#####################################################################"
     
    def set_xdir(self,dir):
        if dir == 'up':
            self.DirX.set_active(1)
        elif dir == 'down': 
            self.DirX.set_active(2)
        else:
            print 'unsupported direction: ',dir
        #self.ManualControlData.send(self.DirX)
        
    def set_ydir(self,dir):
        if dir == 'up':
            self.DirY.set_active(1)
        elif dir == 'down': 
            self.DirY.set_active(2)
        else:
            print 'unsupported direction: ',dir
        #self.ManualControlData.send(self.DirY)
                
    def set_pulse_config(self, pulse_width_high = 123, pulse_width_low = 456):
        #set pulse data
        self.pulse_width_high.set_text(str(pulse_width_high))
        self.pulse_width_low.set_text(str(pulse_width_low))
        
        #send pulse data
        self.CNCConfigData.send()
                
    def set_steps(self,stepXnum = 0, stepYnum = 0):
        self.x_steps.set_text(str(stepXnum))
        self.y_steps.set_text(str(stepYnum))
        self.ManualControlData.send(self.x_steps)
        self.ManualControlData.send(self.y_steps)
    
    def move(self):
        self.CNCCommand.send(self.move_command)
class cfg_file:
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
        print 'created config file'
        
    def save_config_file(self):
        #self.config_object.set('settings', 'g-code file','none')
        for item in self.settings:
            obj = self.builder.get_object(item)
            print item
            if obj.__class__.__name__ == 'Entry':
                try:
                    self.config_object.set('settings', item,obj.get_text())
                except Exception, err:
                    print err
        self.config_object.write(open(self.config_file_name,'w'))
        
    def load_settings(self):
        #create instance of ConfigParser
        #create and fill config file if it does not exist
        #load_settings should be called only after self.settings has been appended to
        if not os.path.isfile(self.config_file_name) :
            
            self.create_config_file(self.config_object, self.config_file_name)
        else:#read config file
            self.config_object.read(self.config_file_name)
        print self.settings
        #get object then set as value in config file
        for item in self.settings:
            obj = self.builder.get_object(item)
            
            print item
            #if obj.__class__ == ''
            try:
                obj.set_text(self.config_object.get('settings', item))
            except Exception, err:
                print 'load settings exception',err
        #self.GTKGCode_File.set_text(self.config_object.get('settings', 'g-code file'))



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
                                        'StepNumY']#,
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
                                        'StepNumY']#,
        
        cfg_handle.settings.extend(name_of_command_data_objects)

    def send(self,widget):
        self.PackedData = []
        #print widget
        #print 'widget class = ',widget.__class__.__name__
        if widget.__class__.__name__ == 'Entry':
            fields = [self.classification,gtk.Buildable.get_name(widget),widget.get_text()]
        elif widget.__class__.__name__ == 'ComboBox':
            index = widget.get_active() #indicate nth item selection
            model = widget.get_model()
            item = model[index][1]
            fields = [self.classification,gtk.Buildable.get_name(widget),item]
        #print fields
        if not Communications.active_serial == None:
            Communications.transmit(framer.wrapfieldscrc(fields))
    
class CfData:
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
        while Communications.active_serial.inWaiting():
            #if there are BytesInBuffer then read one byte   
            #read one byte 
            byte = Communications.active_serial.read(1)
            #use byte as input
            if pw.input(byte) == protocolwrapper.ProtocolStatus.MSG_OK:
                #get fields if footer received
                fields_list = pw.get_fields()
                print fields_list
     
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
        Communications.transmit(self.get_packedData())

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
    Communications.transmit(PackedData)#transmit
    
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
    '''deals with the combo box that allow selection of com port to use
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
            