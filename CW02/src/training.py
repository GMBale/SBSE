from numpy import matrix
from numpy import linalg
import sys
import numpy as np
import math
import time
import random

def CrossOver(p, q):
    ret = []
    for i in range(len(p)):
        ret.append(p[i])
    for i in range(len(q)):
        ret.append(q[i])
    ret.append("+")
    ret.append("2")
    ret.append("/")
    return ret

def CalcPostfix(expr):
    if isinstance(expr, list):
        terms = expr
    else:
        terms = expr.split(' ')
    stack = []
    for i in range(len(terms)):
        t = terms[i]
        if len(stack) > 0:
            top = stack[len(stack) - 1]
        if len(stack) > 1:
            top2 = stack[len(stack) - 2]
        count = 0
        res = 0
        #Unary
        if t == "~":
            count = 1
            res = -top
        elif t == "abs":
            count = 1
            res = math.fabs(top)
        elif t == "sin":
            count = 1
            res = math.sin(top)
        elif t == "cos":
            count = 1
            res = math.cos(top)
        elif t == "tan":
            count = 1
            res = math.tan(top)
        elif t == "asin":
            count = 1
            res = math.asin(top)
        elif t == "acos":
            count = 1
            res = math.acos(top)
        elif t == "atan":
            count = 1
            res = math.atan(top)
        elif t == "sinh":
            count = 1
            res = math.sinh(top)
        elif t == "cosh":
            count = 1
            res = math.cosh(top)
        elif t == "tanh":
            count = 1
            res = math.tanh(top)
        elif t == "exp":
            count = 1
            res = math.exp(top)
        elif t == "sqrt":
            count = 1
            res = math.sqrt(top)
        elif t == "log":
            count = 1
            res = math.log(top)
        #Binary
        elif t == "+":
            count = 2
            res = top2 + top
        elif t == "-":
            count = 2
            res = top2 - top
        elif t == "-":
            count = 2
            res = top2 - top
        elif t == "*":
            count = 2
            res = top2 * top
        elif t == "/":
            count = 2
            res = top2 / top
        elif t == "^":
            count = 2
            res = math.pow(top2, top)
        else:
            res = float(t)
        for j in range(count):
            stack.pop(len(stack)-1)
        stack.append(res)
    if len(stack) != 1:
       raise ValueError('Postfix Error!') 
    return stack[0]

def Error(expr, points, calced):
    n = len(points) - 1;
    y = points[n]

    if isinstance(expr, list):
        exp = []
        for i in range(len(expr)):
            exp.append(expr[i])
            for j in range(n):
                if exp[i] == variables[j]:
                    exp[i] = str(points[j])
    else:
        exp = expr
        for i in range(n-1, -1, -1):
            exp = exp.replace(variables[i], str(points[i]))
    error = (calced + CalcPostfix(exp) - y)
    return error

flag = True

def Fitness(expr, points):
    error = 0
    M = len(points)
    for i in range(len(points)):
        if flag:
            err = Error(expr, points[i], 0) 
            calced[i] = err + points[i][len(points[i])-1]
        else:
            err = Error(expr, points[i], calced[i])
        error = error + (err * err)
    error = error / M
    return error


arglen = len(sys.argv)
if arglen == 1:
    filename = "coursework2-training.csv"
elif arglen == 2:
    filename = sys.argv[1]
else:
   raise ValueError('Arguments Error!') 


f = open(filename, 'r')
points = []

line = f.readline()
variables = line.split(',')
N = len(variables) - 1
variables[N] = variables[N][:-2]
y = []

while True:
    line = f.readline()
    if not line: break
    points.append(map(float, line.split(',')))
f.close()
M = len(points)

for i in range(M):
    y.append(points[i][N])

allLR = []
for i in range(M):
    allLR.append([])
    allLR[i].append(1)
    for j in range(N):
        allLR[i].append(points[i][j])

LRs = []
LRs_b = []

y = matrix(y).T

for j in range(N):
    LRs.append([])
    for i in range(M):
        LRs[j].append([])
        LRs[j][i].append(1)
        LRs[j][i].append(points[i][j])
    X = LRs[j] = matrix(LRs[j])
    LRs_b.append(np.linalg.inv(X.T.dot(X)).dot(X.T).dot(y))
    
X = allLR = matrix(allLR)

betaHat = np.linalg.inv(X.T.dot(X)).dot(X.T).dot(y)

ret = ""
b = betaHat[0].item()
#if b < 0:
#    ret = str(-b) + " ~ "
#else:

allLRArray = []
allLRArray.append(str(b))
for i in range(1, N+1):
#for i in range(1, 2):
    allLRArray.append(variables[i-1])
    b = betaHat[i].item()
#    if b < 0:
#        ret = ret + str(-b) + " ~ "
#    else:
    allLRArray.append(str(b))
    allLRArray.append("*")
    allLRArray.append("+")

LRsArray = []
for j in range(N):
    LRsArray.append([])
    LRsArray[j].append("0")
    LRsArray[j].append("1")
    LRsArray[j].append(str(math.pi * 2))
    LRsArray[j].append("1")
    LRsArray[j].append(variables[j])
    LRsArray[j].append("*")
    LRsArray[j].append("+")
    LRsArray[j].append("sin")
    LRsArray[j].append("*")
    LRsArray[j].append("+")

fits = [0] * N

calced = [0] * M
zeros = [0] * M

GMSE = Fitness(allLRArray, points)
flag = False
GEXPR = allLRArray
f = open('symbreg.txt', 'w')
f.write(" ".join(GEXPR))
f.close

population = []
for i in range(N):
    population.append(LRsArray[i])

startTime = time.time()
while(True):
    print time.time() - startTime, GMSE
    for i in range(len(population)):
        fits[i] = Fitness(population[i], points)
        if fits[i] < GMSE:
            GMSE = fits[i]
            GEXPR = population[i]
            f = open('symbreg.txt', 'w')
            print GEXPR + allLRArray + ["+"]
            f.write(" ".join(GEXPR + allLRArray + ["+"]))
            f.close
    for i in range(len(population)):
        r = random.uniform(0, 0.79)
        population[i][1] = str(r)
        population[i][0] = str(random.uniform(-r, r))
        population[i][2] = str(random.uniform(0, math.pi * 2))
        population[i][2] = str(random.uniform(-100, 100))

print " ".join(allLRArray)
