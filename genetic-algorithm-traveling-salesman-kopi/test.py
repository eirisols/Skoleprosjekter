import generell_functions as gf
import csv
import numpy as np
import operator
#import genetic

with open("european_cities.csv", "r") as f:
    data = list(csv.reader(f, delimiter=';'))

#Krossover test
#parent1 = [0,2,1,3,4]
#parent2 = [3,0,2,1,4]

#print parent1
#print parent2

#child = gf.orderCrossover(parent1, parent2, 1,3)

#print child

#Fitness testing

populationNumber = 60
numberCities = 24
population = list()
values = list()

def calcPopFitness(population):
    popFitness = list()
    path = 0.0

    for pop in population:
        path = gf.calcPath(pop, data)

        popFitness.append(1 / path)

    fitMin = min(popFitness) - (min(popFitness)/2)
    fitSum = sum(popFitness) - (fitMin * len(popFitness))
    for x in range(0, len(popFitness)):
        popFitness[x] = (popFitness[x] - (fitMin)) / fitSum

    return popFitness

for x in range(0, populationNumber):
    population.append(np.random.permutation(numberCities))
    values.append(gf.calcPath(population[x], data))

fitness = calcPopFitness(population)
print (values)
print (fitness)

#list = [1,2,3,4]
#dict = {0:400.0, 1: 300.0, 2:500, 3:100}

#sortet_dict = sorted(dict.items(), key = operator.itemgetter(1))
#print sortet_dict
#while list:
#    list.pop()
#print list
