import colosseum as col
from sys import argv

f = col.open(argv[1], 'w')
while True:
    data, tag = 'hello, world!', 42
    col.send(f, data, tag)
    print(f'--> {data} [tag {tag}, size {len(data)}]')
