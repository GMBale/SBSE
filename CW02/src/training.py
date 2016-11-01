from numpy import matrix
from numpy import linalg
import sys
import numpy as np

print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

arglen = len(sys.argv)
if arglen == 1:
    filename = "coursework2-training.csv"
elif arglen == 2:
    filename = sys.argv[1]
else:
   raise ValueError('Arguments Error!') 


print 'FileName:', filename

f = open(filename, 'r')
points = []

line = f.readline()
variables = line.split(',')
N = len(variables) - 1
variables[N] = variables[N][:-2]
y = []

print variables
while True:
    line = f.readline()
    if not line: break
    points.append(map(float, line.split(',')))
f.close()
M = len(points)

for i in range(M):
    y.append(points[i][N])
    points[i].pop(N)

X = []
for i in range(M):
    X.append([])
    X[i].append(1)
    for j in range(N):
        X[i].append(points[i][j])

X = matrix(X)
y = matrix(y).T

betaHat = np.linalg.inv(X.T.dot(X)).dot(X.T).dot(y)

print betaHat

ret = ""
b = betaHat[0].item()
#if b < 0:
#    ret = str(-b) + " ~ "
#else:
ret = str(b) + " "
for i in range(1, N+1):
    ret = ret + variables[i-1] + " "
    b = betaHat[i].item()
#    if b < 0:
#        ret = ret + str(-b) + " ~ "
#    else:
    ret = ret + str(b) + " "
    ret = ret + "* + "

print ret
