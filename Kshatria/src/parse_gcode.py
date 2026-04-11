'''
Created on May 1, 2015

@author: MHouse1

@details: contains function to parse gcode and returns coordinates to calling function
          and function to plot 2D result in realtime to visualize the parsed results
          before sending to firwmare
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

router_state = enum('router_off', 'router_on', 'router_up','router_down','router_xy','router_z','cnc_pause')

def get_gcode_data(input_file = 'bridesmaid_inner_01.nc',scale=10000):
    '''
    Reads and parses gcode returns a coordinate list for calling function
    to send to firmware
    '''
    #read all lines in gcode file
    with open(input_file) as f:
        gcode_file = f.read().splitlines()
    
    #does post processing on lines read from gcode file
    #this is done to get the gocde file into a 
    #standardized format 
    #what is done:
    #    replace three spaces with no space ex: '   ' = ''
    #    replace two spaces with no space ex: '  ' = ''
    #gcode generation programs supported
    #    inkscape - laser plugin
    #    makercam - aka partcam.swf
    #    DXF2GCODE
    post_processed = []
    for sentence in gcode_file:
        #sentence = sentence.replace('   ','')
        #sentence = sentence.replace('  ','')
        #sentence = sentence.replace(' ','')
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
                
                first_coord_type = tokens[1][0]
                if first_coord_type == 'X':
                    #second_coord_type = tokens[2][0]
                    parsed_coord = line.split('X')[1].replace(' ','') #condensed string no space
                    #print parsed_coord, 'parsed1'
                    parsed_coord = parsed_coord.replace('I',' I')#add a space infront of I
                    parsed_coord = parsed_coord.replace('F',' F')#add a space infront of F
                    #print parsed_coord, 'parsed2'
                    parsed_coord = parsed_coord.split(' ')[0].split('Y')#separate on space
                    #print parsed_coord, 'parsed'
                    xnum = parsed_coord[0]#tokens[1][1:]
                    ynum = parsed_coord[1]#tokens[2][1:]
                elif first_coord_type == 'Z':
                    parsed_coord = line.split('Z')[1].replace(' ','') #condensed string no space
#                     print parsed_coord, 'parsed1'
#                     parsed_coord = parsed_coord.replace('I',' I')#add a space infront of I
#                     print parsed_coord, 'parsed2'
                    parsed_coord = parsed_coord.replace('F',' F')#add a space infront of F
                    parsed_coord = parsed_coord.split(' ')[0]
                    #print parsed_coord, 'parsed3'
                    znum = parsed_coord#tokens[1][1:]
                    
                    
                #print tokens
                #if next two parameter begins with X and Y
                if first_coord_type =='X':# and second_coord_type == 'Y':
                    #print tokens[1][1:] , tokens[2][1:]
                    #print line, Decimal(xnum) , Decimal(ynum)
                    #print tokens
                    #print tokens[1][1:],tokens[2][1:]
                    #convert (x,y) string to float then scale it then convert to int
                    coordinates.append((int(Decimal(xnum)*Decimal(scale)),int(Decimal(ynum)*Decimal(scale)),int(router_state.router_xy)))
                    #print coordinates[-1] #print last item in list
                elif first_coord_type =='Z':
                    #print line,'zline' , znum, 'zum'
                    #print 'gcode z motion', double(znum)
                    if gcode_type =='G0' or gcode_type =='G1':
                        coordinates.append((int(Decimal(znum)*Decimal(scale)),1,int(router_state.router_z)))
                    else:
                        raise ValueError('unexpected gcode type with command z')
                    
                else:
                    print 'ignored:*',tokens, first_coord_type#, second_coord_type
                    print 'line:*',line
            elif gcode_type == 'M3' or gcode_type == 'M03':
                coordinates.append((int(3),int(3),int(router_state.router_on)))
            elif gcode_type == 'M5' or gcode_type == 'M05':
                coordinates.append((int(5),int(5),int(router_state.router_off)))
            elif gcode_type == 'M0' or gcode_type == 'M00':
                coordinates.append((int(6),int(6),int(router_state.cnc_pause)))                
            else:
                print 'ignored: ',tokens
                if gcode_type == 'M0':
                    raw_input('detected M0 pause command')
    return coordinates

def draw_coord(coordinates):
    '''
    draw a 2D view of coordinates in realtime to visualize parsed coordinates
    drawn in order from beginning to end
    '''
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
            print 'machine.SetNextPosition(',x_coord,',', y_coord,');'
            
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
    #get a set of coordinates
    #xycoord = get_gcode_data('snowflake6optimized.ngc', scale = 10000)
    xycoord = get_gcode_data('leaf_engrooving_bit_1.ngc', scale = 10000)
    #xycoord = get_gcode_data('RR.nc', scale = 1)
    #xycoord = get_gcode_data('rect150x100.nc', scale = 1)
    #xycoord = get_gcode_data('rd_bm1.nc', scale = 10000)
    #xycoord = get_gcode_data('PenHolderBottomPolyCircleSimplemultilayer.ngc', scale = 10000)
    #xycoord = get_gcode_data('step_motion1.nc', scale = 1)

    #draw start drawing
    print len(xycoord)
    draw_coord(xycoord)
    #print router_state.router_up
    #print router_state.reverse_mapping[1]
    