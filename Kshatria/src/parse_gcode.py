'''
Created on May 1, 2015

@author: MHouse1
'''
import matplotlib.pyplot as plt 
import math
from numpy import double
from decimal import Decimal #import will fail if theres a number.py in src directory
from matplotlib.cbook import Null

def enum(*sequential, **named):
    '''
    this function is taken from stackoverflow
    see: http://stackoverflow.com/questions/36932/how-can-i-represent-an-enum-in-python
    usage:
            >>> Numbers = enum('router_off', 'router_on', 'router_up','router_down','router_xy')
            >>> Numbers.router_up
            2
            >>> Numbers.router_down
            3
            
            #support for converting the values back to names
            >>> Numbers.reverse_mapping[1]
            'router_on'            
    '''
    enums = dict(zip(sequential, range(len(sequential))), **named)
    reverse = dict((value, key) for key, value in enums.iteritems())
    enums['reverse_mapping'] = reverse
    return type('Enum', (), enums)

router_state = enum('router_off', 'router_on', 'router_up','router_down','router_xy','router_z')

def get_gcode_data(input_file = 'bridesmaid_inner_01.nc',scale=10000):
    '''
    Reads and parses gcode returns a coordinate list for gui_support
    to send to firmware
    '''
    #read all lines in gcode file
    with open(input_file) as f:
        gcode_file = f.read().splitlines()
    
    #do post processing on lines read from gcode file
    #this is done to get the gocde file into a 
    #recognizable format for the rest of the parsing code
    #what is done:
    #    replace three spaces with no space ex: '   ' = ''
    #    replace two spaces with no space ex: '  ' = ''
    #gcode generation programs supported
    #    inkscape - laser plugin
    #    makercam - aka partcam.swf
    #    DXF2GCODE
    post_processed = []
    for sentence in gcode_file:
        sentence = sentence.replace('   ','')
        sentence = sentence.replace('  ','')
        post_processed.append(sentence)
    gcode_file = post_processed
        
    #{router_off = 0, router_on, router_up, router_down,router_xy};
    router_off  = 0
    router_on   = 1
    router_up   = 2
    router_down = 3
    router_xy   = 4
    router_z    = 5

    
    coordinates = []
    for line in gcode_file:
        #remove line break and split on the spaces
        if len(line) == 0:
            pass
        else:
            tokens =  line.split(' ')#line[:-1].split(' ')
                        
            gcode_type = tokens[0]
            #if GCODE found in list
            if gcode_type in ['G0','G1','G2','G3']:
                #print tokens
                #if next two parameter begins with X and Y
                if tokens[1][0] =='X' and tokens[2][0] == 'Y':
                    #print tokens
                    #print tokens[1][1:],tokens[2][1:]
                    #convert (x,y) string to float then scale it then convert to int
                    coordinates.append((int(Decimal(tokens[1][1:])*Decimal(scale)),int(Decimal(tokens[2][1:])*Decimal(scale)),int(router_state.router_xy)))
                    print coordinates[-1] #print last item in list
                elif tokens[1][0] =='Z':
                    print 'gcode z motion', double(tokens[1][1:])
                    if gcode_type =='G0' or gcode_type =='G1':
                        coordinates.append((int(Decimal(tokens[1][1:])*Decimal(scale)),1,int(router_state.router_z)))
                    else:
                        raise ValueError('unexpected gcode type with command z')
                    
                else:
                    print 'ignored:*',tokens
            elif gcode_type == 'M3' or gcode_type == 'M03':
                coordinates.append((int(3),int(3),int(router_on)))
            elif gcode_type == 'M5' or gcode_type == 'M05':
                coordinates.append((int(5),int(5),int(router_off)))
                
            else:
                print 'ignored: ',tokens
    return coordinates

def draw_coord(coordinates):
    fig, ax = plt.subplots()
    x = []
    y = []
    set_limits = True
    for x_coord, y_coord, tool_stat in xycoord:
        if set_limits:
            set_limits = False
            points, = ax.plot(x, y, marker='o', linestyle='--')
            xmax = math.ceil(max(coordinates,key=lambda item:float(item[0]))[0])
            xmin = math.ceil(min(coordinates,key=lambda item:float(item[0]))[0])
            ymax = math.ceil(max(coordinates,key=lambda item:float(item[1]))[1])
            ymin = math.ceil(min(coordinates,key=lambda item:float(item[1]))[1])
            print 'max found x', xmax
            print 'max found y', ymax
            #print max(coordinates,key=lambda item:item[1])[0]
            #return 0
            border = 0.1
            
            ax.set_xlim(xmin-xmin*border, xmax+xmax*border) 
            ax.set_ylim(ymin-ymin*border, ymax+ymax*border) 
        
        if tool_stat == router_state.router_xy:
            #print this and copy to c++ test code to test cnc firmware in CUTE
            #print 'machine.SetNextPosition(',x_coord,',', y_coord,');'
            
            x.append(x_coord)
            y.append(y_coord)
            points.set_data(x, y)
            #raw_input("press enter\n")#uncomment this line for single step animation
            plt.pause(0.001)
    try:
        #raw_input("press enter")
        plt.pause(60)
    except:
        print 'did you press the red x?'
        
if __name__ == '__main__':
    xycoord = get_gcode_data('PenHolderBottomPolyCircleSimplemultilayer.ngc')#('step_motion.nc', scale = 1)#('step_motion1.nc', scale = 1)#('rd_bm1.nc')#('RR.nc')#
    #xycoord = get_gcode_data('step_motion2.nc', scale = 1)#

#     for data in xycoord:
#         print data
    print len(xycoord)
    draw_coord(xycoord)
    #print router_state.router_up
    #print router_state.reverse_mapping[1]
    