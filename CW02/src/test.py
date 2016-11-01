from numpy import matrix
from numpy import linalg
import sys
import numpy as np

print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

arglen = len(sys.argv)
if arglen == 3:
    expr = sys.argv[1]
    filename = sys.argv[2]
else:
   raise ValueError('Arguments Error!') 

print 'FileName:', filename

print 'expr:', expr

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

def CalcPostfix(expr):
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

def SquareError(expr, points):
    n = len(points) - 1;
    y = points[n]
    points.pop(n)
    for i in range(n-1, -1, -1):
        expr = expr.replace(variables[i], str(points[i]))
    error = (CalcPostfix(expr) - y)
    return error * error

error = 0
for i in range(M):
   error = error + SquareError(expr, points[i]) 
error = error / M
print error
