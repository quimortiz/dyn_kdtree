import sys

sys.path.append(".")


import pydynotree as dynotree
import numpy as np
import time


num_points = 10000

a = dynotree.TreeRX(2)

a.addPoint(np.array([0, 0]), 0, True)
a.addPoint(np.array([0, 1]), 1, True)
a.addPoint(np.array([0, 2]), 2, True)
nn = a.search(np.array([0, 2.1]))
print(nn)
print(nn.distance)
print(nn.id)

b = dynotree.TreeR2(-1)
b.addPoint(np.array([0, 0]), 0, True)
b.addPoint(np.array([0, 1]), 1, True)
b.addPoint(np.array([0, 2]), 2, True)
nn = b.search(np.array([0, 2.1]))
print(nn)
print(nn.distance)
print(nn.id)

a = dynotree.TreeRX(2)
b = dynotree.TreeR2(-1)

for i in range(num_points):
    x = np.random.rand(2)
    a.addPoint(x, i, True)
    b.addPoint(x, i, True)

k = np.array([0.81, 0.15])

tic = time.time()
o = a.searchKnn(k, 5)
toc = time.time()
print("elapsed time: ", toc - tic)
# print(o[0].id)

tic = time.time()
oo = b.searchKnn(k, 5)
toc = time.time()
print("elapsed time: ", toc - tic)

# print(o[0].id)

a = dynotree.TreeRX(4)
b = dynotree.TreeR4(-1)

for i in range(num_points):
    x = np.random.rand(4)
    a.addPoint(x, i, True)
    b.addPoint(x, i, True)

k = np.array([0.81, 0.15, 0.1, 0.2])

tic = time.time()
o = a.searchKnn(k, 2)
toc = time.time()
print("elapsed time: ", toc - tic)
print(o[0].id)

tic = time.time()
o = b.searchKnn(k, 2)
toc = time.time()
print("elapsed time: ", toc - tic)
print(o[0].id)


a = dynotree.TreeRX(7)
b = dynotree.TreeR7(-1)

for i in range(num_points):
    x = np.random.rand(7)
    a.addPoint(x, i, True)
    b.addPoint(x, i, True)

k = np.array([0.81, 0.15, 0.1, 0.2, 0.1, 0.1, 0.5])

tic = time.time()
o = a.searchKnn(k, 2)
toc = time.time()
print("elapsed time: ", toc - tic)
print(o[0].id)

tic = time.time()
o = b.searchKnn(k, 2)
toc = time.time()
print("elapsed time: ", toc - tic)
print(o[0].id)
