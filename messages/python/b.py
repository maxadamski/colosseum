import colosseum as col
from sys import argv

f = col.open(argv[1], 'r')
while True:
    data = col.recv(f)
    if not data: continue
    tag, res = data
    print(f'<-- [tag {tag}] {res}')
