import struct
import os

from select import select

def open(f, mode):
    if mode == 'r': return os.open(f, os.O_RDONLY)
    if mode == 'w': return os.open(f, os.O_WRONLY)
    raise Exception(f'unknown mode {mode}')

def close(f):
    os.close(f)

def send(f, data, tag=0):
    size = len(data)
    os.write(f, struct.pack('<Lb', size, tag))
    assert os.write(f, bytes(data, 'utf-8')) == size

def recv(f):
    header_len = 5
    buf = os.read(f, header_len)
    while len(buf) < header_len:
        select([f], [], [f])
        buf += os.read(f, header_len - len(buf))
    size, tag = struct.unpack('<Lb', buf)
    data = os.read(f, size)
    while len(data) < size:
        select([f], [], [f])
        data += os.read(f, size - len(data))
    data = data.decode('utf-8')
    return data, tag

