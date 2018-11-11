from __future__ import division, print_function
from math import floor
import numpy as np
import matplotlib as plt

# IMPORT DATA
number_events_simulated = 10
number_strips = 6
numintrxns = 0
#data = [[10 lists like this], []] in list
data = np.zeros((number_events_simulated, number_strips))
#data = np.array()
print(data)
for i in range(0, number_events_simulated, 1):
    filename = "Strip_SS_" + str(i) + ".txt"
    rawdata = np.genfromtxt(filename, skip_header=1)
    print (rawdata[6:7,1:2]) # indicies = row, col
    number_samples  = int(floor(len(rawdata[6:7,:]) / 10))
    print(number_samples)
    print(rawdata[6:7,:])
    print(len(rawdata[6:7,:]))
    print(floor(len(rawdata[6:7,:]) / 10))
    print(int(floor(len(rawdata[6:7,:]) / 10)))
    for j in range(0, number_strips, 1):
        for k in range(0, number_samples, 1): #number of samples
            data[j,numintrxns] = rawdata[6 + j,k] #take every tenth sample (10 ns)
            print('--')
            print(data[j,numintrxns])
            numintrxns += 1
print(data)
