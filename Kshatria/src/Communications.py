'''
Created on Aug 9, 2014

@author: Mike

@brief    contains serial communication functionality
            Supported features: ability to list serial ports available, set active serial port,
                                buffer messages to be sent into a queue and send using a parallel process
'''
import os

import serial
from serial.tools import list_ports
import time
import multiprocessing
import threading
import Queue
#active_serial = None
serial_activated = False
consumer_portname = None
com_handle = None
fast_queue = Queue.Queue()
slow_queue = Queue.Queue()
stop_sending = False


def transmit(message, transmit_speed = 0):
    #where messages is a list of framed data
    #print 'transmitting...'
    #if not active_serial == None:
    
    #log transmitted message into a text file
    #the file can then be read by CUTE unit test to simulate
    #actual data sent over serial channel
    #//c++ usage example
    #CommSimple listener;
    #
    #std::fstream myfile("c:/bytestream0.txt", std::ios_base::in);
    #
    #int a;
    #while (myfile >> a)
    #{
    #    //printf("%d ", a);
    #    listener.input(a);
    #}
    default_file_name = 'bytestream'
    default_file_ext = '.txt'
    default_file_index = 0
    file_name = default_file_name+str(default_file_index)+default_file_ext
#     while os.path.isfile(file_name):
#         default_file_index += 1
#         file_name = default_file_name+str(default_file_index)+default_file_ext
    with open(file_name, "a") as myfile:
        for char in message:
            myfile.write(str(ord(char))+' ')
            #myfile.write(char)
        #myfile.write(message[:-1])
    
    if transmit_speed == 0:
        #print 'putFast',message
        fast_queue.put(message)
    else:
        #print 'putSlow',message
        slow_queue.put(message)


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

def set_writer(baud_rate = 19200, bytesize = 8, timeout = 1, ):
    '''
        this function will be called once when the GUI first initializes
        it then waits until the user sets an active serial chanel.
        Then goes into the loop that writes messages to queues that will
        be pushed to firmware via the active serial channel.
    '''
    global com_handle
    global stop_sending
    
    print 'waiting for serial selection'
    while consumer_portname is None:
        time.sleep(1)
        print '.',
    com_handle = serial.Serial(port = consumer_portname,baudrate = 19200)

    print 'serial activated'
    
    #if fast queue is not empty then send messages in the fast queue
    #until queue is empty. The slow queue will only send messages
    #if the fast queue is not empty and stop_sending is not active.
    
    while True:
        #print 'consumer active'
        while not fast_queue.empty():
            message_to_send = fast_queue.get()
            #print "OutF: {}".format(message_to_send)
            com_handle.write(message_to_send)
        time.sleep(0.5) 
        #print 'stopsend = {}'.format(stop_sending)
        while not slow_queue.empty() and fast_queue.empty() and not stop_sending:
            message_to_send = slow_queue.get()
            #print "OutS: {}".format(message_to_send)
            com_handle.write(message_to_send)
            #put a little delay so firwmare receiver buffer does not overflow before
            #it can set stop_sending, not the delay must be long enough so the buffer
            #does not overflow, and short enough so firmware does not run out of data to consume
            time.sleep(0.01)
            #set a delay for slow transfer queue
            #break out of delay early if detected fast_queue not empty
#             for i in xrange(2):
#                 if not fast_queue.empty():
#                     break
#                 time.sleep(0.1)#interval to check fast_queue is not empty
        #stop_sending = False
        

def set_reader():
    '''sets the active serial channel
    
        this function will be called once when the GUI first initializes
    '''
    print 'waiting for serial selection'
    global stop_sending
    while com_handle is None:
        time.sleep(1)
        #print '=',
    print 'starting reader'
    while True:
        #time.sleep(0.3)
        received = com_handle.read()
        if received == '1':
            #print 'stop it!'
            stop_sending = True
        elif received == '2':
            #print 'send it'
            stop_sending = False
        #print 'read {}'.format(received)

                        

    
if __name__ == "__main__":
    #print list_serial_ports()
    transmit('hello world')
