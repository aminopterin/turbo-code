#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import os # getcwd

cwd = os.getcwd() # current working directory
outFigPath =cwd +"/outFig.png"

x =[1,2,3,4]
y =[1,4,9,16]
plt.axis([0, 6, 0, 20])

plt.plot(x,y)
#handleFig =plt.figure()
plt.savefig( outFigPath )
