'''
Created on May 1, 2015 <my birthday!>

@author: MHouse1
'''
import matplotlib.pyplot as plt 
import math

def get_gcode_data(input_file = 'bridesmaid_inner_01.nc',scale=10000):
    with open(input_file) as f:
        gcode_file = f.read().splitlines()
    tool_on = 1
    tool_off = 0
    tool_status = tool_off #tool will be off by default
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
                    coordinates.append((int(float(tokens[1][1:])*scale),int(float(tokens[2][1:])*scale),tool_status))
            elif gcode_type == 'M3' or gcode_type == 'M03':
                tool_status = tool_on
            elif gcode_type == 'M5' or gcode_type == 'M05':
                tool_status = tool_off
                
            else:
                print 'ignored:',tokens
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
    xycoord = get_gcode_data('RR.nc')#('rd_bm1.nc')#
    for data in xycoord:
        print data
    print len(xycoord)
    draw_coord(xycoord)
