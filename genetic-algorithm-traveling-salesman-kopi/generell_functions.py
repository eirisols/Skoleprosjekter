def calcPath(permutation, data):
    path = 0.0;
    for x in range (0, len(permutation)):
        if x == len(permutation)-1:
            path += float(data[permutation[x]+1][permutation[0]])
        else :
            path += float(data[permutation[x]+1][permutation[x+1]])
    return path

def orderCrossover(parent1, parent2, start, stop):
    child = [None] * len(parent1)

    child[start:stop] = parent1[start:stop]

    #hvor neste ligger i parent2
    temp1 = stop
    #hvor jeg skal sette neste
    temp2 = stop

    while None in child:
        if parent2[temp1] not in child:
            child[temp2] = parent2[temp1]
            if temp2 == len(child)-1:
                temp2 = 0
            else:
                temp2 += 1
        else:
            if temp1 == len(child)-1:
                temp1 = 0
            else:
                temp1 += 1
    return child
