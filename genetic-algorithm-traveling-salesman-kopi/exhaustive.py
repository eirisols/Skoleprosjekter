import csv
import numpy as np
from itertools import permutations
import time

start_time = time.time()

with open("european_cities.csv", "r") as f:
    data = list(csv.reader(f, delimiter=';'))

antallByer = 10

perm = permutations(range(antallByer))
permListe = list(perm)
strekning = 0
kortesteStrekning = np.inf


for i in range (0, len(permListe)):
    strekning = 0
    for x in range (0, antallByer):

        if x == antallByer-1:
            strekning = strekning + float(data[permListe[i][x] + 1][permListe[i][0]])
        else :
            strekning = strekning + float(data[permListe[i][x] + 1][permListe[i][x + 1]])
    if strekning < kortesteStrekning:
        kortesteStrekning = strekning
        kortstePerm = permListe[i]


print ('Dette er korteste strekning :', kortesteStrekning)
print (kortstePerm)
print ("%s sekunder" % (time.time() - start_time))
