import struct
import os
import sys

from select import select

def open(f, mode):
    if mode == 'r': return os.open(f, os.O_RDONLY)
    if mode == 'w': return os.open(f, os.O_WRONLY)
    raise Exception(f'unknown mode {mode}')

def close(f):
    os.close(f)

def panic(*args, file=sys.stderr, **kwargs):
    print(*args, file=file, **kwargs)

def send(f, data, tag=0):
    size = len(data)
    os.write(f, struct.pack('<Lb', size, tag))

    if isinstance(data, str):
        # TODO: convert to little endian here
        data = bytes(data, 'utf-8')
    elif isinstance(data, int):
        data = struct.pack('<L', data)

    if not isinstance(data, bytes):
        panic(f"Can't send object {data} of type {type(data)} (type not supported).")

    assert os.write(f, data) == size

def recv(f, astype=None):
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

    if astype == str:
        data = data.decode('utf-8')
    elif astype == int:
        data = struct.unpack('<L', data)

    return data, tag

