import colosseum as col
import numpy as np
from sys import argv
from random import randint

f = col.open(argv[1], 'w')
while True:
    tag = randint(1, 2)
    if tag == 1:
        col.send(f, tag, 1024, 'hello, world!', [[1, 2], [3, 4]])
    if tag == 2:
        col.send(f, tag, 1, 2, [[[0]]], 3, 4)
    print(f'--> [tag {tag}]')
