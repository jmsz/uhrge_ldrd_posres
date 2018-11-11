from __future__ import division
import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from math import floor
from math import ceil
import numpy as np

peakEnergy           = 0.0  # 499.99  # keV
stripThreshold       = 0.0  # keV
stripWidth           = 0.25    # In millimeters
stripGap             = 0.25
stripPitch           = stripWidth + stripGap  # In millimeters #TODO
detectorDimensions   = [80., 80., 15.]  # x, y, z in millimeters
numberOfStripsAlongX = floor(detectorDimensions[0] / stripPitch)  # These are chosen to be on top (and tied to + side of bias voltage)
numberOfStripsAlongY = floor(detectorDimensions[1] / stripPitch)  # These are chosen to be on bottom (and tied to - side of bias voltage)
numberOfStrips       = numberOfStripsAlongX + numberOfStripsAlongY
detectorBias         = 1000. # In volts #TODO (FROM DEPLATION + 200V)
electronId           = 0.
gammaId              = 1.

pitches = [0.5]
left_edge =  0 -  detectorDimensions[0] / 2
right_edge =  0 +  detectorDimensions[0] / 2

for k in pitches:
    histdata = []
    number_of_strips_x = int(floor(detectorDimensions[0] /k))
    edge_space_x = (detectorDimensions[0] - k * number_of_strips_x) / 2

    boundaries = []
    boundaries.append(left_edge)
    boundaries.append(left_edge + edge_space_x + k)
    for i in range(2, number_of_strips_x, 1):
        boundaries.append(boundaries[i-1] + k)
    boundaries.append(right_edge)
    print ("BOUNDARIES")
    print (boundaries)

    print ("pitch: ", k)
    for j in gamma_range_x:
        #print ("range: ", j)
        numbtwn = (j / k)
        #print ("num_btwn: ", numbtwn)
        histdata.append(numbtwn)

    strips_hit_gamma_x_all = []
    for gamma in singles_data:
        strips_hit_gamma_x=[]
        for electron in gamma:
            x =GetInteractionPosition(electron)
            x = x[0]

            if not boundaries[0] <= x <= boundaries[-1]:
                print (boundaries[0], " " , x, " ", boundaries[-1])
                print ("interaction coordinate out of range of detector")
            else:
                for j in range(1, number_of_strips_x + 1, 1):
                    if (x == boundaries[j]):
                        strips_hit_gamma_x.append(j)
                    elif (x == boundaries[j-1]):
                        strips_hit_gamma_x.append(j-1)
                    elif (boundaries[j-1] < x < boundaries[j]):
                        strips_hit_gamma_x.append(j-1)

        if (len(strips_hit_gamma_x) > 1):
            for i in range(1, len(strips_hit_gamma_x) , 1):
                btwn = abs(strips_hit_gamma_x[i] - strips_hit_gamma_x[i-1])
                strips_hit_gamma_x_all.append(btwn)

    plt.figure(k)
    plt.clf()
    plt.cla()
    plt.hist(strips_hit_gamma_x_all, floor(detectorDimensions[0] / k))
    plt.xlabel('number of strips between gamma interactions')
    plt.title('pitch: ' + str(k))
    plt.savefig('pitch_' + str(k) + 'fullE_num_strips_btwn_gs.png')
    plt.cla()
    plt.clf()
    #plt.show()

#####sys.exit()
#plt.show()
#num = raw_input(" 1 to exit")
#if num ==1:
#    sys.exit()
# many many strips if interaction in center
# in mm
#sys.exit()
