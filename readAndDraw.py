#!/usr/bin/env python3
# the shebang line indicates this file be passed as a python3 script
# "#!/usr/bin/env python3" is discouraged for compatibility reasons

from matplotlib import pyplot as plt # plotting functions
import os # getcwd

# to set input & output paths
cwd = os.getcwd() # current working directory
outDataPath =cwd +"/out/outTable.txt"
outFigPath =cwd +"/plt/outFig.png"

# to retrieve line by line
handleText =open(outDataPath)
content =handleText.read()
listLine =content.split('\n') # newline as delimiter

# to jump several lines to reach the main table
numParseLine =4 # parse this many lines, as title
textTitle =""
for _ in range( 0, numParseLine ):
    textTitle += listLine[0] +'\n'
    listLine.pop(0)
# end for
textTitle =textTitle[:-1] # remove the last newline

numJumpLine =4 # ignore this more lines
for i in range( 0, numJumpLine ):
    listLine.pop(0)
# end for

# to set axis & title
plt.close("all")
plt.title( textTitle, fontsize =15 )
plt.xlabel( "Signal-to-noise rate (S.N.R.), in dB", fontsize =12 )
plt.ylabel( "Bit error rate (B.E.R.), in dB", fontsize =12 )

# to set plot style
numLineStyle =4
listLineStyle =['-', '--', '-.', ':'] # the larger, the thicker
numColor =5
listColor =['r', 'g', 'c', 'b', 'k'] # red, green, cyan, blue, black
numMarker =6
listMarker =['v', '^', 'o', 's', '*', 'D'] # triangle down, triangle up, circle, square, star, diamond
sizeWidth =3 # fixed line width
sizeMarker =7 # fixed marker size

vecX =[] # to set empty list to enlarge its scope
# split w/o arguments parses any number of spaces and tabs
listStr =listLine[0]. split()
for tmpStr in listStr:
    # strip decimal mark to determine, whether a decimal number
    if not tmpStr. replace('-',''). replace('.',''). isdigit():
        continue
    else:
        vecX.append(float(tmpStr))
# end for
listLine.pop(0)

for idx, line in enumerate(listLine):
    listStr =line.split()
    if not listStr: # "listStr" is empty
        continue
    # end if empty

    tmpLabel =listStr[0]
    listStr.pop(0)

    vecY =[] # to create list to enlarge its scope
    
    for tmpStr in listStr:
        # to strip decimal mark & minus sign to check whether a decimal number
        if not (tmpStr. replace('-',''). replace('.',''). isdigit()):
            continue
        else:
            vecY.append(float(tmpStr))
    # end for each word

    plt.plot( vecX, vecY,
            markersize =sizeMarker,
            linewidth =sizeWidth,
            linestyle =listLineStyle[ int( idx % numLineStyle ) ],
            color =listColor[ int( idx % numColor ) ],
            marker =listMarker[ int( idx % numMarker ) ],
            label =tmpLabel )
# end for each line

# to set legend and save figure
distShift =-9.0 # shifted, to the left, considerably
plt.legend( loc='center left', borderaxespad =distShift )
plt.savefig( outFigPath, bbox_inches ="tight" )
