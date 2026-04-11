'''
Created on Apr 20, 2015

@author: MHouse1
'''
import gui_support
import time
class HardwareTest:
    
    def __init__(self,builder):
        builder_object = builder
        cfg_file_handle  = gui_support.CfgFile(builder_object)

        self.CNCConfigData = gui_support.CfgData(builder_object, cfg_file_handle)
        self.ManualControlData = gui_support.ManualControlData(builder_object,cfg_file_handle)
        self.CNCCommand = gui_support.CncCommand(builder_object, cfg_file_handle)
        
        self.pulse_width_high = builder_object.get_object('HVal')
        self.pulse_width_low = builder_object.get_object('LVal')
        self.x_steps = builder_object.get_object('StepNumX')
        self.y_steps = builder_object.get_object('StepNumY')
        self.move_command = builder_object.get_object('MoveXY')
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