import struct
import os
import sys
import numpy as np

from select import select

type_size   = {0:1, 1:4, 2:1, 3:4}
struct_type = {0:'<B', 1:'<L', 2:'<b', 3:'<l'}
numpy_type  = {0:'<u1', 1:'<u4', 2:'<i1', 3:'<i4'}

def hexdump(bytes):
    return ' '.join(f'{x:02X}' for x in bytes)

def panic(*args, file=sys.stderr, **kwargs):
    print(*args, file=file, **kwargs)
    sys.exit(1)

def open(f, mode):
    if mode == 'r': return os.open(f, os.O_RDONLY)
    if mode == 'w': return os.open(f, os.O_WRONLY)
    panic(f'unknown mode {mode}')

def close(f):
    os.close(f)

def send(f, tag, *args):
    type = bytearray()
    data = bytearray()
    
    for arg in args:
        if isinstance(arg, int):
            type += struct.pack('<B', 1)
            data += struct.pack('<L', arg)
            continue

        if isinstance(arg, list):
            arg = np.array(arg)
        elif isinstance(arg, str):
            arg = np.fromstring(arg, '<u1')

        if isinstance(arg, np.ndarray):
            dims = struct.pack('<B', len(arg.shape))[0]
            base, view = None, None
            if arg.dtype == np.uint8:
                base, view = 0, '<u1'
            elif arg.dtype == np.uint32 or arg.dtype == np.uint64:
                base, view = 1, '<u4'
            elif arg.dtype == np.int8:
                base, view = 2, '<i1'
            elif arg.dtype == np.int32 or arg.dtype == np.int64:
                base, view = 3, '<i4'
            data += arg.astype(np.dtype(view)).tobytes()
            type += struct.pack('<B', 1 << 7 | dims << 4 | base)
            for dim in arg.shape:
                type += struct.pack('<L', dim)
            # TODO: convert to little endian here

    type = struct.pack('<L', len(type)) + type
    tlen, dlen = len(type), len(data)
    head = struct.pack('<Lb', tlen + dlen, tag)
    sent = os.write(f, head + type + data)
    return sent

def recv(f):
    hlen = 5
    head = os.read(f, hlen)
    while len(head) < hlen:
        select([f], [], [f])
        head += os.read(f, hlen - len(head))
    size, tag = struct.unpack('<Lb', head)
    body = os.read(f, size)
    while len(body) < size:
        select([f], [], [f])
        body += os.read(f, size - len(body))

    tlen, = struct.unpack('<L', body[:4]); body = body[4:]
    type, body = body[:tlen], body[tlen:]
    dlen = size - hlen - tlen + 1
    data, body = body[:dlen], body[dlen:]

    args = []
    while type:
        argtype, type  = type[0], type[1:]
        isarr = argtype >> 7 & 0x01
        ndim  = argtype >> 4 & 0x07
        base  = argtype & 0x0F
        size  = type_size[base]
        sfmt  = struct_type[base]
        afmt  = numpy_type[base]
        if isarr:
            dims = []
            for x in range(ndim):
                argdim, type  = type[:4], type[4:]
                dim = struct.unpack('<L', argdim)[0]
                size *= dim
                dims.append(dim)
            arg, data = data[:size], data[size:]
            arg = np.frombuffer(arg, dtype=afmt)
            arg = arg.reshape(dims)
            if base == 0 and len(dims) == 1:
                arg = str(arg, encoding='utf-8')
            args.append(arg)

        else:
            arg, data = data[:size], data[size:]
            args.append(struct.unpack(sfmt, arg)[0]) 

    return (tag, args)

