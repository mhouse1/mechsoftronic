'''
Created on May 5, 2015

@author: MHouse1
'''
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
from matplotlib.collections import LineCollection
from matplotlib.colors import ListedColormap, BoundaryNorm

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
ax = plt.axes(xlim=(0, 2), ylim=(-2, 2))

cmap = ListedColormap(['b', 'r', 'b'])
norm = BoundaryNorm([0, 1, 1.5, 2], cmap.N)

line = LineCollection([], cmap=cmap, norm=norm,lw=2)
line.set_array(np.linspace(0, 2, 1000))
ax.add_collection(line)

# initialization function: plot the background of each frame
def init():
    line.set_segments([])
    return line,

# animation function.  This is called sequentially
def animate(i):
    x = np.linspace(0, 2, 1000)
    y = np.sin(2 * np.pi * (x - 0.01 * i))
    points = np.array([x, y]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)
    print segments
    line.set_segments(segments)
    return line,

# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
                           frames=200, interval=20)

plt.show()