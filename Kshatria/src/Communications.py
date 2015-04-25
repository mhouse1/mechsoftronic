'''
Created on Aug 9, 2014

@author: Mike
'''
import os

import serial
from serial.tools import list_ports
import time
import Queue
from threading import Thread
import multiprocessing
active_serial = None

message_queue = multiprocessing.Queue()

class SerialSendProcess(object):
    """ Threading example class

    The run() method will be started and it will run in the background
    until the application exits.
    
    04/19/2015 tried to create a thread to send serial data so the GUI
    does not slow down when writing over serial. it has become apparent
    that this will not work, multiprocessing would work better for this.
    """
 
    def __init__(self, interval=1):
        """ Constructor

        :type interval: int
        :param interval: Check interval, in seconds
        """
        self.interval = interval
 
        thread = multiprocessing.Process(name = 'serial sending thread', target =self.run,args=(message_queue,))
        thread.daemon = True                            # Daemonize thread
        thread.start()                                  # Start the execution
 
    def run(self,msg_queue):
        """ Method that runs forever """
#         while True:
#             # Do something
#             time.sleep(1)
#             size  = message_queue.qsize()
#             print 'size',size
#             if not message_queue.empty():
#                 msg= '\nitems in queue = ' +str(size)+ 'message dequeued:'+message_queue.get() 
#                 message_queue.task_done() 
#                 print msg
        while True:
            # Do something
            print 'waiting'
            time.sleep(1)
            while not msg_queue.empty():
                print 'doing something'
                msg = msg_queue.get()
                for ch in msg:
                    #print 'ch',ch
                    active_serial.write(ch)
                    time.sleep(0.01)


def transmit(messages):
    #where messages is a list of framed data
    print 'transmitting...'
    if not active_serial == None:
        for framed_data in messages:
            message_queue.put(framed_data)
            print 'queued msg:',framed_data
        #not sure why but i need to call empty()
        #or the parallel process will wait till the 3rd queued item before sending
        #the second item, all items afterwards seems to send okay
        message_queue.empty()
        
            #msg = '\nqueued ' + framed_data+  'queue size = '+str(message_queue.qsize())
            #print msg
#             #message_queue.join()
# #             print framed_data,
#             for ch in framed_data:
#                 active_serial.write(ch)
#                 time.sleep(0.01)



#             print framed_data
#             active_serial.write(framed_data)
#             time.sleep(0.5) 
    else:
#         triangle.nc GCode file
#         [#[8885F03D#]#[###[gcode###]###[G90###]#]]
#         [#[D3BFEF5A#]#[###[gcode###]###[G21###]#]]
#         [#[6BCC3A67#]#[###[gcode###]###[G0###]###[X6.4844###]###[Y8.4801###]#]]
#         [#[79EEEE0F#]#[###[gcode###]###[M03###]#]]
#         [#[5867A4B0#]#[###[gcode###]###[G1###]###[F30.000000###]#]]
#         [#[3073076F#]#[###[gcode###]###[G1###]###[X29.4366###]###[Y8.4801###]#]]
#         [#[857B4A14#]#[###[gcode###]###[G1###]###[X17.552###]###[Y30.7195###]#]]
#         [#[329FEDE8#]#[###[gcode###]###[G1###]###[X6.484###]###[Y8.48###]#]]
#         [#[97518E46#]#[###[gcode###]###[M05###]#]]
#         [#[96A149AD#]#[###[gcode###]###[G0###]###[X0.000###]###[Y0.000###]#]]
#         [#[B147E607#]#[###[gcode###]###[M05###]#]]
#         [#[D22EF06B#]#[###[gcode###]###[M02###]#]]        
#         for framed_data in messages:
#             print framed_data,
        print 'no serial to send:',messages
    #print '\n'
    
def list_serial_ports():
    # Windows
    if os.name == 'nt':
        # Scan for available ports.
        available = []
        for i in range(256):
            try:
                s = serial.Serial(i)
                available.append('COM'+str(i + 1))
                s.close()
            except serial.SerialException:
                pass
        return available
    else:
        # Mac / Linux
        return [port[0] for port in list_ports.comports()]
        #['/dev/cu.Michael-SerialServer-1', '/dev/cu.MikeHousesiPod-Wireless', '/dev/cu.Bluetooth-Modem', '/dev/cu.Bluetooth-Incoming-Port', '/dev/cu.MikeHousesiPhone-Wirele']

        #['/dev/cu.Michael-SerialServer-1', '/dev/cu.MikeHousesiPod-Wireless', '/dev/cu.Bluetooth-Modem', '/dev/cu.Bluetooth-Incoming-Port', '/dev/cu.MikeHousesiPhone-Wirele', '/dev/cu.SLAB_USBtoUART']

def Set_Active_Serial_Channel(port_name, baud_rate = 19200, bytesize = 8, timeout = 1, valid = True):
    '''sets the active serial channel
    
        this function will be called once when the GUI first initializes
    '''
    global active_serial
    if valid:
        active_serial = serial.Serial(port = port_name,baudrate = baud_rate)
        print active_serial
        #print 'starting thread'
        #launch a process that sends data if data queue is not empty
        transmitter = SerialSendProcess()

#     #example read
#     raw_data = active_serial.read()
#     data = None if len(raw_data) == 0 else ord(x)
#     #example write
#     active_serial.write('a')
    
if __name__ == "__main__":
    print list_serial_ports()
