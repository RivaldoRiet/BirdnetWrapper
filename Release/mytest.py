import math

def myabs(x):
    return math.fabs(x)

x = 1
if x == 1:
    # indented four spaces
    print("x was 1.")


def multiply(a, b):
    print ('Will compute', a, 'times', b)
    c = 7
    for i in range(0, a):
        c = c + b
    return c
