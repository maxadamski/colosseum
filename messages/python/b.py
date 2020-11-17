import colosseum as col
from sys import argv

f = col.open(argv[1], 'r')
while True:
    data, tag = col.recv(f)
    if not data: continue
    print(f'<-- {data} [tag {tag}, size {len(data)}]')
