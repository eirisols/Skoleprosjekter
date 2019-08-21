"""
    This pre-code is a nice starting point, but you can
    change it to fit your needs.
"""
import numpy as np
import random
import math
import pandas as pd

class mlp:
    def __init__(self, inputs, targets, nhidden):
        self.beta = 1
        self.eta = 0.1
        self.momentum = 0.0
        self.targets = targets
        self.nhidden = nhidden
        #Number of iterations the algorithm goes before checking the Error
        self.numberIterations = 15
        #How many times the neural network gets a worse error before it terminates
        self.endError = 10
        #
        self.printInfo = True
        #print(len(inputs[0]))
        #print(inputs[0])
        #print(len(targets[0]))
        ##print(targets[0])
        #print(nhidden)

        self.hidden, self.output = self.makeWeights(inputs, targets, nhidden)

        #print(self.hidden[0])
        #print(len(self.hidden[0]))
        #print(len(self.hidden))

        #print("output")
        #print(len(self.output[0]))
        #print(len(self.output))

        #to check if it learned

    # You should add your own methods as well!
    def makeWeights(self, inputs, targets, nhidden):
        hidden = []
        output = []
        for i in range(nhidden):
            temp = []
            for x in range(len(inputs[0])+1):
                temp.append(random.uniform(-1.0, 1.0))
            hidden.append(temp)

        for i in range(len(targets[0])):
            temp = []
            for x in range(nhidden+1):
                temp.append(random.uniform(-1.0, 1.0))
            output.append(temp)

        return hidden, output
    #helper fucntion for forward
    def forwardHelp(self, inputs, weights):
        list = []
        for i in range(len(weights)):
            sum = weights[i][0]*-1
            for x in range(len(inputs)):
                sum += weights[i][x+1]*inputs[x]
            list.append(sigmoid(sum))
        return list

#needs to change
    def earlystopping(self, inputs, targets, valid, validtargets):

        #listA, listB = self.forward(valid)

        #self.testOutput(listB, validtargets)
        a = 0
        lowestError = 10000
        teller = 0
        i = 0
        bestWeights = [self.hidden, self.output]

        while True:

            self.train(inputs, targets)
            if i == a:
                #print("Midt i test")
                listA, listB = self.forward(valid)
                #self.testOutput(listB, validtargets)
                sumError = self.getError(listB, validtargets)
                if self.printInfo:
                    print('Iteration: ', i)
                    print("Error: ", sumError)
                if sumError < lowestError:
                    bestWeights = [self.hidden, self.output]
                    lowestError = sumError
                    teller = 0
                elif teller >= self.endError:
                    self.hidden = bestWeights[0]
                    self.output = bestWeights[1]
                    break
                else:
                    teller += 1
                #print("Midt i test")
                a += self.numberIterations;
            i += 1
        #print('Ferdig siste test')

        if self.printInfo:
            listA, listB = self.forward(valid)
            self.testOutput(listB, validtargets, False)
            print('Ferdig siste test')
        #To check if i learned
        #listA, listB = self.forward(inputs)
        #print (listB[0])
        #print (targets[0])

    def getError(self, outputs, targets):

        sumError = 0;
        for i in range(len(outputs)):
            for x in range(len(outputs[i])):
                sumError += ((outputs[i][x] - targets[i][x])**2)
        sumError = sumError/2
        return sumError

    def testOutput(self, outputs, targets, test):
        if test == False:
            outputs = self.changeOutput(outputs)
            targets = np.array(targets).tolist()
        antallRiktig = 0;

        for i in range(len(outputs)):
            if(outputs[i] == targets[i]):
                antallRiktig += 1
            if test:

                print("Input: " , i)
                print(outputs[i])
                print(targets[i])

        print(antallRiktig, " : ", len(targets))
    def changeOutput(self, outputs):
        lists = []
        for i in outputs:
            tempList = []
            for x in range(len(i)):
                if i[x] > 0.5:
                    tempList.append(1.0)
                else:
                    tempList.append(0.0)
            lists.append(tempList)
        return lists

#training. Kan kanskje dele denne opp i hjelpefunskjoner
    def train(self, inputs, targets):

        #Randomiserer inputs og targets
        sortsInput = zip(inputs, targets)
        random.shuffle(sortsInput)
        inputs, targets = zip(*sortsInput)

        listA, listY = self.forward(inputs)

        #gaar gjennom for hver input
        for i in range((len(inputs)/2)):
            i = random.randint(0, (len(inputs)-1));
            delta1 = []
            delta2 = []

            for x in range(len(listY[i])):
                delta1.append((listY[i][x] - targets[i][x]) * listY[i][x] * (1 - listY[i][x]))

            for x in range(len(listA[i])):
                sum = 0
                for j in range(len(listY[i])):
                    sum += self.output[j][x] * delta1[j]
                temp = listA[i][x] * (1 - listA[i][x]) * sum
                delta2.append(temp)
            #endre vektene her. Ha en hjelpemetode

            self.trainHelp(self.output, listA[i], delta1)
            self.trainHelp(self.hidden, inputs[i], delta2)


        #print('kjorer training')

    def trainHelp(self, weights, inputs, delta):

        for i in range(len(weights)):
            weights[i][0] = self.updateWeights(weights[i][0], delta[i], -1)
            for x in range(1, len(weights[i])):
                weights[i][x] = self.updateWeights(weights[i][x], delta[i], inputs[x-1])

    def updateWeights(self, weights, delta, input):
        return weights - (self.eta * delta * input)
#forwaard return a listB with the output to all the input
    def forward(self, inputs):
        listA = []
        listB = []
        for data in inputs:
            listA.append(self.forwardHelp(data, self.hidden))

        for data in listA:
            listB.append(self.forwardHelp(data, self.output))


        return listA, listB

    def transform(self, targets):
        tempList = []
        for data in targets:
            if data == [1,0,0,0,0,0,0,0]:
                tempList.append(1)
            elif data == [0,1,0,0,0,0,0,0]:
                tempList.append(2)
            elif data == [0,0,1,0,0,0,0,0]:
                tempList.append(3)
            elif data == [0,0,0,1,0,0,0,0]:
                tempList.append(4)
            elif data == [0,0,0,0,1,0,0,0]:
                tempList.append(5)
            elif data == [0,0,0,0,0,1,0,0]:
                tempList.append(6)
            elif data == [0,0,0,0,0,0,1,0]:
                tempList.append(7)
            elif data == [0,0,0,0,0,0,0,1]:
                tempList.append(8);
        return tempList

    def transform2(self, inputs, targets):
        listInputs = []
        listTargets = []
        bool = True
        for i in range(len(inputs)):
            bool = True
            for x in range(len(inputs[i])):
                if inputs[i][x] == 1.0:
                    listInputs.append(x+1)
                    listTargets.append(targets[i])
                    bool = False
            if bool:
                listInputs.append(0)
                listTargets.append(targets[i])
        return listInputs, listTargets

    def confusion(self, inputs, targets):
        listA, listB = self.forward(inputs)

        listB2 = self.changeOutput(listB)
        targets2 = np.array(targets).tolist()

        #self.testOutput(listB, targets, True)


        targets2 = self.transform(targets2)
        listB2, targets2 = self.transform2(listB2, targets2)

        #self.testOutput(listB, targets, True)

        actual = pd.Series(listB2, name='Actual')
        predicted = pd.Series(targets2, name='Predicted')
        df_confusion = pd.crosstab(actual, predicted, rownames=['Actual'], colnames=['Predicted'], margins=True)

        df_conf_norm = df_confusion.div(df_confusion.sum(axis=1), axis=0)
        #df_conf_norm = df_confusion / df_confusion.sum(axis=1)
        self.testOutput(listB, targets, False)
        print(df_confusion)
        print(df_conf_norm)

def sigmoid(x):
    a = 1
    return 1 / (1 + math.exp(-x))
