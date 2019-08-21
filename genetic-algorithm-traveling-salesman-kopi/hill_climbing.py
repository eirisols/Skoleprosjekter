import csv
import numpy as np
import time
import random
import statistics

start_time = time.time()

with open("european_cities.csv", "r") as f:
    data = list(csv.reader(f, delimiter=';'))

    def bytt(liste, a, b):
        tempListe = liste.copy()
        tall1 = tempListe[a]
        tall2 = tempListe[b]
        tempListe[a] = tall2
        tempListe[b] = tall1

        return tempListe

def hillClimber():
    numberCities = 10
    path = 0
    shortestPath = 0

    perm = np.random.permutation(numberCities)

    for x in range (0, numberCities):
        if x == numberCities-1:
            shortestPath = shortestPath + float(data[perm[x]+1][perm[0]])
        else :
            shortestPath = shortestPath + float(data[perm[x]+1][perm[x+1]])

    for i in range(0, 3000):
        path = 0
        randomNumber1 = random.randint(0, numberCities-1)
        randomNumber2 = random.randint(0, numberCities-1)
        tempPerm = bytt(perm, randomNumber1, randomNumber2)

        for x in range (0, numberCities):
            if x == numberCities-1:
                path = path + float(data[tempPerm[x]+1][tempPerm[0]])
            else :
                path = path + float(data[tempPerm[x]+1][tempPerm[x+1]])


        if(path < shortestPath):
            perm = tempPerm
            shortestPath = path

    #print (shortestPath)
    #print (perm)

    return shortestPath

listPaths = list()

for i in range(20):
    shortestPath = hillClimber()
    listPaths.append(shortestPath)

print ("Beste resultat %s" % (min(listPaths)))
print ("Verste resultat %s " % (max(listPaths)))
print ("Gjennomsnitt av resultater %s" % (statistics.mean(listPaths)))
print ("Standard deviation: %s" % (statistics.stdev(listPaths)))

print ("%s sekunder" % (time.time() - start_time))
