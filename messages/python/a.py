import colosseum as col
import numpy as np
from time import time
from sys import argv
from random import randint

board = np.zeros((36, 6, 6)).astype(np.uint8)

f = col.open(argv[1], 'w')
while True:
    tag = randint(1, 3)
    t = time()
    if tag == 1:
        col.send(f, tag, 1024, 'hello, world!', [[1, 2], [3, 4]])
    if tag == 2:
        col.send(f, tag, 1, 2, [[[0]]], 3, 4)
    if tag == 3:
        col.send(f, tag, board)
    t = time() - t
    print(f'--> [tag {tag} time {t*1e9:.2f}]')
