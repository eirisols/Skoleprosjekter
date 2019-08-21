import csv
import numpy as np
import time
import random
import generell_functions as gf
import statistics

start_time = time.time()

with open("european_cities.csv", "r") as f:
    data = list(csv.reader(f, delimiter=';'))

populationNumber = 100
numberCities = 10
generations = 400
fitnessRate = 0.01
mutationRate = 0.1

#makes a random number between 0 and 1
def randomNumber():
    number = random.random()

    return number

#mutates one child
def mutate(liste, a, b):
    tempListe = list(liste)
    tall1 = tempListe[a]
    tall2 = tempListe[b]
    tempListe[a] = tall2
    tempListe[b] = tall1

    return tempListe

#makes a list of the fitness of the population
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

#chooses one parent from the population
def nextParent(population, popFitness, fitnessRate):
    currentParent = None

    #randNumb = randomNumber()


    while fitnessRate > 0.0:
        randomNumber = random.randint(0, len(popFitness)-1)
        fitnessRate -= popFitness[randomNumber]
        #print fitnessRate
        if fitnessRate <= 0.0:
            currentParent = population[randomNumber]
            break

    return currentParent

#Makes a list of parents, takes halv the number of the population
def makeParents(population, fitnessRate):
    popFitness = calcPopFitness(population)
    parents = list()
    #maxFit = max(popFitness)
    #indexMax = popFitness.index(maxFit)
    #parents.append(population[indexMax])

    for x in range((populationNumber//2)):
        parents.append(nextParent(population, popFitness, fitnessRate))

    return parents

#makes two children, calls orderCrossover inn generell_functions
def orderCrossover(parent1, parent2, randomNumber1, randomNumber2):
    child1 = gf.orderCrossover(parent1, parent2, randomNumber1, randomNumber2)
    child2 = gf.orderCrossover(parent2, parent1, randomNumber1, randomNumber2)
    return child1, child2

#makes a list of children and returns it
def makeChildren(parents):
    tempParents = list(parents)
    children = list()
    while tempParents:
        parent1 = tempParents.pop()
        parent2 = tempParents.pop()
        randomNumber1 = random.randint(0, len(parent1)-1)
        #print "tall1"
        #print randomNumber1
        randomNumber2 = random.randint(randomNumber1, len(parent1)-1)
        #print "tall2"
        #print randomNumber2
        child1, child2 = orderCrossover(parent1, parent2, randomNumber1, randomNumber2)
        children.append(child1)
        children.append(child2)

    return children

def mutateChildren(children):
    tempChildren = list()
    for child in children:
        mutateChance = random.random()
        if mutateChance < mutationRate:
            randomNumber1 = random.randint(0, numberCities-1)
            randomNumber2 = random.randint(0, numberCities-1)
            tempChild = mutate(child, randomNumber1, randomNumber2)
            tempChildren.append(tempChild)
        else:
            tempChildren.append(child)
    return tempChildren


def getShortest(population):
    shortestPath = np.inf
    shortestCities = None
    for x in range(0, populationNumber):
        currentPath = gf.calcPath(population[x], data)

        if currentPath < shortestPath:
            shortestPath = currentPath
            shortestCities = population[x]
    return shortestPath, shortestCities

def selection(population):
    newPop = list(population)
    popFitness = calcPopFitness(newPop)
    for i in range(populationNumber//2):
        minFit = min(popFitness)
        indexMin = popFitness.index(minFit)
        newPop.pop(indexMin)
        popFitness.pop(indexMin)
    return newPop
#Makes population

def geneticA():
    population = list()
    newPopulation = list()
    #values = [0.0] * populationNumber

    listOfShortestPath = list()
    for x in range(0, populationNumber):
        population.append(np.random.permutation(numberCities))

    for x in range(0, generations):
        parents = list()
        children = list()
        #print "Foreldre"
        #print list(population)

        #Makes parents
        parents = makeParents(population, fitnessRate)
        #print "foreldre"
        #print parents
        #Makes children
        children = makeChildren(parents)
        #print "barn"
        #print children
        #Mutates children
        children = mutateChildren(children)
        #print "muterte barn"
        #print children
        #
        population.extend(children)

        newPopulation = selection(population)

        population = newPopulation
        shortestPath, shortestCities = getShortest(population)
        listOfShortestPath.append(shortestPath)
        print ("Gen ", x, ": Shortest path is ", shortestPath,)
        #print (shortestCities)

        random.shuffle(population)
    return listOfShortestPath

listAll = list()
shortestList = list()

for i in range(20):
    tempList = geneticA()
    listAll.append(tempList)
    tempShort = min(tempList)
    shortestList.append(tempShort)

print ("Best result: %s" % (min(shortestList)))
print ("Worst result: %s " % (max(shortestList)))
print ("Average result: %s" % (statistics.mean(shortestList)))
print ("Standard deviation: %s" % (statistics.stdev(shortestList)))
#print values
#print ("Final shortest path ", shortestP)
#print shortestCities
print ("%s seconds" % (time.time() - start_time))
