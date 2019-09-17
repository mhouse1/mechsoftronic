'''
Created on May 5, 2015

@author: MHouse1

@brief    plots gcode in realtime
'''
#!/usr/bin/env python
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import math

def get_xy_coordinates(input_file = 'bridesmaid_05.nc'):
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
                coordinates.append((tokens[1][1:],tokens[2][1:],1))
        else:
            print 'ignored:',tokens
    return coordinates
xycoord = get_xy_coordinates('rd_bm1.nc')#('RR.nc')#
#print xycoord[-5:]
#exit()
def _update_plot(i, fig, scat):
    #print 'i  =',i
    if i == 0:
        return scat,
    #scat.set_offsets(xycoord[-i:])#(([0, i],[50, i],[100, i]))
    #print xycoord[-i:]
    #print('Frames: %d' %i)
    #plt.plot(get_axi(xycoord,0),get_axi(xycoord,1))
    xline = []
    yline = []
    for x in xycoord[:i+1]:
        xline.append(x[0])
        yline.append(x[1])
    lines.set_data(xline,yline)#([x[0] for x in xycoord[-i+1:]],[x[1] for x in xycoord[-i+1:]])
    points.set_data(xline[-1:],yline[-1:])#(xycoord[-i:][0][0],xycoord[-i:][0][1])#<-use for reversed routing
    return scat,

fig =  plt.figure()                

x = [0]
y = [0]

ax = fig.add_subplot(111)
ax.grid(True, linestyle = '--', color = '0.90')

ax.set_xlim([0, math.ceil(float(max(xycoord,key=lambda item:float(item[0]))[0]))])
ax.set_ylim([0, math.ceil(float(max(xycoord,key=lambda item:float(item[1]))[1]))])

get_axi = lambda item, num: [x[num] for x in item]
print  get_axi(xycoord,0)
points, = plt.plot([],[],'ro')
lines, = plt.plot([],[],'g:',alpha=0.9)
scat = plt.scatter(x, y, c = [50])
scat.set_alpha(0.05)

anim = animation.FuncAnimation(fig, _update_plot, fargs = (fig, scat),
                               frames = len(xycoord)+1, interval = 50,blit = False, repeat = False)
              
plt.show()