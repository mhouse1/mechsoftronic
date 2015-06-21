'''
Created on May 5, 2015

@author: MHouse1

@brief    demonstration of realtime graph updates using coordinates
'''
#!/usr/bin/env python
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

def _update_plot(i, fig, scat):
    scat.set_offsets(([0, i],[50, i],[100, i]))
    print('Frames: %d' %i)

    return scat,

fig =  plt.figure()                

x = [0, 50, 100]
y = [0, 0, 0]

ax = fig.add_subplot(111)
ax.grid(True, linestyle = '-', color = '0.75')
ax.set_xlim([-50, 200])
ax.set_ylim([-50, 200])

scat = plt.scatter(x, y, c = x)
scat.set_alpha(0.8)

anim = animation.FuncAnimation(fig, _update_plot, fargs = (fig, scat),
                               frames = 100, interval = 100)
              
plt.show()