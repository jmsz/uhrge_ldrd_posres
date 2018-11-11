#!/usr/bin/env python
from __future__ import division
import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from math import floor
from math import ceil
import numpy as np

####################################################
# List of global parameters
###################################################

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


filename = "./test.dat"
print(filename)

def ProcessFile(filename):
    '''Processes file and returns all gammas events as a nested list (list of
    list of list): gamma event --> interactions --> interaction parameters'''
    gammaList = []
    filer = open(filename, 'r')
    line = filer.readline()
    while (True):  # Iterate by gamma events
        line = filer.readline()
        if (not line):
            break
        line = line.split('\r\n')[0]
        interactionList = []
        if (line.split(' ')[0] == 'C'):
            while (True):  # Iterate by interactions
                line = filer.readline().split('\r\n')[0]
                if (line.split(' ')[0] == 'D' or not line):
                    break
                lineList = [float(x) for x in line.split(" ") if x != '']
                interactionList.append(lineList)
        else:
            print("File is corrupt")
            sys.exit()
        gammaList.append(interactionList)
    return gammaList


def ProcessFileTEST(filename):
    gammaList = [[[0.0, 0.0, 500.0, -39.9, -39.9, 2.8406],
                  [0.0, 0.0, 20.0, 0.0, 0.0, 2.8406]],
                 [[0.0, 0.0, 500.0, -40.0, -40.0, 2.8406]],
                 [[0.0, 0.0, 8.0, -39.5, -39.5, 2.8406],
                  [0.0, 0.0, 500.0, -39.5, -39.5, 2.8406]],
                 [[0.0, 0.0, 10.0, -39.2, -39.2, 2.8406]]]
    for elec in gammaList:
        print(elec)
    return gammaList

# Level 1: Fields extraction


def GetGammaNumberOfInteractions(gamma):
    '''Returns the number of interactions occurred for a given gamma'''
    return len(gamma)


def GetInteractionId(interaction):
    return interaction[0]


def GetInteractionEnergy(interaction):
    '''Returns the energy of an interaction related to a gamma'''
    return float(interaction[2])


def GetInteractionPosition(interaction):
    '''Returns the 3D position of an interaction [x, y, z], all in millimeters'''
    return interaction[3:6]


def GetGammaIds(gamma):
    '''Returns a list with all the ids of all the interactions related to a gamma'''
    ids = []
    for interaction in gamma:
        ids.append(interaction[0])
    return ids


def GetGammaEnergy(gamma):
    '''Returns the total energy associated with a gamma'''
    gammaEnergy = 0.
    for interaction in gamma:
        gammaEnergy += GetInteractionEnergy(interaction)
    return gammaEnergy


def GetGammaElectronEnergy(gamma):
    '''Returns the energy associated with all the electrons produced by a gamma'''
    gammaEnergy = 0.
    for interaction in gamma:
        if (GetInteractionId(interaction) == electronId):
            gammaEnergy += GetInteractionEnergy(interaction)
    return gammaEnergy


def GetGammaGammaEnergy(gamma):
    '''Returns the energy associated with all the gammas produced by a gamma'''
    gammaEnergy = 0.
    for interaction in gamma:
        if (GetInteractionId(interaction) == gammaId):
            gammaEnergy += GetInteractionEnergy(interaction)
    return gammaEnergy

#  Level 2: Data selection


def GetGammasToOnlyElectrons(gammaList):
    '''Return a list of those gammas that produced only electrons'''
    gammaToElectronsList = []
    for gamma in gammaList:
        if all([id == electronId for id in GetGammaIds(gamma)]):
            gammaToElectronsList.append(gamma)
    return gammaToElectronsList


def GetFullEnergyEvents(gammaList):  # TODO TODO
    '''Return a list of those gammas that produced only electrons'''
    gammaToElectronsList = []
    for gamma in gammaList:
        if all([id == electronId for id in GetGammaIds(gamma)]):
            gammaToElectronsList.append(gamma)
    return gammaToElectronsList

def GetPhotoelectricEvents(gammaList):  # TODO TODO
    '''Return a list of those gammas that produced only electrons'''
    gammaToElectronsList = []
    for gamma in gammaList:
        if all([id == electronId for id in GetGammaIds(gamma)]):
            gammaToElectronsList.append(gamma)
    return gammaToElectronsList

# Level 3: Data processing


def GetInteractionSignalChargeOnStrips(interaction):  # TODO
    '''Returns a list of the signals (one per strip) induced by a given
    secondary particle (electron) associated with a gamma'''
    charge = GetInteractionEnergy(interaction) * 1000. / 2.9
    interactionSignals = []
    for i in range(0, numberOfStrips):
        interactionSignals.append(0.)
    return interactionSignals


def GetGammaSignalChargeOnStrips(gamma):
    '''Returns a list of the signals (one per strip) induced by all the
    secondary particles (electrons) associated with a gamma'''
    gammaSignals = [0. for x in range(0, numberOfStrips)]
    for interaction in gamma:
        interactionSignals = GetInteractionSignalChargeOnStrips(interaction)
        gammaSignals = [gammaSignals[x] + interactionSignals[x] for x in range(0, numberOfStrips)]
    return gammaSignals


def GetGammaNumberOfHitStrips(gamma):
    '''Returns the number of strips whose signals exceed a given threshold'''
    gammaSignals = GetGammaSignalChargeOnStrips(gamma)
    numberOfHitStrips = 0
    for signal in gammaSignals:
        if signal > stripThreshold:
            numberOfHitStrips += 1
    return numberOfHitStrips


def GetDistributionOfNumberOfHitStrips(gammaList):
    '''Returns a list of the number of hit strips
    in the detector for all selected gammas'''
    numberOfHitsList = []
    for gamma in gammaList:
        numberOfHitsList.append(GetGammaNumberOfHitStrips(gamma))
    return numberOfHitsList


def GetDistributionOfGammaEnergies(gammaList, interactionId=None):
    '''Returns a list with the energy of the gammas'''
    gammaEnergies = []
    for gamma in gammaList:
        if (interactionId == electronId):
            gammaEnergies.append(GetGammaElectronEnergy(gamma))
        elif (interactionId == gammaId):
            gammaEnergies.append(GetGammaGammaEnergy(gamma))
        else:
            gammaEnergies.append(GetGammaEnergy(gamma))
    return gammaEnergies


def GetInteractionsPositions(gammaList, out):  # out = either list of numbers or list of lists 'num'/'lis'
    '''Returns three lists with the coordinates of all the secondaries of
    all gammas as either 'num' or 'list' (combined or as a list of lists)'''
    interactionsX = []
    interactionsY = []
    interactionsZ = []
    interactionsXlist = []
    interactionsYlist = []
    interactionsZlist = []
    for gamma in gammaList:
        Xlist = []
        Ylist = []
        Zlist = []
        for interaction in gamma:
            er = GetInteractionEnergy(interaction)
            if er >= float(stripThreshold):
                interactionPosition = GetInteractionPosition(interaction)
                interactionsX.append(interactionPosition[0])
                interactionsY.append(interactionPosition[1])
                interactionsZ.append(interactionPosition[2])
                Xlist.append(interactionPosition[0])
                Ylist.append(interactionPosition[1])
                Zlist.append(interactionPosition[2])
            else:
                pass
        interactionsXlist.append(Xlist)
        interactionsYlist.append(Ylist)
        interactionsZlist.append(Zlist)

    if out == 'lis':
        return interactionsXlist, interactionsYlist, interactionsZlist
    elif out == 'num':
        return interactionsX, interactionsY, interactionsZ
    else:
        return interactionsX, interactionsY, interactionsZ

# Level 5: Plotting results


def PlotEnergyHistogram(data):
    '''Plots the distribution of the number of hit strips'''
    plt.hist(data, bins=200)
    plt.xlabel('Energy [keV]')
    plt.savefig('EnergyHistogram.png')
    plt.show()


def PlotNumberOfStripsHistogram(data):
    '''Plots the distribution of the number of hit strips'''
    plt.hist(data, bins=20)
    plt.xlabel('Number of Strips - Threshold ' + str(stripThreshold) + 'e')
    plt.show()


def PlotInteractionsPositions(dataX, dataY, dataZ):
    '''Plots the position of all the secondary particles'''
    fig = plt.figure()
    ax = Axes3D(fig)
    ax.scatter(dataX, dataY, dataZ)
    ax.set_xlabel('x [mm]')
    ax.set_ylabel('y [mm]')
    ax.set_zlabel('z [mm]')
    plt.show()

# Main of the software (plots various things)


print("=========================================")
print("==                                     ==")
print("=============== STARTING ================")
print("==                                     ==")
print("=========================================")

gammaList = []
data = ProcessFile(filename)
print('---- number of events: ----')
print("unprocessed: ", len(data))

three_strip_events = []
for gamma in gammaList:
    instrip = 0
    for interaction in gamma:
        pos = GetInteractionPosition(interaction)
        z = pos[0]
        x = pos[1]
        y = pos[2]
        if -0.5 <= x <= 0.5 and -0.5 <= y <= 0.5:
            instrip += 1
        else:
            pass
    if instrip > 0:
        three_strip_events.append(gamma)

for gamma in three_strip_events:
    e_tot = GetGammaEnergy(gamma)
    n_tot = GetGammaNumberOfInteractions(gamma)
    e1
    e2
    e3
    e4
    e5
    e6
    e7
    e8
    e9
    x1
    x2
    x3
    x4
    x5
    x6
    x7
    x8
    x9
    for interaction in gamma:
        pos = GetInteractionPosition(interaction)
        z = pos[0]
        x = pos[1]
        y = pos[2]
        e = GetInteractionEnergy(interaction)
    if -0.5 <= x <= 0:
        e1
    elif 0 <= x <= 0.5:
    elif 0.5 <= x <= 1:
    else:
        print('x error')
    if -0.5 <= y <= 0:
    elif 0 <= y <= 0.5:
    elif 0.5 <= y <= 1:
    else:
        print('y error')
    strip1 = 0
    strip2 = 0
    strip3 = 0
    strip4 = 0
    strip5 = 0
    strip6 = 0



pitches = [0.5]
left_edge =  0 -  detectorDimensions[0] / 2
right_edge =  0 +  detectorDimensions[0] / 2
k = pitches[0]
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
print(len(boundaries))
print(ceil(len(boundaries)/2))
print(boundaries[len(boundaries) - 1])
print(boundaries[ceil(len(boundaries)/2)])

sys.exit()

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
