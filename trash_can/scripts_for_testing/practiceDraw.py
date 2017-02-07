#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import os # getcwd

cwd = os.getcwd() # current working directory
outDataPath =cwd +"/outTable"
outFigPath =cwd +"/outFig.png"

def f(a,t):
    return np.exp(-0.1*a*t)

numWidth =4
listWidth =[1, 2, 3, 4] # the larger, the thicker
numColor =5
listColor =['r', 'g', 'c', 'b', 'k'] # red, green, cyan, blue, black 
numMarker =6
listMarker =['v', '^', 'o', 's', '*', 'D'] # triangle down, triangle up, circle, square, star, diamond
sizeMarker =8 # fixed marker size

x =list(np.arange(0, 3, 0.2))
numDataSet =10

plt.close("all")
for i in range(numDataSet):
    y =[ f(i,el) for el in x ]
    #print(y)
    plt.plot( x, y,
            markersize =sizeMarker,
            linewidth =listWidth[ int( i % numWidth ) ],
            color =listColor[ int( i % numColor ) ],
            marker =listMarker[ int( i % numMarker ) ],
            label =str(i) )

# legend present, to the left, shifted considerably
plt.legend( loc='center left', borderaxespad=-7. )
plt.savefig( outFigPath, bbox_inches ="tight" )
