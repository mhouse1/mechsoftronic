'''
Created on May 1, 2015

@author: MHouse1
'''
import matplotlib.pyplot as plt 
import math

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

router_state = enum('router_off', 'router_on', 'router_up','router_down','router_xy')

def get_gcode_data(input_file = 'bridesmaid_inner_01.nc',scale=10000):
    with open(input_file) as f:
        gcode_file = f.read().splitlines()
    
    #{router_off = 0, router_on, router_up, router_down,router_xy};
    router_off  = 0
    router_on   = 1
    router_up   = 2
    router_down = 3
    router_xy   = 4

    
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

                    coordinates.append((int(float(tokens[1][1:])*scale),int(float(tokens[2][1:])*scale),int(router_xy)))
                elif tokens[1][0] =='Z':
                    
                    if gcode_type =='G1':
                        coordinates.append((int(0),int(0),int(router_down)))
                    elif gcode_type =='G0':
                        coordinates.append((int(7),int(7),int(router_up)))
                    else:
                        raise ValueError('unexpected gcode type with command z')
                    
                else:
                    print 'ignored:*',tokens
            elif gcode_type == 'M3' or gcode_type == 'M03':
                coordinates.append((int(0),int(0),int(router_on)))
            elif gcode_type == 'M5' or gcode_type == 'M05':
                coordinates.append((int(3),int(3),int(router_off)))
                
            else:
                print 'ignored: ',tokens
    return coordinates

def draw_coord(coordinates):
    fig, ax = plt.subplots()
    x = []
    y = []
    
    for t in range(len(coordinates)):
        if t == 0:
            points, = ax.plot(x, y, marker='o', linestyle='--')
            xmax = math.ceil(max(coordinates,key=lambda item:float(item[0]))[0])
            ymax = math.ceil(max(coordinates,key=lambda item:float(item[1]))[1])
            print 'max found x', xmax
            print 'max found y', ymax
            #print max(coordinates,key=lambda item:item[1])[0]
            #return 0
            ax.set_xlim(0, xmax) 
            ax.set_ylim(0, ymax) 
        else:
            x_coord, y_coord, tool_stat = coordinates.pop()
            #print x_coord, y_coord
            x.append(x_coord)
            y.append(y_coord)
            points.set_data(x, y)
        #if str(t)[-1] == '0': 
        #plt.pause(0.1)
    try:
        plt.pause(60)
    except:
        print 'did you press the red x?'
        
if __name__ == '__main__':
#     xycoord = get_gcode_data('rd_bm1.nc')#('RR.nc')#
#     for data in xycoord:
#         print data
#     print len(xycoord)
#     draw_coord(xycoord)
    print router_state.router_up
    print router_state.reverse_mapping[1]
    