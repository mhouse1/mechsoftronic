'''
Created on May 1, 2015 <my birthday!>

@author: MHouse1
'''
import matplotlib.pyplot as plt 
import math

def get_xy_coordinates(input_file = 'bridesmaid_inner_01.nc'):
    gcode_file = file(input_file).readlines()
    
    coordinates = []
    for line in gcode_file:
        #remove line break and split on the spaces
        tokens =  line[:-1].split(' ')
        
        #if GCODE found in list
        if tokens[0] in ['G0','G1','G2','G3']:
            #print tokens
            #if next two parameter begins with X and Y
            if tokens[1][0] =='X' and tokens[2][0] == 'Y':
                #print tokens
                #print tokens[1][1:],tokens[2][1:]
                coordinates.append((tokens[1][1:],tokens[2][1:]))
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
            xmax = math.ceil(float(max(coordinates,key=lambda item:float(item[0]))[0]))
            ymax = math.ceil(float(max(coordinates,key=lambda item:float(item[1]))[1]))
            print 'max found x', xmax
            print 'max found y', ymax
            print max(coordinates,key=lambda item:item[1])[0]
            #return 0
            ax.set_xlim(0, xmax) 
            ax.set_ylim(0, ymax) 
        else:
            x_coord, y_coord = coordinates.pop()
            #print x_coord, y_coord
            x.append(x_coord)
            y.append(y_coord)
            points.set_data(x, y)
        #if str(t)[-1] == '0': 
        plt.pause(0.1)
    try:
        plt.pause(60)
    except:
        print 'did you press the red x?'
xycoord = get_xy_coordinates('bridesmaid_05.nc')#('RR.nc')#

print len(xycoord)
draw_coord(xycoord)
